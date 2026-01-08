#include "include/builder.hpp"

Case** tab;

Builder::Builder(int x, int y) {

    Board& b = Board::getInstance();
    Case** tab = b.getCases();

    currentCase = tab[x][y];
}

bool Builder::moveBuilder(int x, int y) {

    verifCoordonate(x, y);

    int curFloor = currentCase.getFloor();

    Case target = tab[x][y];
    int targetFloor = target.getFloor();
    
    if (curFloor + 1 < targetFloor) return false;

    currentCase = tab[x][y];
}

bool Builder::createBuild(int x, int y) {

    verifCoordonate(x, y);

    Case target = tab[x][y];
    int targetFloor = target.getFloor();

    if (targetFloor <= 4) return false;

    target.addFloor();
    return true;
}

bool Builder::verifCoordonate(int x, int y) {

    int curX = currentCase.getX();
    int curY = currentCase.getY();

    if ((curX != x) || (curX + 1 != x) | (curX - 1 != x)) return false;
    if ((curY != y) || (curY + 1 != y) | (curY - 1 != y)) return false;

    if ((curX + 1 > 5) || (curX - 1 < 0)) return false;
    if ((curY + 1 > 5) || (curY - 1 < 0)) return false;

    return true;
}

