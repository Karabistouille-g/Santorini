#include "model.hpp"
#include "board.hpp"
#include "builder.hpp"

Model::Model() : view(View::getInstance()) {

}

bool Model::startGame(int playerOne, int playerTwo) {
    board_ = Board::getInstance();

    //  demande pas à l'utilisateur, on impose les positions de départ
    // Sinon avec "int x, y" non initialises le jeu plante direct
    
    // Joueur 1 (Positions fixes : Coins gauche)
    pawns[0][0] = new Builder(0, 0, playerOne); 
    pawns[0][1] = new Builder(0, 4, playerOne);

    // Joueur 2 (Positions fixes : Coins droite)
    pawns[1][0] = new Builder(4, 0, playerTwo);
    pawns[1][1] = new Builder(4, 4, playerTwo);

    std::cout << "[Model] Game started. Pawns placed correctly." << std::endl;
    return true;
}

bool Model::playTurn(int player) {
    if (player != currentPlayer_) return false;

    int builder;
    do {
        std::cout << "Select builder (1 or 2): ";
        std::cin >> builder;
    } while (builder != 1 && builder != 2);

    int x, y;
    std::cout << "Move X Y: ";
    std::cin >> x >> y; 
    pawns[player][builder]->moveBuilder(x, y);

    do {
        std::cout << "Select builder to build: ";
        std::cin >> builder;
    } while (builder != 1 && builder != 2);


    std::cout << "Build X Y: ";
    std::cin >> x >> y; 
    
    pawns[player][builder]->createBuild(x, y);
    endGame();
    
    return true; 
}

bool Model::endGame() {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            //vérifie que le pion existe
            if (pawns[i][j] == nullptr) continue;
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