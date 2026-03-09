#include <iostream>
#include <string>
#include <limits>
#include <thread>
#include <chrono>

// MAGIE AUDIO : MINIAUDIO
//compile le moteur audio directement dans le main 
#define MINIAUDIO_IMPLEMENTATION
#include "../include/miniaudio.h"

#include "controller.hpp"
#include "view.hpp"
#include "board.hpp"

using namespace santorini;

//MOTEUR AUDIO GLOBAL
ma_engine audioEngine;
bool audioInitialized = false;

// Fonction globale utilisable par le fichier view.cpp pour jouer un son
void playSoundFX(const char* filepath) {
    if (audioInitialized) {
        ma_engine_play_sound(&audioEngine, filepath, NULL);
    }
}

int main(int argc, char* argv[]) {
    // INITIALISATION AUDIO
    if (ma_engine_init(NULL, &audioEngine) == MA_SUCCESS) {
        audioInitialized = true;
    } else {
        std::cout << "[Audio] Attention : Pas de peripherique audio detecte (Le jeu continuera sans son)." << std::endl;
    }

    bool isOnline = false;
    bool isServer = false;
    std::string ip = "127.0.0.1";
    int port = 5050;
    int aiChoice = 1; // Valeur par défaut
    
    std::string pseudo1 = "Joueur 1";
    std::string pseudo2 = "Joueur 2";

    // GESTION DES ARGUMENTS ET SAISIE AVANT TOUTE FENÊTRE
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

    // INITIALISATION (La fenêtre s'ouvre)
    Controller& controller = Controller::getInstance();
    controller.setAIDifficulty(aiChoice);
    controller.setPlayerNames(pseudo1, pseudo2);

    if (controller.createGame(isOnline, isServer, ip, port) != 0) {
        if (audioInitialized) ma_engine_uninit(&audioEngine);
        return -1;
    }

    View& view = View::getInstance();
    GLFWwindow* window = view.getWindow();

    // BOUCLE PRINCIPALE
    while (!glfwWindowShouldClose(window)) {
        //vider le buffer
        view.viewBoard(); 

        if (!view.isWon()) {
            view.processInput(window, controller);

            if (isOnline) {
                controller.processNetwork();
            } else {
                // L'IA doit être appelée mais ne doit pas bloquer
                controller.processAI();
            }
        } else {
            view.processInput(window, controller); 
            view.winner(true, controller.getCurrentPlayer());
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Limiteur FPS
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    // On detruit le moteur audio
    if (audioInitialized) {
        ma_engine_uninit(&audioEngine);
    }
    return 0;
}