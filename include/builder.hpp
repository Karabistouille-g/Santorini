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
        Builder(int x, int y);

        /**
         * Get the position of the builder
         */
        Case* getPosition();

        /**
         * Move the builder on the board
         */
        bool moveBuilder(int x, int y);

        /**
         * Build a floor on the board
         */
        bool createBuild(int x, int y);

    private:

        Board* b_; 
        Case* position_;
        int player_; 

        /**
         * Verify if the coordonate is corect
         */
        bool validCase(Case* case_);
};

#endif // BUILDER_H