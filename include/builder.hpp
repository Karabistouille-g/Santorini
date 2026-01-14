#ifndef BUILDER_H
#define BUILDER_H

class Case;
class Board;

#include <stdio.h>
#include <iostream>

class Builder {
    
    public:
        /**
         * Create a builder
         */
        Builder();

        /**
         * Get the position of the builder
         */
        Case* getPosition();

        /**
         * Move the builder on the board
         */
        bool moveBuilder();

        /**
         * Build a floor on the board
         */
        bool createBuild();

    private:

        Board* b_; 
        Case* position_;
        int player_; 

        /**
         * Verify if the coordonate is corect
         */
        bool validCase(Case* case_);

        /**
         * Ask the player where to place the builder
         */
        Case* placeOnTheBoard();
};

#endif // BUILDER_H