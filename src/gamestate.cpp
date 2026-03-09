#include "gamestate.hpp"
#include "board.hpp"
#include "case.hpp"
#include "builder.hpp"

GameState::GameState() {}

GameState::GameState(const GameState& other) {
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
            floors[i][j] = other.floors[i][j];

    for (int b = 0; b < 4; b++) {
        builderPos[b][0] = other.builderPos[b][0];
        builderPos[b][1] = other.builderPos[b][1];
    }
}

bool GameState::isInside(int x, int y) {
    return x >= 0 && x < 5 && y >= 0 && y < 5;
}

bool GameState::isOccupied(int x, int y) {
    for (int i = 0; i < 4; i++)
        if (builderPos[i][0] == x && builderPos[i][1] == y)
            return true;
    return false;
}

void GameState::loadFromBoard(Board* b) {

    // Copie des niveaux
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            floors[x][y] = b->getCase(x, y)->getFloor();
        }
    }

    int iaIndex = 0;       // 0,1
    int playerIndex = 2;   // 2,3

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {

            Case* c = b->getCase(x, y);
            Builder* builder = c->getBuilder();

            if (builder != nullptr) {

                if (builder->getPlayer() == 1) { // IA
                    builderPos[iaIndex][0] = x;
                    builderPos[iaIndex][1] = y;
                    iaIndex++;
                }
                else { // Joueur
                    builderPos[playerIndex][0] = x;
                    builderPos[playerIndex][1] = y;
                    playerIndex++;
                }
            }
        }
    }
}