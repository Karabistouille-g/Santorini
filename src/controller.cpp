#include "controller.hpp"
#include "model.hpp"
#include "view.hpp"
#include "network_manager.hpp"
#include "bob.hpp"
#include "builder.hpp"
#include "case.hpp"
#include <iostream>
#include <thread>

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
      myPlayerId_(-1),
      aiDifficulty_(1)
{}

Controller::~Controller() {}

int Controller::createGame(bool isOnline, bool isServer, const std::string& ip, int port) {
    model_->startGame(0, 1);
    isOnlineMode_ = isOnline;

    if (isOnlineMode_) {
        networkReady_ = false;
        if (isServer) {
            myPlayerId_ = 0;
            isMyTurn_   = true;
            std::cout << "--- SERVER MODE: En attente du client sur le port " << port << " ---" << std::endl;
            std::thread([this, port]() {
                if (net_->startServer(port)) {
                    std::cout << "[Network] Client connecte ! La partie commence." << std::endl;
                    networkReady_ = true;
                } else {
                    std::cerr << "[Network] Erreur serveur." << std::endl;
                }
            }).detach();
        } else {
            myPlayerId_ = 1;
            isMyTurn_   = false;
            std::cout << "--- CLIENT: Connexion a " << ip << ":" << port << " ---" << std::endl;
            std::thread([this, ip, port]() {
                if (net_->connectToClient(ip, port)) {
                    std::cout << "[Network] Connecte au serveur ! La partie commence." << std::endl;
                    networkReady_ = true;
                } else {
                    std::cerr << "[Network] Erreur de connexion." << std::endl;
                }
            }).detach();
        }
    } else {
        myPlayerId_   = 0;
        isMyTurn_     = true;
        networkReady_ = true;
    }
    return 0;
}

int Controller::selectMove(int pawnId, int x, int y) {
    if (isOnlineMode_ && (!isMyTurn_ || !networkReady_)) return 0;

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
    if (isOnlineMode_ && (!isMyTurn_ || !networkReady_)) return false;

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

    // Pas encore connecte : afficher dans le titre
    if (!networkReady_) {
        glfwSetWindowTitle(View::getInstance().getWindow(),
            myPlayerId_ == 0
            ? "Santorini  |  [SERVEUR] En attente du client..."
            : "Santorini  |  [CLIENT] Connexion en cours...");
        return;
    }

    auto pktOpt = net_->receivePacket();
    if (!pktOpt.has_value()) return;

    Packet p    = pktOpt.value();
    int enemyId = (myPlayerId_ == 0) ? 1 : 0;
    Builder* pawn = model_->getPawn(enemyId, p.workerId);
    if (!pawn) return;

    if (p.type == ActionType::MOVE) {
        pawn->moveBuilder(p.x, p.y);
        if (pawn->getPosition()->getFloor() == 3)
            View::getInstance().setWinner(enemyId);
    }
    else if (p.type == ActionType::BUILD) {
        pawn->createBuild(p.x, p.y);
        isMyTurn_ = true;
        std::cout << "--- C'EST VOTRE TOUR ---" << std::endl;
        model_->printTerminalBoard();
    }
}

void Controller::processAI() {
    if (isOnlineMode_ || model_->getCurrentPlayer() != 1 || aiThinking_)
        return;

    aiThinking_ = true;
    int diff = aiDifficulty_;

    // FIX FREEZE : l'IA tourne dans un thread separe pour ne pas geler GLFW.
    // depth 4 max (depth 5 = plusieurs secondes de calcul = "ne repond pas")
    std::thread([this, diff]() {
        int depth = (diff == 1) ? 1 : (diff == 2 ? 3 : 4);
        std::cout << "[IA] Bob reflechit (profondeur " << depth << ")..." << std::endl;

        Bob bob(diff);
        bob.playTurn();

        // FIX WIN IA : vérifier si l'IA a atteint le niveau 3 apres son deplacement
        for (int i = 0; i < 2; i++) {
            Builder* pawn = model_->getPawn(1, i);
            if (pawn && pawn->getPosition()->getFloor() == 3) {
                std::cout << "[IA] Bob a GAGNE !" << std::endl;
                View::getInstance().setWinner(1);
                aiThinking_ = false;
                return;
            }
        }

        model_->nextTurn();
        isMyTurn_ = true;
        std::cout << "[IA] Bob a fini de jouer." << std::endl;
        aiThinking_ = false;
    }).detach();
}

int Controller::getCurrentPlayer() { return myPlayerId_; }

} // namespace santorini