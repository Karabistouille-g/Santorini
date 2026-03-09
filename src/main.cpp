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
bool      audioInitialized = false;
ma_sound  bgMusic;
bool      bgMusicLoaded = false;

void playSoundFX(const char* filepath) {
    if (audioInitialized) {
        std::cout << "🎵 [AUDIO] Lecture de : " << filepath << std::endl; // LE MOUCHARD
        ma_engine_play_sound(&audioEngine, filepath, NULL);
    }
}

int main(int argc, char* argv[]) {

    // --- AUDIO ---
    if (ma_engine_init(NULL, &audioEngine) == MA_SUCCESS) {
        audioInitialized = true;
    } else {
        std::cout << "[Audio] Pas de peripherique audio (le jeu continuera sans son)." << std::endl;
    }

    bool isOnline = false, isServer = false;
    std::string ip = "127.0.0.1";
    int port = 5050, aiChoice = 1;
    std::string pseudo1 = "Joueur 1", pseudo2 = "Joueur 2";

    if (argc > 1 && std::string(argv[1]) == "server") {
        isOnline = true; isServer = true;
        std::cout << "Entrez votre pseudo (Serveur) : "; std::cin >> pseudo1;
        pseudo2 = "Adversaire";
    } else if (argc > 1 && std::string(argv[1]) == "client") {
        isOnline = true; isServer = false;
        std::cout << "Entrez votre pseudo (Client) : "; std::cin >> pseudo2;
        pseudo1 = "Adversaire";
        if (argc > 2) ip = argv[2];
        if (argc > 3) port = std::stoi(argv[3]);
    } else if (argc > 1 && std::string(argv[1]) == "solo") {
        isOnline = false;
        std::cout << "Entrez votre pseudo : "; std::cin >> pseudo1;
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

    if (window == nullptr) {
        std::cerr << "\n[ ERREUR CRITIQUE ] Impossible de lancer l'interface graphique !" << std::endl;
        std::cerr << "-> Verifiez que votre carte graphique supporte OpenGL 3.3." << std::endl;
        if (audioInitialized) ma_engine_uninit(&audioEngine);
        return -1;
    }

    // --- MUSIQUE DE FOND EN BOUCLE ---
    if (audioInitialized) {
        if (ma_sound_init_from_file(&audioEngine, "sounds/music.wav",
            MA_SOUND_FLAG_STREAM, NULL, NULL, &bgMusic) == MA_SUCCESS) {
            ma_sound_set_looping(&bgMusic, MA_TRUE);   // boucle infinie
            ma_sound_set_volume(&bgMusic, 0.45f);      // volume doux en fond
            ma_sound_start(&bgMusic);
            bgMusicLoaded = true;
        } else {
            std::cout << "[Audio] Musique de fond non trouvee (sounds/music.wav)" << std::endl;
        }
    }

    // --- BOUCLE PRINCIPALE ---
    while (!glfwWindowShouldClose(window)) {
        view.viewBoard();

        if (!view.isWon()) {
            view.processInput(window, controller);
            if (isOnline) controller.processNetwork();
            else          controller.processAI();
        } else {
            // Arreter la musique et laisser win.wav jouer une fois
            if (bgMusicLoaded) {
                ma_sound_stop(&bgMusic);
                bgMusicLoaded = false;
            }
            view.processInput(window, controller);
            view.winner(true, controller.getCurrentPlayer());
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    // --- NETTOYAGE ---
    if (bgMusicLoaded) ma_sound_uninit(&bgMusic);
    if (audioInitialized) ma_engine_uninit(&audioEngine);
    return 0;
}