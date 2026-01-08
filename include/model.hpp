#ifndef MODEL_H
#define MODEL_H

class Model {

    public:

        /**
         * Create the model
         */
        Model();

        /**
         * Create the board with case and add th builders
         */
        bool startGame();

        /**
         * Start the turn of the next player
         */
        bool playTurn();

        /**
         * Check the win condition
         */
        bool endGame();

    private:
};

#endif // MODEL_H