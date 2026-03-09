#include "network_manager.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h> // Indispensable pour getaddrinfo
#include <fcntl.h>
#include <arpa/inet.h>

namespace santorini {

NetworkManager::NetworkManager() : socketFd_(-1), connectionFd_(-1), isServer_(false), connected_(false) {}

NetworkManager::~NetworkManager() {
    if (connected_) { close(connectionFd_); if (isServer_) close(socketFd_); }
}

bool NetworkManager::startServer(int port) {
    socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd_ == -1) return false;

    int opt = 1;
    setsockopt(socketFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(socketFd_, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) return false;

    listen(socketFd_, 1);
    
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    connectionFd_ = accept(socketFd_, (struct sockaddr*)&clientAddr, &clientLen);

    if (connectionFd_ < 0) return false;

    int flags = fcntl(connectionFd_, F_GETFL, 0);
    fcntl(connectionFd_, F_SETFL, flags | O_NONBLOCK);

    isServer_ = true;
    connected_ = true;
    return true;
}

bool NetworkManager::connectToClient(const std::string& host, int port) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;

    std::string portStr = std::to_string(port);
    if (getaddrinfo(host.c_str(), portStr.c_str(), &hints, &res) != 0) {
        std::cerr << "[Network] Impossible de resoudre l'hote: " << host << std::endl;
        return false;
    }

    socketFd_ = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (socketFd_ == -1) {
        freeaddrinfo(res);
        return false;
    }

    if (connect(socketFd_, res->ai_addr, res->ai_addrlen) < 0) {
        perror("[Network] Echec connexion");
        close(socketFd_);
        freeaddrinfo(res);
        return false;
    }

    freeaddrinfo(res);

    connectionFd_ = socketFd_;
    
    int flags = fcntl(connectionFd_, F_GETFL, 0);
    fcntl(connectionFd_, F_SETFL, flags | O_NONBLOCK);

    isServer_ = false;
    connected_ = true;
    return true;
}

bool NetworkManager::sendPacket(const Packet& packet) {
    if (!connected_) return false;
    ssize_t sent = send(connectionFd_, &packet, sizeof(Packet), 0);
    return sent == sizeof(Packet);
}

std::optional<Packet> NetworkManager::receivePacket() {
    if (!connected_) return std::nullopt;
    Packet pkt;
    
    // CORRECTION ICI : MSG_DONTWAIT garantit que cette ligne ne gèlera jamais le jeu
    ssize_t valread = recv(connectionFd_, &pkt, sizeof(Packet), MSG_DONTWAIT);
    
    if (valread > 0) return pkt;
    else if (valread == 0) { connected_ = false; return std::nullopt; }
    
    return std::nullopt; 
}

bool NetworkManager::isConnected() const { return connected_; }

}