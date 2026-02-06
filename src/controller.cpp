#include "../include/controller.hpp"
#include "../include/model.hpp"
#include "../include/network_manager.hpp"
#include <iostream>
#include <view.hpp>

namespace santorini {

Controller::Controller() 
    : model_(std::make_unique<Model>()), 
      net_(std::make_unique<NetworkManager>()),
      isOnlineMode_(false),
      isMyTurn_(false),
      myPlayerId_(-1) {
}

Controller::~Controller() {}

// On récupère le port ici
int Controller::createGame(bool isOnline, bool isServer, const std::string& ip, int port) {
    model_->startGame(1, 2); 

    isOnlineMode_ = isOnline;

    if (isOnlineMode_) {
        if (isServer) {
            // Le serveur écoute toujours sur son port local 5050
            std::cout << "--- SERVER MODE: Listening on port 5050 ---" << std::endl;
            if (!net_->startServer(5050)) return -1;
            myPlayerId_ = 0;
            isMyTurn_ = true; 
        } else {
            // Le client se connecte à l'IP et au PORT donnés (ceux de Ngrok potentiellement)
            std::cout << "--- CLIENT MODE: Connecting to " << ip << ":" << port << " ---" << std::endl;
            if (!net_->connectToClient(ip, port)) return -1;
            myPlayerId_ = 1;
            isMyTurn_ = false;
        }
    } else {
        myPlayerId_ = 0; 
        isMyTurn_ = true;
    }
    return 0;
}

bool Controller::selectMove(int pawnId, int x, int y) {
    if (isOnlineMode_ && !isMyTurn_) {
        std::cout << "[Game] Not your turn!" << std::endl;
        return false;
    }

    Builder* pawn = model_->getPawn(myPlayerId_, pawnId);
    if (!pawn) {
        std::cout << "[Error] Pawn not found!" << std::endl;
        return false;
    }

    bool success = pawn->moveBuilder(x, y);
    
    if (success) {
        std::cout << "[Game] Move Validated!" << std::endl;
        if (isOnlineMode_) {
            Packet p{ActionType::MOVE, pawnId, x, y};
            net_->sendPacket(p);
        }
        return true;
    } else {
        std::cout << "[Game] Invalid Move (Check rules or collisions)" << std::endl;
        return false;
    }
}

bool Controller::selectBuild(int pawnId, int x, int y) {
    if (isOnlineMode_ && !isMyTurn_) {
        std::cout << "[Game] Not your turn!" << std::endl;
        return false;
    }

    Builder* pawn = model_->getPawn(myPlayerId_, pawnId);
    if (!pawn) return false;

    bool success = pawn->createBuild(x, y);

    if (success) {
        std::cout << "[Game] Build Validated!" << std::endl;
        if (isOnlineMode_) {
            Packet p{ActionType::BUILD, pawnId, x, y};
            net_->sendPacket(p);
            isMyTurn_ = false;
            std::cout << "--- END OF TURN (Waiting for opponent) ---" << std::endl;
            View::getInstance().viewBoard(false);
        }
        return true;
    } else {
        std::cout << "[Game] Invalid Build" << std::endl;
        return false;
    }
}

void Controller::processNetwork() {
    if (!isOnlineMode_ || isMyTurn_) return;

    auto pktOpt = net_->receivePacket();
    if (pktOpt.has_value()) {
        Packet p = pktOpt.value();
        int enemyId = (myPlayerId_ == 0) ? 1 : 0;
        Builder* enemyPawn = model_->getPawn(enemyId, p.workerId);

        if (enemyPawn) {
            if (p.type == ActionType::MOVE) {
                std::cout << "[Network] Opponent Moved Pawn " << p.workerId << std::endl;
                enemyPawn->moveBuilder(p.x, p.y);
            } 
            else if (p.type == ActionType::BUILD) {
                std::cout << "[Network] Opponent Built at " << p.x << "," << p.y << std::endl;
                enemyPawn->createBuild(p.x, p.y);
                isMyTurn_ = true;
                std::cout << "--- YOUR TURN ---" << std::endl;
            }
        }
    }
}

}