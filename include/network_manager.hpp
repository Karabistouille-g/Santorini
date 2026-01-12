#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include "common.hpp"
#include <string>
#include <optional> // C++17 feature, très pratique

namespace santorini {

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();

    /**
     * @brief Configure le jeu en mode SERVEUR (attend une connexion)
     * @param port Le port d'écoute (ex: 8080)
     * @return true si un client s'est connecté avec succès
     */
    bool startServer(int port);

    /**
     * @brief Configure le jeu en mode CLIENT (se connecte à quelqu'un)
     * @param ip L'adresse IP du serveur (ex: "127.0.0.1")
     * @param port Le port (ex: 8080)
     * @return true si connecté
     */
    bool connectToClient(const std::string& ip, int port);

    /**
     * @brief Envoie une action à l'adversaire
     */
    bool sendPacket(const Packet& packet);

    /**
     * @brief Reçoit une action (Bloquant : attend jusqu'a recevoir)
     * @return Un packet optionnel (vide si erreur/déconnexion)
     */
    std::optional<Packet> receivePacket();

    bool isConnected() const;

private:
    int socketFd_;     // Le socket principal
    int connectionFd_; // Le socket de la connexion active (pour le serveur)
    bool isServer_;
    bool connected_;
};

}

#endif // NETWORK_MANAGER_HPP