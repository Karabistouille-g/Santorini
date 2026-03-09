#include "controller.hpp"
#include "model.hpp"
#include "view.hpp"
#include "network_manager.hpp"
#include "bob.hpp"
#include "builder.hpp"
#include "case.hpp"
#include <iostream>
#include <thread>
#include <atomic>

namespace santorini {

Controller& Controller::getInstance() {
    static Controller instance;
    return instance;
}

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
    model_->startGame(0, 1); 

    isOnlineMode_ = isOnline;

    if (isOnlineMode_) {
        if (isServer) {
            myPlayerId_ = 0;
            isMyTurn_ = true;
            // Lancer accept() dans un thread pour ne pas bloquer GLFW
            std::cout << "--- SERVER MODE: En attente d'un client sur le port " << port << " ---" << std::endl;
            std::thread([this, port]() {
                if (net_->startServer(port)) {
                    std::cout << "[Network] Client connecte ! La partie commence." << std::endl;
                } else {
                    std::cerr << "[Network] Erreur serveur." << std::endl;
                }
            }).detach();
        } else {
            myPlayerId_ = 1;
            isMyTurn_ = false;
            std::cout << "--- CLIENT: Connexion a " << ip << ":" << port << " ---" << std::endl;
            std::thread([this, ip, port]() {
                if (net_->connectToClient(ip, port)) {
                    std::cout << "[Network] Connecte au serveur ! La partie commence." << std::endl;
                } else {
                    std::cerr << "[Network] Erreur de connexion." << std::endl;
                }
            }).detach();
        }
    } else {
        myPlayerId_ = 0; 
        isMyTurn_ = true;
    }
    return 0;
}

int Controller::selectMove(int pawnId, int x, int y) {
    if (isOnlineMode_ && !isMyTurn_) return 0;

    Builder* pawn = model_->getPawn(myPlayerId_, pawnId);
    if (!pawn) return 0;

    bool success = pawn->moveBuilder(x, y);

    if (success) {
        if (isOnlineMode_) {
            Packet p{ActionType::MOVE, pawnId, x, y};
            net_->sendPacket(p);
        }
        if (pawn->getPosition()->getFloor() == 3) return 2;
        return 1;
    }
    return 0;
}

bool Controller::selectBuild(int pawnId, int x, int y) {
    if (isOnlineMode_ && !isMyTurn_) return false;

    Builder* pawn = model_->getPawn(myPlayerId_, pawnId);
    if (!pawn) return false;

    bool success = pawn->createBuild(x, y);

    if (success) {
        if (isOnlineMode_) {
            Packet p{ActionType::BUILD, pawnId, x, y};
            net_->sendPacket(p);
            isMyTurn_ = false;
        } else {
             model_->nextTurn();
             isMyTurn_ = false; 
        }
        return true;
    }
    return false;
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
                enemyPawn->moveBuilder(p.x, p.y);
                if (enemyPawn->getPosition()->getFloor() == 3) {
                    // MODIF : On utilise setWinner pour bien afficher le nom de l'adversaire
                    View::getInstance().setWinner(enemyId); 
                }
            } 
            else if (p.type == ActionType::BUILD) {
                enemyPawn->createBuild(p.x, p.y);
                isMyTurn_ = true;
                std::cout << "--- C'EST VOTRE TOUR ---" << std::endl;
                model_->printTerminalBoard();
            }   
        }
    }
}

//pour savoir si l'IA réfléchit déjà
static std::atomic<bool> aiThinking{false};

void Controller::processAI() {
    // Si c'est au tour de Bob (Joueur 1) et qu'il ne réfléchit pas déjà
    if (!isOnlineMode_ && model_->getCurrentPlayer() == 1 && !aiThinking) {
        
        aiThinking = true; // On bloque les futurs appels
        
        // MODIF : On retire le thread détaché ici pour éviter les crashs graphiques (synchronisation)
        int depth = (aiDifficulty_ == 1) ? 1 : (aiDifficulty_ == 2 ? 3 : 5);
        std::cout << "[IA] Bob reflechit (Profondeur " << depth << ")..." << std::endl;
        
        Bob bob(aiDifficulty_);
        bob.playTurn(); 
        
        model_->nextTurn();
        this->isMyTurn_ = true; 
        
        std::cout << "[IA] Bob a fini de jouer." << std::endl;
        aiThinking = false; // On libère le flag
    }
}

int Controller::getCurrentPlayer() { return myPlayerId_; }

} // namespace santorini