#ifndef VIEW_HPP
#define VIEW_HPP

#include <glad/glad.h>
#include <GL/glext.h>
#include <glm/glm.hpp>

#define WINDOW_BASE_WIDTH 800
#define WINDOW_BASE_HEIGHT 600

class View
{
    public :
        /**
         * Constructor
         * Initializes the window, OpenGL and necessary libraries
         */
        View();
        ~View() = default;

        /**
         * Render the game board
         * @param is3D Whether to render in 3D or 2D
         */
        void viewBoard( bool is3D );
        /**
         * Render the winning screen
         * @param is3D Whether to render in 3D or 2D
         * @param p The winning player
         */
        void winner( bool is3D, int p );
    
    private :
        // Debugging functions

        /**
         * Check for OpenGL errors and print them to stderr
         * Source : https://learnopengl.com/In-Practice/Debugging
         * @param file The file where the error check is called (default: current file)
         * @param line The line where the error check is called (default: current line)
         * @return The last error code
         */
        GLenum glCheckError( const char * file = __FILE__, int line = __LINE__ );
        /**
         * Print a 4x4 matrix to stdout for debugging
         * @param m The matrix to print
         */
        void debugMat( glm::mat4 m );
        /**
         * Print a 3x3 matrix to stdout for debugging
         * @param m The matrix to print
         */
        void debugMat( glm::mat3 m );
        /**
         * Print a 2x2 matrix to stdout for debugging
         * @param m The matrix to print
         */
        void debugMat( glm::mat2 m );

};

#endif