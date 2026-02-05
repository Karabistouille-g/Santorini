#ifndef BOB_H
#define BOB_H

#include <board.hpp>
#include <case.hpp>
#include <builder.hpp>
#include <vector>

struct MoveInfo {
    int moveX, moveY;
    int buildX, buildY;
    int score;
};

class Bob {

    public:
        
        Bob();

        void playTurn();


    private:

        Board* b;
        
        // Builder de l'IA
        Builder* iaBuilderFirst;
        Builder* iaBuilderSecond;

        // Builder du joueur
        Builder* playerBuilderFirst;
        Builder* playerBuilderSecond;

        int score();

        int minimax(int h, bool isMaximizing);
};

#endif