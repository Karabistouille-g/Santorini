#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <memory>
#include <string>
#include "common.hpp"

// Forward declaration
class Model; 

namespace santorini {

class NetworkManager;

class Controller {
public:
    Controller();
    ~Controller();

    // On ajoute le paramètre "port" ici (par défaut 5050)
    int createGame(bool isOnline, bool isServer, const std::string& ip = "", int port = 5050);

    bool selectMove(int pawnId, int x, int y);
    bool selectBuild(int pawnId, int x, int y);
    
    void processNetwork();

private:
    std::unique_ptr<Model> model_;
    std::unique_ptr<NetworkManager> net_;

    bool isOnlineMode_;
    bool isMyTurn_;
    int myPlayerId_;
};

}

#endif