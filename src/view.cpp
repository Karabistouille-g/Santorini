#include "view.hpp"
#include "case.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

View::View() : cursorX_(2), cursorY_(2), lockX_(-1), lockY_(-1), lock_(false), build_(false), win_(false), lockBuilder_(nullptr), keyUpPressed_(false), keyDownPressed_(false), keyRightPressed_(false), keyLeftPressed_(false), b_(Board::getInstance())
{
    // // Initialize GLFW
    if( !glfwInit() )
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }
    // Set OpenGL version to 4.3
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    // Create window and OpenGL context
    window_ = glfwCreateWindow( WINDOW_BASE_WIDTH, WINDOW_BASE_HEIGHT, "Santorini", nullptr, nullptr );
    if( !window_ )
    {
        std::cerr << "[ ERROR ] Failed to create window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent( window_ );

    // Initialize GLAD
    if( !gladLoadGLLoader( ( GLADloadproc ) glfwGetProcAddress ) )
    {
        std::cout << "[ ERROR ] Failed to initialize GLAD" << std::endl;
        return;
    }

    glViewport( 0, 0, WINDOW_BASE_WIDTH, WINDOW_BASE_HEIGHT );
    // Update window upon resizing
    glfwSetFramebufferSizeCallback( window_,
        []( GLFWwindow * window, int width, int height )
        {
            glViewport( 0, 0, width, height );
        }
    );

    glfwSetKeyCallback( window_,
        []( GLFWwindow * window, int key, int scancode, int action, int mods )
        {
            if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
                glfwSetWindowShouldClose( window, true );
        }
    );

    glEnable( GL_DEPTH_TEST );
    // glEnable( GL_CULL_FACE );
    glEnable( GL_BLEND );
    // // glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    // glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    // glCheckError();

    s_ = std::make_unique<Shader>("../include/shaders/vertex.vert", "../include/shaders/fragment.frag", nullptr);
    m_ = std::make_unique<Mesh>();
}

View::~View()
{
    s_.reset(); 
    m_.reset();

    glfwDestroyWindow( window_ );
    glfwTerminate();
    std::cout << "[ INFO ] Close the window" << std::endl;
}

View & View::getInstance()
{
    static View instance;
    return instance;
}

void View::viewBoard()
{
    glClearColor(0.15f, 0.15f, 0.34f, 1.0f);

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    s_->activate();

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_BASE_WIDTH / (float)WINDOW_BASE_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

    s_->setMat4("projection", projection);
    s_->setMat4("view", view);

    float spacing = 0.25f;

    if (win_) {
        winner(false, 0);
    }
    
    for( int x = 0; x < 5; x++ )
    {
        for( int y = 0; y < 5; y++ )
        {
            int floor = b_->getCase(x, y)->getFloor();
            glm::mat4 model = glm::mat4(1.0f);

            float posX = (x - 2.0f) * spacing;
            float posY = (y - 2.0f) * spacing;
            
            model = glm::translate(model, glm::vec3(posX, posY, 0.01f));
            s_->setVec3("caseColor", 0.2f * floor + 0.2f, 0.2f * floor + 0.2f, 0.2f * floor + 0.2f);
            s_->setMat4("model", model);

            m_->draw(*s_);

            if (b_->getCase(x, y)->getBuilder()) {
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.015f));
                model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f));

                if (b_->getCase(x, y)->getBuilder()->getPlayer() == 0) {
                    s_->setVec3("caseColor", 1.0f, 0.0f, 0.0f);
                }
                if (b_->getCase(x, y)->getBuilder()->getPlayer() == 1) {
                    s_->setVec3("caseColor", 0.0f, 0.0f, 1.0f);
                }
                s_->setMat4("model", model);

                m_->draw(*s_);
            }

            // Case du curseur
            if (x == cursorX_ && y == cursorY_) {
                glm::mat4 cursorModel = glm::mat4(1.0f);
                cursorModel = glm::translate(cursorModel, glm::vec3(posX, posY, 0.0015f));
                cursorModel = glm::scale(cursorModel, glm::vec3(1.1f, 1.1f, 1.0f)); 
                s_->setVec3("caseColor", 1.0f, 0.50f, 0.39f);
                s_->setMat4("model", cursorModel);
                m_->draw(*s_);
            }

            // Case de séléction
            if (x == lockX_ && y == lockY_) {
                glm::mat4 cursorModel = glm::mat4(1.0f);
                cursorModel = glm::translate(cursorModel, glm::vec3(posX, posY, 0.001f));
                cursorModel = glm::scale(cursorModel, glm::vec3(1.1f, 1.1f, 1.0f)); 
                s_->setVec3("caseColor", 1.0f, 0.0f, 0.0f);
                s_->setMat4("model", cursorModel);
                m_->draw(*s_);

                // Case possible
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        if (i == 0 && j == 0) continue;

                        int targetX = x + i;
                        int targetY = y + j;

                        // SÉCURITÉ : On vérifie qu'on ne sort pas du plateau (0 à 4)
                        if (targetX >= 0 && targetX < 5 && targetY >= 0 && targetY < 5) {
                            
                            // Calcul de la position réelle sur l'écran
                            float pX = (targetX - 2.0f) * spacing;
                            float pY = (targetY - 2.0f) * spacing;
                            
                            glm::mat4 moveModel = glm::mat4(1.0f);
                            moveModel = glm::translate(moveModel, glm::vec3(pX, pY, 0.001f));
                            moveModel = glm::scale(moveModel, glm::vec3(1.1f, 1.1f, 1.0f)); 
                            
                            s_->setVec3("caseColor", 0.0f, 1.0f, 0.0f); // Vert pour "possible"
                            s_->setMat4("model", moveModel);
                            m_->draw(*s_);
                        }
                    }
                }
            }
        }
    }
}

void View::winner( bool is3D, int p )
{
    viewBoard();
    if( !is3D )
    {
        Board * b = Board::getInstance();
        std::cout << "Player " << p << " wins !" << std::endl;
        return;
    }
}

GLFWwindow* View::getWindow() {
    return window_;
}

void View::processInput(GLFWwindow *window, santorini::Controller &c)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        if (!keyUpPressed_) {
            if (cursorY_ < 4) {
                cursorY_++;
            }
            keyUpPressed_ = true;
        }
    } else {
        keyUpPressed_ = false;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (!keyDownPressed_) {
            if (cursorY_ > 0) {
                cursorY_--;
            } 
            keyDownPressed_ = true;
        }
    } else {
        keyDownPressed_ = false;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        if (!keyRightPressed_) {
            if (cursorX_ < 4) {
                cursorX_++;
            } 
            keyRightPressed_ = true;
        }
    } else {
        keyRightPressed_ = false;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        if (!keyLeftPressed_) {
            if (cursorX_ > 0) {
                cursorX_--;
            } 
        keyLeftPressed_ = true;
        }
    } else {
        keyLeftPressed_ = false;
    }

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if (!keyEnterPressed_) {
            if (!lock_ && !build_) {
                auto targetBuilder = b_->getCase(cursorX_, cursorY_)->getBuilder();
                // On vérifie qu'il y a un builder et que c'est le nôtre
                if (targetBuilder && targetBuilder->getPlayer() == c.getCurrentPlayer()) {
                    lockX_ = cursorX_;
                    lockY_ = cursorY_;
                    lockBuilder_ = targetBuilder;
                    lock_ = true;
                    std::cout << "PERSONNAGE SELECTIONNE" << std::endl;
                }
            } 
            else if (lock_ && !build_) {
                if (c.selectMove(lockBuilder_->getId(), cursorX_, cursorY_) == 1) {
                    lockX_ = cursorX_;
                    lockY_ = cursorY_;
                    build_ = true;
                    std::cout << "PERSONNAGE DEPLACE" << std::endl;
                } else if (c.selectMove(lockBuilder_->getId(), cursorX_, cursorY_) == 2) {
                    win_ = true;
                }
            }
            else if (lock_ && build_) {
                if (c.selectBuild(lockBuilder_->getId(), cursorX_, cursorY_)) {
                    lockX_ = -1;
                    lockY_ = -1;
                    build_ = false;
                    lock_ = false;
                    lockBuilder_ = nullptr;
                    std::cout << "BUILDING PLACE" << std::endl;
                }

            }
            keyEnterPressed_ = true;
        }
    } else {
        keyEnterPressed_ = false;
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