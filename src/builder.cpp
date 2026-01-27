#include "builder.hpp"
#include "board.hpp"
#include "case.hpp"

#include <iostream>

Builder::Builder(int x, int y) {
    b_ = Board::getInstance();
    position_ = b_->getCase(x, y);
}

Case* Builder::getPosition() {
    return position_;
} 

bool Builder::moveBuilder(int x, int y) {

    std::cout << getPosition()->getX() << "," << getPosition()->getY() << " -> " << x << "," << y << std::endl;

    Case* target = b_->getCase(x, y);
    if (!validCase(target)) return false;

    std::cout << "Target floor: " << target->getFloor() << std::endl;

    if (target->getFloor() >= 4) return false;

    std::cout << "Current floor: " << position_->getFloor() << std::endl;

    int floor = position_->getFloor();
    int targetFloor = target->getFloor();
    
    if (targetFloor > floor + 1) return false;

    std::cout << "Move successful" << std::endl;

    position_->setBuilder(nullptr);
    target->setBuilder(this);

    position_ = target;
    return true;
}

bool Builder::createBuild(int x, int y) {

    std::cout << getPosition()->getX() << "," << getPosition()->getY() << " -> " << x << "," << y << std::endl;

    Case* target = b_->getCase(x, y);
    if (!validCase(target)) return false;

    std::cout << "Target floor before build: " << target->getFloor() << std::endl;

    if (target->getFloor() >= 4) return false;

    std::cout << "Build successful" << std::endl;

    target->addFloor();
    return true;
}

bool Builder::validCase(Case* target) {
    
    int curX = position_->getX();
    int curY = position_->getY();

    int tarX = target->getX();
    int tarY = target->getY();

    if (tarX < 0 || tarX > 4 || tarY < 0 || tarY > 4) return false;

    int diffX = tarX - curX;
    int diffY = tarY - curY;

    if (diffX == 0 && diffY == 0) return false;
    
    if (diffX < -1 || diffX > 1) return false;
    if (diffY < -1 || diffY > 1) return false;

    return true;
}
