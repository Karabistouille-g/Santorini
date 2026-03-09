#ifndef BOB_H
#define BOB_H

#include <board.hpp>
#include <case.hpp>
#include <builder.hpp>
#include <vector>
#include <algorithm>

struct MoveInfo {
    int moveX, moveY;
    int buildX, buildY;
    int score;
};

class Bob {

public:
    Bob(int difficulty = 2);   // 1 = facile, 2 = normal, 3 = difficile
    void playTurn();

private:

    Board* b;

    Builder* iaBuilderFirst;
    Builder* iaBuilderSecond;

    Builder* playerBuilderFirst;
    Builder* playerBuilderSecond;

    int difficulty_;

    int score();
    int minimax(int depth, int alpha, int beta, bool maximizing);
    int countMoves(Builder* b);
};

#endif