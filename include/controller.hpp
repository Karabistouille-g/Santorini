#pragma once

#include <memory>
#include <string>
#include <vector>
#include <atomic>

//Sortie du namespace pour éviter le conflit "incomplete type"
class Model;
class View;

namespace santorini {

class NetworkManager;

class Controller {
public:
    static Controller& getInstance();
    
    // Setup
    int createGame(bool isOnline, bool isServer, const std::string& ip, int port);
    void setAIDifficulty(int lvl) { aiDifficulty_ = lvl; }
    
    // Pseudos
    void setPlayerNames(const std::string& p1, const std::string& p2) {
        playerNames_[0] = p1;
        playerNames_[1] = p2;
    }
    std::string getPlayerName(int id) { return playerNames_[id]; }

    // Game Logic
    int selectMove(int pawnId, int x, int y);
    bool selectBuild(int pawnId, int x, int y);
    
    // Systems
    void processNetwork();
    void processAI();
    
    // Getters
    int getCurrentPlayer();
    bool isMyTurn() { return isMyTurn_; }
    bool isOnlineMode() { return isOnlineMode_; }

private:
    Controller();
    ~Controller();
    Controller(const Controller&) = delete;
    void operator=(const Controller&) = delete;

    std::unique_ptr<Model> model_;
    std::unique_ptr<NetworkManager> net_;
    
    bool isOnlineMode_;
    bool isMyTurn_;
    int myPlayerId_; // 0 ou 1
    int aiDifficulty_;
    std::atomic<bool> networkReady_{false};
    std::atomic<bool> aiThinking_{false};
    
    std::string playerNames_[2]; // Stockage des noms
};

}