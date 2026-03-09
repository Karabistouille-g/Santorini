#ifndef MODEL_H
#define MODEL_H

#include "board.hpp"
#include "builder.hpp"
#include "case.hpp"
#include <stdio.h>
#include <view.hpp>

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

      

        // --- Partie de louis pour le controller---
        // Permet de recup pion spécifique pour le bouger
        Builder* getPawn(int player, int index) {
            if (player >= 0 && player < 2 && index >= 0 && index < 2) {
                return pawns[player][index];
            }
            return nullptr;
        }
        void printTerminalBoard();

        int getCurrentPlayer() const { return currentPlayer_; }

        void nextTurn() { 
         currentPlayer_ = (currentPlayer_ == 0) ? 1 : 0; 
        }
        // ------------------------------------------------

    private:

        Board* board_;
        int currentPlayer_ = 0;
        Builder* pawns[2][2];
        View& view;
        
        bool gameOver_ = false;
        int winner_ = -1;
    

public:
    bool isGameOver() const;
    int getWinner() const;
};

#endif // MODEL_H