#include "include/case.hpp"

Case::Case(int x, int y) {
    x_ = x;
    y_ = y;
    floor_ = 0;
}

int Case::getFloor() {
    return floor_;
}

int Case::getX() {
    return x_;
}

int Case::getY() {
    return y_;
}

bool Case::addFloor() {
    if (floor_ < 4) {
        floor_ += 1;
        return true;
    }
    return false;
}

