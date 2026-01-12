#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <memory>
#include <string>
#include "network_manager.hpp"

// Forward declarations (on dit juste que les classes existent)
class Model;  // Classe de ton collègue
class View;   // Classe de l'autre collègue (supposée)

namespace santorini {

class Controller {
public:
    Controller();
    ~Controller();

    /**
     * @brief Initialise le jeu et la connexion réseau
     * @param isOnline true pour jouer en réseau
     * @param isServer true si on héberge
     * @param ip IP cible (seulement si isServer = false)
     * @return 0 si succès, -1 si erreur
     */
    int createGame(bool isOnline, bool isServer, const std::string& ip = "127.0.0.1");

    /**
     * @brief Vérifie le réseau (à appeler dans la boucle principale)
     */
    void processNetwork();

private:
    std::unique_ptr<Model> model_;
    // std::unique_ptr<View> view_; a decommenter quand thibault aura fait View
    std::unique_ptr<NetworkManager> net_;

    bool isOnlineMode_;
    bool isMyTurn_;
};

}

#endif // CONTROLLER_HPP