#ifndef CASE_H
#define CASE_H

#include <builder.hpp>

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

        /**
         * Remove build
         */
        bool removeFloor();
        
        /**
         * Get the builder on this case
         */
        Builder* getBuilder();

        /**
         * Set the builder on this case
         */
        void setBuilder(Builder* builder);

    private:

        int x_;
        int y_;
        int floor_;  
        Builder* builder_;
};

#endif // CASE_H