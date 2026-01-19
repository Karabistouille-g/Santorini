#ifndef MODEL_H
#define MODEL_H

#include "board.hpp"
#include "builder.hpp"
#include "case.hpp"
#include <stdio.h>

class Model {

    public:

        /**
         * Create the model
         */
        Model();

        /**
         * Create the board with case and add th builders
         */
        bool startGame(int playerOne, int playerTwo);

        /**
         * Start the turn of the next player
         */
        bool playTurn(int player);

        /**
         * Check the win condition
         */
        bool endGame();

        // --- Partie de louis pour le controller---
        // Permet de recup pion spécifique pour le bouger
        Builder* getPawn(int player, int index) {
            if (player >= 0 && player < 2 && index >= 0 && index < 2) {
                return pawns[player][index];
            }
            return nullptr;
        }
       
        // ------------------------------------------------

    private:

        Board* board_;
        int currentPlayer_;
        Builder* pawns[2][2];
};

#endif // MODEL_H