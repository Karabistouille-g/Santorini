#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>

// Forward declaration propre
class Board;

struct Move {
    int builderIndex;   // 0-1 = IA, 2-3 = joueur
    int moveX, moveY;
    int buildX, buildY;
};

class GameState {

public:

    int floors[5][5];
    int builderPos[4][2];   // [builder][x/y]

    GameState();
    GameState(const GameState& other);

    std::vector<Move> getAllMoves(bool iaTurn);
    void applyMove(const Move& m);

    int evaluate();
    bool isWinningState();

    void loadFromBoard(Board* b);

private:

    bool isInside(int x, int y);
    bool isOccupied(int x, int y);
};

#endif