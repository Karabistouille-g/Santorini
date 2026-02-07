#include <iostream>
#include <string>
#include <limits>

#include "controller.hpp"
#include "view.hpp"
#include "board.hpp"

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

    std::cout << "Main ligne 40" << std::endl;

    char cmd;
    int cpt = 0;
    int id, x, y;
    View& view = View::getInstance();
    GLFWwindow* window = view.getWindow();

    // 3. Boucle principale
    while (!glfwWindowShouldClose(window)) {

        controller.processNetwork();
        
        view.processInput(window, controller);

        if (Board::getInstance() != nullptr) {
            view.viewBoard();
        }

        glfwPollEvents();
        glfwSwapBuffers(window);

        cpt++;
    }

    return 0;
}