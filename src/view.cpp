#include "view.hpp"
#include "case.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

// --- LIEN AVEC LE MOTEUR AUDIO DANS MAIN.CPP ---
extern void playSoundFX(const char* filepath);

View::View() : cursorX_(2), cursorY_(2), lockX_(-1), lockY_(-1), lock_(false), build_(false), win_(false), winnerId_(-1), lockBuilder_(nullptr), keyUpPressed_(false), keyDownPressed_(false), keyRightPressed_(false), keyLeftPressed_(false), b_(Board::getInstance())
{
    if( !glfwInit() ) { std::cerr << "Failed to initialize GLFW" << std::endl; return; }
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    window_ = glfwCreateWindow( WINDOW_BASE_WIDTH, WINDOW_BASE_HEIGHT, "Santorini", nullptr, nullptr );
    if( !window_ ) { std::cerr << "[ ERROR ] Failed to create window" << std::endl; glfwTerminate(); return; }
    glfwMakeContextCurrent( window_ );
    glfwSwapInterval(1);

    if( !gladLoadGLLoader( ( GLADloadproc ) glfwGetProcAddress ) ) { std::cout << "[ ERROR ] Failed to initialize GLAD" << std::endl; return; }

    glViewport( 0, 0, WINDOW_BASE_WIDTH, WINDOW_BASE_HEIGHT );
    glfwSetFramebufferSizeCallback( window_, []( GLFWwindow * window, int width, int height ) { glViewport( 0, 0, width, height ); } );
    glfwSetKeyCallback( window_, []( GLFWwindow * window, int key, int scancode, int action, int mods ) {});

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    s_ = std::make_unique<Shader>("../include/shaders/vertex.vert", "../include/shaders/fragment.frag", nullptr);
    m_ = std::make_unique<Mesh>();
}

View::~View() { s_.reset(); m_.reset(); glfwDestroyWindow( window_ ); glfwTerminate(); }

View & View::getInstance() { static View instance; return instance; }

// --- LANCEMENT DE LA MUSIQUE DE VICTOIRE ---
void View::setWinner(int id) { 
    if (!win_) { 
        playSoundFX("sounds/win.mp3");
    }
    win_ = true; 
    winnerId_ = id;
}

// --- LANCEMENT DES BRUITAGES (Animations) en .mp3 ---
void View::triggerMoveAnimation(int sx, int sy, int ex, int ey, int sFloor, int eFloor, Builder* b) {
    moveAnimActive_ = true;
    moveAnimStartX_ = sx; moveAnimStartY_ = sy;
    moveAnimEndX_ = ex; moveAnimEndY_ = ey;
    moveAnimStartFloor_ = sFloor; moveAnimEndFloor_ = eFloor;
    movingBuilder_ = b;
    moveAnimStartTime_ = glfwGetTime();
    
    // SON DE REBOND/SAUT
    playSoundFX("sounds/move.mp3");
}

void View::triggerBuildAnimation(int x, int y, int floor) {
    buildAnimActive_ = true;
    buildAnimX_ = x; buildAnimY_ = y;
    buildAnimFloor_ = floor;
    buildAnimStartTime_ = glfwGetTime();
    
    // NOUVEAU : On reinitialise le son de l'upgrade
    buildUpgradeSoundPlayed_ = false;
    
    // SONS D'IMPACT INITIAUX (Phase 1)
    if (floor == 1) playSoundFX("sounds/build1.mp3");
    else if (floor == 2) playSoundFX("sounds/build2.mp3");
    else if (floor == 3) playSoundFX("sounds/build3.mp3");
    else if (floor == 4) playSoundFX("sounds/dome.mp3"); // Le dome a direct son effet magique
}

void View::viewBoard() {
    
    if (firstFrame_) {
        for (int px = 0; px < 5; px++) {
            for (int py = 0; py < 5; py++) {
                prevFloors_[px][py] = b_->getCase(px, py)->getFloor();
                prevBuilderPos_[px][py] = b_->getCase(px, py)->getBuilder();
            }
        }
        firstFrame_ = false;
    }

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            int currentFloor = b_->getCase(x, y)->getFloor();
            Builder* currentBuilder = b_->getCase(x, y)->getBuilder();
            
            if (currentFloor > prevFloors_[x][y]) triggerBuildAnimation(x, y, currentFloor);
            
            if (currentBuilder != nullptr && prevBuilderPos_[x][y] != currentBuilder) {
                int oldX = x, oldY = y;
                for (int px = 0; px < 5; px++) {
                    for (int py = 0; py < 5; py++) {
                        if (prevBuilderPos_[px][py] == currentBuilder) { oldX = px; oldY = py; }
                    }
                }
                triggerMoveAnimation(oldX, oldY, x, y, prevFloors_[oldX][oldY], currentFloor, currentBuilder);
            }
        }
    }

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            prevFloors_[x][y] = b_->getCase(x, y)->getFloor();
            prevBuilderPos_[x][y] = b_->getCase(x, y)->getBuilder();
        }
    }

    float time = glfwGetTime();
    if (win_) {
        float pulse = (sin(time * 5.0f) + 1.0f) / 2.0f; 
        glClearColor(0.8f + 0.1f * pulse, 0.6f + 0.1f * pulse, 0.1f, 1.0f);
    } else {
        float cycle = sin(time * 0.15f); 
        glClearColor(0.1f + 0.1f * cycle, 0.15f + 0.05f * cycle, 0.3f + 0.1f * cycle, 1.0f); 
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    s_->activate();

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_BASE_WIDTH / (float)WINDOW_BASE_HEIGHT, 0.1f, 100.0f);
    float radAngle = glm::radians(camAngle_); float radElev = glm::radians(camElevation_);
    float camX = 2.0f + camRadius_ * cos(radElev) * sin(radAngle);
    float camY = camRadius_ * sin(radElev);
    float camZ = 2.0f + camRadius_ * cos(radElev) * cos(radAngle);
    glm::mat4 view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(2.0f, 0.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    s_->setMat4("projection", projection);
    s_->setMat4("view", view);

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            int floors = b_->getCase(x, y)->getFloor();

            bool isReachable = false;
            if (lock_ && lockBuilder_ != nullptr) {
                int dx = abs(x - lockBuilder_->getPosition()->getX());
                int dy = abs(y - lockBuilder_->getPosition()->getY());
                bool adjacent = (dx <= 1 && dy <= 1 && !(dx == 0 && dy == 0));
                Case* target = b_->getCase(x, y);
                if (!build_) isReachable = adjacent && (target->getBuilder() == nullptr) && (target->getFloor() <= lockBuilder_->getPosition()->getFloor() + 1) && (target->getFloor() < 4);
                else isReachable = adjacent && (target->getBuilder() == nullptr) && (target->getFloor() < 4);
            }

            renderElement(glm::vec3(x, -0.1f, y), glm::vec3(0.95f, 0.2f, 0.95f), glm::vec3(0.2f, 0.5f, 0.25f), 1.0f);

            if (isReachable) {
                glm::vec3 hlColor = build_ ? glm::vec3(0.2f, 0.5f, 0.9f) : glm::vec3(0.3f, 0.85f, 0.3f);
                renderElement(glm::vec3(x, 0.05f + (floors * 0.5f), y), glm::vec3(0.8f, 0.1f, 0.8f), hlColor, 0.4f);
            }

            for (int f = 0; f < floors; f++) {
                glm::vec3 color;
                glm::vec3 scale = glm::vec3(0.85f, 0.42f, 0.85f);
                glm::vec3 rot = glm::vec3(0.0f);
                glm::vec3 pos = glm::vec3(x, (f * 0.5f) + 0.25f, y);
                
                if (f == 0) color = glm::vec3(0.85f, 0.45f, 0.25f);      
                else if (f == 1) color = glm::vec3(0.60f, 0.85f, 0.80f); 
                else if (f == 2) color = glm::vec3(0.95f, 0.95f, 0.95f); 
                else if (f == 3) {
                    color = glm::vec3(0.1f, 0.3f, 0.9f);                 
                    scale = glm::vec3(0.7f, 0.35f, 0.7f);
                }
                
                // --- ANIMATION DE CONSTRUCTION EN DEUX PHASES ---
                if (buildAnimActive_ && x == buildAnimX_ && y == buildAnimY_ && f == buildAnimFloor_ - 1) {
                    float totalAnimTime = glfwGetTime() - buildAnimStartTime_;
                    float t = totalAnimTime / 0.4f; // 0 à 1 pour l'impact, 1 à 2 pour l'upgrade
                    
                    if (t < 2.0f) {
                        // PHASE 1 : MOUVEMENT ET IMPACT (0 à 0.4s)
                        if (t <= 1.0f) {
                            if (buildAnimFloor_ == 1) { 
                                scale.y *= t;
                                pos.y -= (1.0f - t) * 0.25f;
                            } 
                            else if (buildAnimFloor_ == 2) { 
                                rot.y = (1.0f - t) * 360.0f;
                                scale.x *= (0.5f + 0.5f * t);
                                scale.z *= (0.5f + 0.5f * t);
                            } 
                            else if (buildAnimFloor_ == 3) { 
                                float bounce = abs(cos(t * 3.14159f * 1.5f)) * (1.0f - t);
                                pos.y += bounce * 1.5f;
                            } 
                            else if (buildAnimFloor_ == 4) { 
                                pos.y += (1.0f - t) * 4.0f; 
                                float flash = 1.0f - (t * 2.0f);
                                if (flash > 0) color = color + glm::vec3(flash); 
                            }
                        } 
                        // PHASE 2 : L'EFFET UPGRADE MAGIQUE (0.4s à 0.8s)
                        else {
                            if (buildAnimFloor_ < 4) { // On ne fait l'upgrade que pour les blocs normaux, pas le dôme
                                
                                // On joue le son une seule fois
                                if (!buildUpgradeSoundPlayed_) {
                                    playSoundFX("sounds/upgrade.mp3");
                                    buildUpgradeSoundPlayed_ = true;
                                }
                                
                                // Effet de scintillement (flash blanc) et léger grossissement
                                float shine = sin((t - 1.0f) * 3.14159f); // Monte et descend doucement
                                color = color + glm::vec3(shine * 0.6f);  // Flash blanc
                                scale = scale + glm::vec3(shine * 0.08f); // Petit rebond / gonflement
                            }
                        }
                    } else { 
                        buildAnimActive_ = false; 
                    }
                }
                
                renderElement(pos, scale, color, 1.0f, rot);
            }

            if (b_->getCase(x, y)->getBuilder()) {
                Builder* p = b_->getCase(x, y)->getBuilder();
                bool isSel = (p == lockBuilder_);
                
                if (moveAnimActive_ && p == movingBuilder_) {
                    float animTime = (glfwGetTime() - moveAnimStartTime_) / 0.4f;
                    if (animTime < 1.0f) {
                        float t = animTime;
                        float curX = moveAnimStartX_ + (moveAnimEndX_ - moveAnimStartX_) * t;
                        float curY = moveAnimStartY_ + (moveAnimEndY_ - moveAnimStartY_) * t;
                        float startHeight = (moveAnimStartFloor_ * 0.5f) + 0.25f;
                        float endHeight = (moveAnimEndFloor_ * 0.5f) + 0.25f;
                        float curHeight = startHeight + (endHeight - startHeight) * t;
                        curHeight += sin(t * 3.14159f) * 1.2f; 
                        renderPawn(glm::vec3(curX, curHeight, curY), p->getPlayer(), isSel);
                    } else {
                        renderPawn(glm::vec3(x, (floors * 0.5f) + 0.25f, y), p->getPlayer(), isSel);
                        moveAnimActive_ = false;
                    }
                } else {
                    renderPawn(glm::vec3(x, (floors * 0.5f) + 0.25f, y), p->getPlayer(), isSel);
                }
            }

            if (x == cursorX_ && y == cursorY_) {
                float intensity = 0.6f + 0.4f * sin(glfwGetTime() * 6.0f);
                renderElement(glm::vec3(x, (floors * 0.5f) + 0.05f, y), glm::vec3(1.05f, 0.5f, 1.05f), glm::vec3(1.0f, 0.6f, 0.0f), intensity);
            }
        }
    }
}

void View::renderPawn(const glm::vec3& pos, int player, bool isSelected) {
    glm::vec3 baseColor = (player == 0) ? glm::vec3(0.9f, 0.2f, 0.2f) : glm::vec3(0.9f, 0.8f, 0.1f);
    if (isSelected) {
        float p = 0.7f + 0.3f * sin(glfwGetTime() * 10.0f);
        baseColor = baseColor * p + glm::vec3(0.2f); 
    }
    renderElement(pos + glm::vec3(0.0f, 0.2f, 0.0f), glm::vec3(0.45f, 0.4f, 0.45f), baseColor, 1.0f);
    glm::vec3 headColor = baseColor + glm::vec3(0.2f); 
    renderElement(pos + glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f), headColor, 1.0f);
}

void View::renderElement(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& color, float alpha, glm::vec3 rotation) {
    glm::mat4 model = glm::mat4(1.0f);
    float floatOffset = 0.0f;
    if (!win_) floatOffset = sin(glfwGetTime() * 1.5f) * 0.08f; 
    
    model = glm::translate(model, position + glm::vec3(0.0f, floatOffset, 0.0f));
    if (rotation.x != 0.0f) model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    if (rotation.y != 0.0f) model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    if (rotation.z != 0.0f) model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    
    s_->setVec3("caseColor", color.x, color.y, color.z);
    s_->setFloat("alpha", alpha); 
    s_->setMat4("model", model);
    m_->draw(*s_);
}

void View::winner(bool is3D, int p) {}

GLFWwindow* View::getWindow() { return window_; }

void View::processInput(GLFWwindow *window, santorini::Controller &c)
{
    if (win_) {
        std::string winnerName = c.getPlayerName(winnerId_);
        std::string title = "VICTOIRE DE " + winnerName + " !!! (Appuyez sur Q, A ou Echap pour quitter)";
        glfwSetWindowTitle(window, title.c_str());
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        return; 
    }

    float camSpeed = 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camAngle_ -= camSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) camAngle_ += camSpeed;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) camElevation_ += camSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camElevation_ -= camSpeed;
    if (camElevation_ > 89.0f) camElevation_ = 89.0f;
    if (camElevation_ < 10.0f) camElevation_ = 10.0f;

    std::string currentPlayer = c.getPlayerName(c.getCurrentPlayer());
    if (c.isOnlineMode() && !c.isMyTurn()) glfwSetWindowTitle(window, ("Tour de l'adversaire (" + c.getPlayerName(c.getCurrentPlayer() == 0 ? 1 : 0) + ") - Patientez...").c_str());
    else if (!lock_ && !build_) glfwSetWindowTitle(window, (currentPlayer + " - A votre tour ! (Selectionnez avec ENTREE)").c_str());
    else if (lock_ && !build_) glfwSetWindowTitle(window, (currentPlayer + " - Deplacement : choisissez une case verte").c_str());
    else if (lock_ && build_) glfwSetWindowTitle(window, (currentPlayer + " - Construction : choisissez une case bleue").c_str());

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { if (!keyUpPressed_) { if (cursorY_ > 0) cursorY_--; keyUpPressed_ = true; } } else keyUpPressed_ = false;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { if (!keyDownPressed_) { if (cursorY_ < 4) cursorY_++; keyDownPressed_ = true; } } else keyDownPressed_ = false;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { if (!keyRightPressed_) { if (cursorX_ < 4) cursorX_++; keyRightPressed_ = true; } } else keyRightPressed_ = false;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { if (!keyLeftPressed_) { if (cursorX_ > 0) cursorX_--; keyLeftPressed_ = true; } } else keyLeftPressed_ = false;

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if (!keyEnterPressed_) {
            if (!lock_ && !build_) {
                auto targetBuilder = b_->getCase(cursorX_, cursorY_)->getBuilder();
                if (targetBuilder && targetBuilder->getPlayer() == c.getCurrentPlayer()) { lockX_ = cursorX_; lockY_ = cursorY_; lockBuilder_ = targetBuilder; lock_ = true; }
            } 
            else if (lock_ && !build_) {
                int moveResult = c.selectMove(lockBuilder_->getId(), cursorX_, cursorY_);
                if (moveResult == 1) { lockX_ = cursorX_; lockY_ = cursorY_; build_ = true; } 
                else if (moveResult == 2) { setWinner(c.getCurrentPlayer()); }
            }
            else if (lock_ && build_) {
                if (c.selectBuild(lockBuilder_->getId(), cursorX_, cursorY_)) {
                    lock_ = false; build_ = false; lockX_ = -1; lockY_ = -1; lockBuilder_ = nullptr;
                }
            }
            keyEnterPressed_ = true;
        }
    } else { keyEnterPressed_ = false; }
}

GLenum View::glCheckError( const char * file, int line ) const noexcept { return glGetError(); }
void View::debugMat( glm::mat4 m ) const noexcept {}
void View::debugMat( glm::mat3 m ) const noexcept {}
void View::debugMat( glm::mat2 m ) const noexcept {}