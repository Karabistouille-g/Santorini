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
    pawns[0][0] = new Builder(0, 0, playerOne, 0); 
    pawns[0][1] = new Builder(0, 4, playerOne, 1);

    // Joueur 2 (Positions fixes : Coins droite)
    pawns[1][0] = new Builder(4, 0, playerTwo, 0);
    pawns[1][1] = new Builder(4, 4, playerTwo, 1);

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
    
    
    return true; 
}

void Model::printTerminalBoard() {
    std::cout << "\n--- PLATEAU ACTUEL (Niveaux) ---\n";
    for (int y = 4; y >= 0; y--) { 
        for (int x = 0; x < 5; x++) {
            // On affiche le niveau
            std::cout << " " << board_->getCase(x, y)->getFloor() << " ";
        }
        std::cout << "\n";
    }
    std::cout << "--------------------------------\n";
}

bool Model::isGameOver() const {
    return gameOver_;
}

int Model::getWinner() const {
    return winner_;
}