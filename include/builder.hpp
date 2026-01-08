#ifndef BUILDER_H
#define BUILDER_H

#include "case.hpp"
#include "board.hpp"

#include <stdio.h>

class Builder {
    
    public:

        /**
         * Create a builder
         * @param x coord X selected by the player
         * @param y coord Y selected by the player
         */
        Builder(int x, int y);

        /**
         * Move the builder on the board
         */
        bool moveBuilder(int x, int y);

        /**
         * Build a floor on the board
         */
        bool createBuild(int x, int y);

    private:

        Case currentCase;

        /**
         * Verify if the coordonate is corect
         */
        bool verifCoordonate(int x, int y);
};

#endif // BUILDER_H