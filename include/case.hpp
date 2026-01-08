#ifndef CASE_H
#define CASE_H

class Case {  

    public:

        Case();
        
        /**
         * Create the case (floor = 0)
         * @param x coord X
         * @param y coord Y
         */
        Case(int x, int y);

        /**
         * Get build
         */
        int getFloor();

        /**
         * Get X
         */
        int getX();

        /**
         * Get Y
         */
        int getY();

        /**
         * Set build
         */
        bool addFloor();

    private:

        int x_;
        int y_;
        int floor_;  
};

#endif // CASE_H