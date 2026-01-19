#include "include/builder.hpp"
#include "include/board.hpp"
#include "include/case.hpp"

Builder::Builder(int x, int y) {
    b_ = Board::getInstance();
    position_ = b_->getCase(x, y);
}

Case* Builder::getPosition() {
    return position_;
} 

bool Builder::moveBuilder(int x, int y) {

    Case* target = b_->getCase(x, y);
    if (!validCase(target)) return false;

    int floor = position_->getFloor();
    int targetFloor = target->getFloor();
    
    if (floor + 1 < targetFloor) return false;

    position_ = target;
    return true;
}

bool Builder::createBuild(int x, int y) {

    Case* target = b_->getCase(x, y);
    if (!validCase(target)) return false;

    int targetFloor = target->getFloor();
    if (targetFloor >= 4) return false;

    target->addFloor();
    return true;
}

bool Builder::validCase(Case* target) {
    // Coordonnées actuelles
    int curX = position_->getX();
    int curY = position_->getY();

    // Coordonnées cibles
    int tarX = target->getX();
    int tarY = target->getY();

    // 1. Vérifier les limites du tableau (0 à 4)
    if (tarX < 0 || tarX > 4 || tarY < 0 || tarY > 4) return false;

    // 2. Calculer la distance (delta)
    int diffX = tarX - curX;
    int diffY = tarY - curY;

    // 3. On ne peut pas rester sur place
    if (diffX == 0 && diffY == 0) return false;

    // 4. On ne peut bouger que de 1 case max (diagonale comprise)
    // Donc la différence doit être entre -1 et 1
    
    if (diffX < -1 || diffX > 1) return false;
    if (diffY < -1 || diffY > 1) return false;


    return true;
}
