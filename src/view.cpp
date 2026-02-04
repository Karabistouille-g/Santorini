#include "view.hpp"
#include "board.hpp"
#include "case.hpp"
#include <iostream>

View::View()
{
    // // Initialize GLFW
    // if( !glfwInit() )
    // {
    //     std::cerr << "Failed to initialize GLFW" << std::endl;
    //     return;
    // }
    // // Set OpenGL version to 4.3
    // glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    // glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    // glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    // Create window and OpenGL context
    // window_ = glfwCreateWindow( WINDOW_BASE_WIDTH, WINDOW_BASE_HEIGHT, "Santorini", nullptr, nullptr );
    // if( !window_ )
    // {
    //     std::cerr << "Failed to create window" << std::endl;
    //     glfwTerminate();
    //     return;
    // }
    // glfwMakeContextCurrent( window_ );

    // // Initialize GLAD
    // if( !gladLoadGLLoader( ( GLADloadproc ) glfwGetProcAddress ) )
    // {
    //     std::cout << "Failed to initialize GLAD" << std::endl;
    //     return;
    // }

    // glViewport( 0, 0, WINDOW_BASE_WIDTH, WINDOW_BASE_HEIGHT );
    // // Update window upon resizing
    // glfwSetFramebufferSizeCallback( window_,
    //     []( GLFWwindow * window, int width, int height )
    //     {
    //         glViewport( 0, 0, width, height );
    //     }
    // );

    // glfwSetKeyCallback( window_,
    //     []( GLFWwindow * window, int key, int scancode, int action, int mods )
    //     {
    //         if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
    //             glfwSetWindowShouldClose( window, true );
    //     }
    // );

    // glEnable( GL_DEPTH_TEST );
    // glEnable( GL_CULL_FACE );
    // // glEnable( GL_BLEND );
    // // glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    // glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    // glCheckError();
}

View::~View()
{
    // glfwDestroyWindow( window_ );
    // glfwTerminate();
    // glCheckError();
}

View & View::getInstance()
{
    static View instance;
    return instance;
}

void View::viewBoard( bool is3D )
{
    // glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    Board * b = Board::getInstance();
    if( !is3D )
    {
        for( int x = 0; x < 5; x++ )
        {
            for( int y = 0; y < 5; y++ )
            {
                if( b->getCase( x, y )->getBuilder() )
                {
                    std::cout << "|X|";
                }
                else
                {
                    std::cout << "|" << b->getCase( x, y )->getFloor() << "|";
                }
            }
            std::cout << "\n";
        }
        std::cout << std::endl; // newline and flush
        return;
    }

    // glfwPollEvents();
    // glfwSwapBuffers( window_ );
}

void View::winner( bool is3D, int p )
{
    viewBoard( is3D );
    if( !is3D )
    {
        Board * b = Board::getInstance();
        std::cout << "Player " << p << " wins !" << std::endl;
        return;
    }
}

GLenum View::glCheckError( const char * file, int line ) const noexcept
{
    GLenum errorCode = glGetError();
    while( ( errorCode != GL_NO_ERROR ) )
    {
        const char * error;
        switch( errorCode )
        {
            case GL_INVALID_ENUM: error = "INVALID ENUM"; break;
            case GL_INVALID_VALUE: error = "INVALID VALUE"; break;
            case GL_INVALID_OPERATION: error = "INVALID OPERATION"; break;
            case GL_OUT_OF_MEMORY: error = "OUT OF MEMEORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID FRAMEBUFFER OPERATION"; break;
            case GL_INVALID_INDEX: error = "INVALID INDEX"; break;
        }
        std::cerr << error << " in " << file << " at line " << line << std::endl;
        errorCode = glGetError();
    }
    return errorCode;
}

void View::debugMat( glm::mat4 m ) const noexcept
{
    for( int i = 0; i < 4; i++ )
    {
        for( int j = 0; j < 4; j++ )
            std::cout << m[j][i] << "  ";
        std::cout << "\n";
    }
    std::cout << std::endl;
}

void View::debugMat( glm::mat3 m ) const noexcept
{
    for( int i = 0; i < 3; i++ )
    {
        for( int j = 0; j < 3; j++ )
            std::cout << m[j][i] << "  ";
        std::cout << "\n";
    }
    std::cout << std::endl;
}

void View::debugMat( glm::mat2 m ) const noexcept
{
    for( int i = 0; i < 2; i++ )
    {
        for( int j = 0; j < 2; j++ )
            std::cout << m[j][i] << "  ";
        std::cout << "\n";
    }
    std::cout << std::endl;
}