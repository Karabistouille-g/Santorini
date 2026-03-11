#include "builder.hpp"
#include "board.hpp"
#include "case.hpp"

#include <iostream>

Builder::Builder(int x, int y, int player, int id) : player_(player), id_(id) {
    b_ = Board::getInstance();
    position_ = b_->getCase(x, y);
    position_->setBuilder(this);
    moves_.push(position_);
}

Case* Builder::getPosition() {
    return position_;
} 

bool Builder::moveBuilder(int x, int y) {
    if (x < 0 || x >= 5 || y < 0 || y >= 5) return false;
    Case* target = b_->getCase(x, y);

    // SÉCURITÉ : On vérifie si la case est déjà occupée par un autre Builder
    if (target->getBuilder() != nullptr) {
        std::cout << "[Rules] Mouvement impossible : Case deja occupee !" << std::endl;
        return false;
    }
    if (!validCase(target)) return false;

    if (target->getFloor() >= 4) return false;

    int floor = position_->getFloor();
    int targetFloor = target->getFloor();
    
    if (targetFloor > floor + 1) return false;

    // On déplace le pion
    position_->setBuilder(nullptr);
    target->setBuilder(this);
    position_ = target;
    moves_.push(position_);

    // CONDITION DE VICTOIRE : Si on arrive sur un étage 3
    if (targetFloor == 3) {
        std::cout << "VICTOIRE DETECTEE pour joueur " << player_ << std::endl;
        
    }

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

int Builder::getPlayer() {
    return player_;
}

int Builder::getId() {
    return id_;
}

void Builder::undoMove() {
    if (moves_.size() <= 1) return; // guard
    moves_.pop();
    Case* previous = moves_.top();
    position_->setBuilder(nullptr);
    previous->setBuilder(this);
    position_ = previous;
}

void Builder::undoBuild() {
    if (builds_.empty()) return; // guard
    Case* build = builds_.top();
    builds_.pop();
    build->removeFloor();
}

void Builder::hardReset(Case* c) {
    // Vide les stacks et force la position — appele une seule fois apres minimax
    if (position_) position_->setBuilder(nullptr);
    while (!moves_.empty())  moves_.pop();
    while (!builds_.empty()) builds_.pop();
    position_ = c;
    position_->setBuilder(this);
    moves_.push(position_);
}