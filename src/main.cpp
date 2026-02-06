#include <iostream>
#include <string>
#include <limits>
#include "../include/controller.hpp"
#include <view.hpp>

using namespace santorini;

int main(int argc, char* argv[]) {

    Controller controller;
    bool isServer = false;
    std::string ip = "127.0.0.1";
    int port = 5050; // Port par défaut

    // 1. Lecture des arguments
    if (argc > 1 && std::string(argv[1]) == "server") {
        isServer = true;
    } else if (argc > 2 && std::string(argv[1]) == "client") {
        ip = argv[2];
        // Si un 4ème argument est donné, c'est le port (utile pour Ngrok)
        if (argc > 3) {
            port = std::stoi(argv[3]);
        }
    } else {
        std::cout << "Usage:" << std::endl;
        std::cout << "  ./santorini server" << std::endl;
        std::cout << "  ./santorini client [IP] [PORT (optionnel)]" << std::endl;
        return 0;
    }

    // 2. Démarrage du jeu avec le port
    if (controller.createGame(true, isServer, ip, port) != 0) {
        std::cerr << "Erreur au démarrage du jeu." << std::endl;
        return -1;
    }

    std::cout << "\n=== JEU LANCE ===" << std::endl;
    std::cout << "Commandes :" << std::endl;
    std::cout << " - 'm' : Move (Deplacer)" << std::endl;
    std::cout << " - 'b' : Build (Construire)" << std::endl;
    std::cout << " - 'r' : Refresh (Voir si l'adversaire a joue)" << std::endl;

    char cmd;
    int id, x, y;

    // 3. Boucle principale
    while (true) {
        controller.processNetwork();
        View::getInstance().viewBoard(false);

        std::cout << "\nAction (m/b/r) > ";
        std::cin >> cmd;

        if (cmd == 'r') {
            std::cout << "Verification du reseau..." << std::endl;
            continue; 
        }
        else if (cmd == 'm') {
            std::cout << "Format: [ID_PION] [X] [Y] (ex: 0 1 1) > ";
            if (std::cin >> id >> x >> y) {
                controller.selectMove(id, x, y);
            } else {
                std::cout << "Erreur de saisie." << std::endl;
                std::cin.clear(); 
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
        else if (cmd == 'b') {
            std::cout << "Format: [ID_PION] [X] [Y] (ex: 0 1 1) > ";
            if (std::cin >> id >> x >> y) {
                controller.selectBuild(id, x, y);
            } else {
                std::cout << "Erreur de saisie." << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }

    return 0;
}