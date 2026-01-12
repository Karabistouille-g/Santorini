#include "network_manager.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>     // close
#include <sys/socket.h> // socket, bind, listen...
#include <arpa/inet.h>  // inet_addr

namespace santorini {

NetworkManager::NetworkManager() 
    : socketFd_(-1), connectionFd_(-1), isServer_(false), connected_(false) {}

NetworkManager::~NetworkManager() {
    if (connected_) {
        close(connectionFd_);
        if (isServer_) close(socketFd_);
    }
}

bool NetworkManager::startServer(int port) {
    // 1. Création du socket
    socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd_ == -1) {
        perror("[Network] Error creating socket");
        return false;
    }

    // Option pour réutiliser le port tout de suite après arrêt (évite l'erreur "Address already in use")
    int opt = 1;
    setsockopt(socketFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. Configuration de l'adresse
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Accepte toutes les IPs locales
    serverAddr.sin_port = htons(port);

    // 3. Bind (Lier le socket au port)
    if (bind(socketFd_, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("[Network] Bind failed");
        return false;
    }

    // 4. Listen (Mise en écoute)
    listen(socketFd_, 1);
    std::cout << "[Network] Server listening on port " << port << "..." << std::endl;

    // 5. Accept (Attente bloquante d'un client)
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    connectionFd_ = accept(socketFd_, (struct sockaddr*)&clientAddr, &clientLen);

    if (connectionFd_ < 0) {
        perror("[Network] Accept failed");
        return false;
    }

    std::cout << "[Network] Client connected!" << std::endl;
    isServer_ = true;
    connected_ = true;
    return true;
}

bool NetworkManager::connectToClient(const std::string& ip, int port) {
    // 1 création du socket
    socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd_ == -1) return false;

    // 2 configuration de la cible
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    // Conversion String -> IP Adresse
    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "[Network] Invalid address/ Address not supported" << std::endl;
        return false;
    }

    // 3. Connect (Tentative de connexion)
    std::cout << "[Network] Connecting to " << ip << ":" << port << "..." << std::endl;
    if (connect(socketFd_, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("[Network] Connection failed");
        return false;
    }

    // En mode client le socket de communication est le socket principal
    connectionFd_ = socketFd_;
    isServer_ = false;
    connected_ = true;
    std::cout << "[Network] Connected to server!" << std::endl;
    return true;
}

bool NetworkManager::sendPacket(const Packet& packet) {
    if (!connected_) return false;
    // On envoie la structure brute
    ssize_t sent = send(connectionFd_, &packet, sizeof(Packet), 0);
    return sent == sizeof(Packet);
}

std::optional<Packet> NetworkManager::receivePacket() {
    if (!connected_) return std::nullopt;

    Packet pkt;
    // recv est bloquant par défaut ici
    ssize_t valread = recv(connectionFd_, &pkt, sizeof(Packet), 0);

    if (valread > 0) {
        return pkt;
    } else {
        // 0 = déconnexion, -1 = erreur
        std::cout << "[Network] Connection lost." << std::endl;
        connected_ = false;
        return std::nullopt;
    }
}

bool NetworkManager::isConnected() const {
    return connected_;
}

}