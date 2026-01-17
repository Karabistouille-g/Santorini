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
}

bool Builder::createBuild(int x, int y) {

    Case* target = b_->getCase(x, y);
    if (validCase(target)) return false;

    int targetFloor = target->getFloor();
    if (targetFloor <= 4) return false;

    target->addFloor();
    return true;
}

bool Builder::validCase(Case* target) {

    int curX = position_->getX();
    int curY = position_->getY();

    if ((curX + 1 > 5) || (curX - 1 < 0)) return false;
    if ((curY + 1 > 5) || (curY - 1 < 0)) return false;

    int tarX = target->getX();
    int tarY = target->getY();

    if ((curX == tarX) || (curX + 1 != tarX) || (curX - 1 != tarX)) return false;
    if ((curY == tarY) || (curY + 1 != tarY) || (curY - 1 != tarY)) return false;

    return true;
}
