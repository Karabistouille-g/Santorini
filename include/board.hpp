#ifndef BOARD_H
#define BOARD_H

#include "case.hpp"

class Board {
    
    public:

        /**
         * Get the instance of the board or create it if don't exist
         */
        static Board& getInstance();

        /**
         * Get the tab of case
         */
        static Case** getCases();

    private:

        static Board board_;
        Case** tab[5][5];

        /**
         * Create the board
         */
        Board();

};

#endif // BOARD_H