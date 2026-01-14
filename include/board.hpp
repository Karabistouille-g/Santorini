#ifndef BOARD_H
#define BOARD_H

class Case;

class Board {
    public:

        /**
         * Get the instance of the board or create it if don't exist
         */
        static Board* getInstance();

        /**
         * Get the tab of case
         */
        static Case* getCase(int x, int y);

    private:

        static Board* instance_;

        static Case* cases_[5][5];

        /**
         * Create the board
         */
        Board();
};

#endif // BOARD_H