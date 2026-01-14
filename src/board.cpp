#include "include/board.hpp"
#include "include/case.hpp"

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

Case* Board::getCase(int x, int y) {
    return cases_[x][y];
}