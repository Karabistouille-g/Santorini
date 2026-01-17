#include "include/model.hpp"
#include "include/board.hpp"
#include "include/builder.hpp"

Model::Model() {

}

bool Model::startGame(int playerOne, int playerTwo) {
    board_ = Board::getInstance();

    int x, y; // Demander aux joueurs ou placer leurs pions

    // First builder
    pawns[0][0] = new Builder(x, y);
    pawns[1][0] = new Builder(x, y);

    // Second builder
    pawns[0][1] = new Builder(x, y);
    pawns[1][1] = new Builder(x, y);

    return true;
}

bool Model::playTurn(int player) {

    if (player != currentPlayer_) return false;

    int builder;

    do {
        std::cout << "Select the builder to move (1 or 2) : ";
        std::cin >> builder;
    } while (builder != 1 && builder != 2);

    int x, y; // Demander ou le joueur veut déplacer son pion
    pawns[player][builder]->moveBuilder(x, y);

    do {
        std::cout << "Select the builder to build (1 or 2) : ";
        std::cin >> builder;
    } while (builder != 1 && builder != 2);

    int x, y; // Demander ou le joueur veut construire
    pawns[player][builder]->createBuild(x, y);

    endGame();

    player = (player + 1) % 2; // FIXME a adapté
}

bool Model::endGame() {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            Case* pos = pawns[i][j]->getPosition();
            if (pos == nullptr) continue; // FIXME message d'erreur
            if (pos->getFloor() == 3) {
                std::cout << "Player win" << std::endl;
                // TODO faire appelle a la fonction d'écran de victoire
                return true;
            }
        }
    }
    return false;
}