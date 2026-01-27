#include "board.hpp"
#include "case.hpp"

Board* Board::instance_ = nullptr;
Case* Board::cases_[5][5] = {nullptr};

Board::Board() {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            cases_[i][j] = new Case(i, j);
        }
    }
}

Board* Board::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new Board();
    }
    return instance_;
}

bool Board::resetInstance() {
    if (instance_ != nullptr) {
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                delete cases_[i][j];
                cases_[i][j] = nullptr;
            }
        }
        delete instance_;
        instance_ = nullptr;
        return true;
    }
    return false;
}

Case* Board::getCase(int x, int y) {
    return cases_[x][y];
}