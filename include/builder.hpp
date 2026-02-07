#ifndef BUILDER_H
#define BUILDER_H

class Case;
class Board;

#include <stdio.h>
#include <iostream>
#include <stack>

class Builder {
    
    public:
        /**
         * Create a builder
         */
        Builder(int x, int y, int player);

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

        int getPlayer();

        void undoMove();

        void undoBuild();

    private:

        Board* b_; 
        Case* position_;
        int player_; 
        std::stack<Case*> moves_;
        std::stack<Case*> builds_;

        /**
         * Verify if the coordonate is corect
         */
        bool validCase(Case* case_);
};

#endif // BUILDER_H