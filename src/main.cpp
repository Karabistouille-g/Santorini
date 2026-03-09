#include <iostream>
#include <string>
#include <limits>
#include <thread>
#include <chrono>

#define MINIAUDIO_IMPLEMENTATION
#include "../include/miniaudio.h"

#include "controller.hpp"
#include "view.hpp"
#include "board.hpp"

using namespace santorini;

ma_engine audioEngine;
bool audioInitialized = false;

void playSoundFX(const char* filepath) {
    if (audioInitialized) {
        ma_engine_play_sound(&audioEngine, filepath, NULL);
    }
}

int main(int argc, char* argv[]) {
    if (ma_engine_init(NULL, &audioEngine) == MA_SUCCESS) {
        audioInitialized = true;
    } else {
        std::cout << "[Audio] Attention : Pas de peripherique audio detecte." << std::endl;
    }

    bool isOnline = false;
    bool isServer = false;
    std::string ip = "127.0.0.1";
    int port = 5050;
    int aiChoice = 1; 
    
    std::string pseudo1 = "Joueur 1";
    std::string pseudo2 = "Joueur 2";

    if (argc > 1 && std::string(argv[1]) == "server") {
        isOnline = true;
        isServer = true;
        std::cout << "Entrez votre pseudo (Serveur) : ";
        std::cin >> pseudo1;
        pseudo2 = "Adversaire";
    } else if (argc > 1 && std::string(argv[1]) == "client") {
        isOnline = true;
        isServer = false;
        std::cout << "Entrez votre pseudo (Client) : ";
        std::cin >> pseudo2;
        pseudo1 = "Adversaire";
        if (argc > 2) ip = argv[2];
        if (argc > 3) port = std::stoi(argv[3]); 
    } else if (argc > 1 && std::string(argv[1]) == "solo") {
        isOnline = false;
        std::cout << "Entrez votre pseudo : ";
        std::cin >> pseudo1;
        pseudo2 = "Bob l'IA";
        std::cout << "--- CHOIX DU NIVEAU DE BOB ---\n1: Facile\n2: Moyen\n3: Difficile\nVotre choix : ";
        if (!(std::cin >> aiChoice)) aiChoice = 1;
    } else {
        std::cout << "Usage:\n  ./santorini server\n  ./santorini client [IP] [PORT]\n  ./santorini solo" << std::endl;
        if (audioInitialized) ma_engine_uninit(&audioEngine);
        return 0;
    }

    Controller& controller = Controller::getInstance();
    controller.setAIDifficulty(aiChoice);
    controller.setPlayerNames(pseudo1, pseudo2);

    if (controller.createGame(isOnline, isServer, ip, port) != 0) {
        if (audioInitialized) ma_engine_uninit(&audioEngine);
        return -1;
    }

    View& view = View::getInstance();
    GLFWwindow* window = view.getWindow();

    // --- CORRECTION CRUCIALE : ANTI-SEGFAULT ---
    // Si la machine du pote ne supporte pas OpenGL, window_ sera nul.
    // On l'empeche d'entrer dans la boucle sinon = Segfault !
    if (window == nullptr) {
        std::cerr << "\n[ ERREUR CRITIQUE ] Impossible de lancer l'interface graphique !" << std::endl;
        std::cerr << "-> Verifie que ta carte graphique supporte OpenGL 3.3." << std::endl;
        if (audioInitialized) ma_engine_uninit(&audioEngine);
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        view.viewBoard(); 

        if (!view.isWon()) {
            view.processInput(window, controller);

            if (isOnline) {
                controller.processNetwork();
            } else {
                controller.processAI();
            }
        } else {
            view.processInput(window, controller); 
            view.winner(true, controller.getCurrentPlayer());
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    if (audioInitialized) {
        ma_engine_uninit(&audioEngine);
    }
    return 0;
}