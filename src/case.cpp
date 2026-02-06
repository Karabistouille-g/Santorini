#include "case.hpp"
#include "builder.hpp"

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

bool Case::removeFloor() {
    if (floor_ > 0) {
        floor_ -= 1;
        return true;
    }
    return false;
}

Builder* Case::getBuilder() {
    return builder_;
}

void Case::setBuilder(Builder* builder) {
    builder_ = builder;
}

