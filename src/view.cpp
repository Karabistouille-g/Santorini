#include "view.hpp"
#include "case.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

extern void playSoundFX(const char* filepath);

View::View()
    : cursorX_(2), cursorY_(2),
      lockX_(-1), lockY_(-1),
      lock_(false), build_(false), win_(false), winnerId_(-1),
      lockBuilder_(nullptr),
      keyUpPressed_(false), keyDownPressed_(false),
      keyRightPressed_(false), keyLeftPressed_(false),
      b_(Board::getInstance())
{
    if (!glfwInit()) { std::cerr << "Failed to initialize GLFW" << std::endl; return; }

    // Callback erreur GLFW : affiche le vrai message au lieu de crasher silencieusement
    glfwSetErrorCallback([](int code, const char* desc) {
        std::cerr << "[GLFW Error " << code << "] " << desc << std::endl;
    });

    // OpenGL 3.3 Core : minimum supporté partout depuis 2010
    // (Linux natif, WSL2, PC de fac, iGPU Intel/AMD, Mesa)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // IMPORTANT : pas de GLFW_SAMPLES ici — le MSAA peut faire echouer la
    // creation de contexte sur certains drivers (VMs, iGPU anciens, Mesa basique)

    window_ = glfwCreateWindow(WINDOW_BASE_WIDTH, WINDOW_BASE_HEIGHT, "Santorini", nullptr, nullptr);
    if (!window_) {
        std::cerr << "[ ERROR ] Impossible de creer la fenetre OpenGL 3.3." << std::endl;
        std::cerr << "[ INFO  ] Verifiez que vos drivers graphiques sont a jour." << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[ ERROR ] Failed to initialize GLAD" << std::endl;
        return;
    }

    // Affiche la version OpenGL reelle — utile pour debugger sur PC inconnu
    std::cout << "[OpenGL] Version : " << glGetString(GL_VERSION)  << std::endl;
    std::cout << "[OpenGL] GPU     : " << glGetString(GL_RENDERER) << std::endl;

    glViewport(0, 0, WINDOW_BASE_WIDTH, WINDOW_BASE_HEIGHT);
    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow*, int w, int h) { glViewport(0, 0, w, h); });
    glfwSetKeyCallback(window_, [](GLFWwindow*, int, int, int, int) {});

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE); // Active MSAA

    s_ = std::make_unique<Shader>("../include/shaders/vertex.vert", "../include/shaders/fragment.frag", nullptr);
    m_ = std::make_unique<Mesh>();

    // Angle de caméra isométrique agréable par défaut
    camAngle_     = 45.0f;
    camElevation_ = 38.0f;
    camRadius_    = 10.5f;
}

View::~View() { s_.reset(); m_.reset(); glfwDestroyWindow(window_); glfwTerminate(); }

View& View::getInstance() { static View instance; return instance; }

void View::setWinner(int id) {
    if (!win_) {
        playSoundFX("sounds/win.wav");
        winAnimStartTime_ = (float)glfwGetTime();
    }
    win_      = true;
    winnerId_ = id;
}

void View::triggerMoveAnimation(int sx, int sy, int ex, int ey, int sFloor, int eFloor, Builder* b) {
    moveAnimActive_    = true;
    moveAnimStartX_    = sx; moveAnimStartY_ = sy;
    moveAnimEndX_      = ex; moveAnimEndY_   = ey;
    moveAnimStartFloor_= sFloor; moveAnimEndFloor_ = eFloor;
    movingBuilder_     = b;
    moveAnimStartTime_ = glfwGetTime();
    playSoundFX("sounds/move.wav");
}

void View::triggerBuildAnimation(int x, int y, int floor) {
    buildAnimActive_        = true;
    buildAnimX_             = x; buildAnimY_ = y;
    buildAnimFloor_         = floor;
    buildAnimStartTime_     = glfwGetTime();
    buildUpgradeSoundPlayed_= false;
    if (floor == 1)      playSoundFX("sounds/build1.wav");
    else if (floor == 2) playSoundFX("sounds/build2.wav");
    else if (floor == 3) playSoundFX("sounds/build3.wav");
    else if (floor == 4) playSoundFX("sounds/dome.wav");
}

// =============================================================================
// RENDU PRINCIPAL
// =============================================================================
void View::viewBoard() {

    // --- Détection d'animations (inchangé) ---
    if (firstFrame_) {
        for (int px = 0; px < 5; px++)
            for (int py = 0; py < 5; py++) {
                prevFloors_[px][py]     = b_->getCase(px, py)->getFloor();
                prevBuilderPos_[px][py] = b_->getCase(px, py)->getBuilder();
            }
        firstFrame_ = false;
    }
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            int     curF  = b_->getCase(x, y)->getFloor();
            Builder* curB = b_->getCase(x, y)->getBuilder();
            if (curF > prevFloors_[x][y]) triggerBuildAnimation(x, y, curF);
            if (curB != nullptr && prevBuilderPos_[x][y] != curB) {
                int oldX = x, oldY = y;
                for (int px = 0; px < 5; px++)
                    for (int py = 0; py < 5; py++)
                        if (prevBuilderPos_[px][py] == curB) { oldX = px; oldY = py; }
                triggerMoveAnimation(oldX, oldY, x, y, prevFloors_[oldX][oldY], curF, curB);
            }
        }
    }
    for (int x = 0; x < 5; x++)
        for (int y = 0; y < 5; y++) {
            prevFloors_[x][y]     = b_->getCase(x, y)->getFloor();
            prevBuilderPos_[x][y] = b_->getCase(x, y)->getBuilder();
        }

    // --- Couleur de fond ---
    float time = (float)glfwGetTime();
    if (win_) {
        float pulse = (sin(time * 4.0f) + 1.0f) * 0.5f;
        glClearColor(0.55f + 0.12f * pulse, 0.40f + 0.08f * pulse, 0.05f, 1.0f);
    } else {
        // Ciel nocturne doux, légèrement animé
        float cycle = sin(time * 0.12f) * 0.5f + 0.5f;
        glClearColor(0.04f + 0.03f * cycle, 0.06f + 0.03f * cycle, 0.14f + 0.05f * cycle, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    s_->activate();
    s_->setFloat("time", time);
    // Emissive à zéro par défaut
    s_->setVec3("emissive", 0.0f, 0.0f, 0.0f);
    s_->setFloat("emissiveStrength", 0.0f);

    // --- Caméra ---
    glm::mat4 projection = glm::perspective(glm::radians(42.0f),
        (float)WINDOW_BASE_WIDTH / (float)WINDOW_BASE_HEIGHT, 0.1f, 200.0f);
    float radAngle = glm::radians(camAngle_);
    float radElev  = glm::radians(camElevation_);
    float camX     = 2.0f + camRadius_ * cos(radElev) * sin(radAngle);
    float camY     = camRadius_ * sin(radElev);
    float camZ     = 2.0f + camRadius_ * cos(radElev) * cos(radAngle);
    glm::mat4 view = glm::lookAt(glm::vec3(camX, camY, camZ),
                                 glm::vec3(2.0f, 0.4f, 2.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));
    s_->setMat4("projection", projection);
    s_->setMat4("view", view);
    // Passer la position réelle de la caméra pour un spéculaire correct
    s_->setVec3("viewPos", camX, camY, camZ);

    // ==========================================================================
    // FOND : MER / VIDE INFINI (île flottante)
    // ==========================================================================
    renderElement(glm::vec3(2.0f, -6.0f, 2.0f),
                  glm::vec3(120.0f, 0.8f, 120.0f),
                  glm::vec3(0.03f, 0.06f, 0.16f), 1.0f);

    // Reflets animés sur la mer
    float waveA = sin(time * 0.7f) * 0.01f;
    float waveB = sin(time * 1.1f + 1.5f) * 0.01f;
    s_->setVec3("emissive", 0.04f + waveA, 0.08f + waveB, 0.22f);
    s_->setFloat("emissiveStrength", 0.3f);
    renderElement(glm::vec3(2.0f, -5.64f, 2.0f),
                  glm::vec3(110.0f, 0.05f, 110.0f),
                  glm::vec3(0.05f, 0.12f, 0.35f), 0.55f);
    s_->setVec3("emissive", 0.0f, 0.0f, 0.0f);
    s_->setFloat("emissiveStrength", 0.0f);

    // ==========================================================================
    // PLATEAU : Socle en pierre + cadre en bois
    // ==========================================================================
    glm::vec3 stoneColor = glm::vec3(0.52f, 0.46f, 0.38f);
    glm::vec3 woodColor  = glm::vec3(0.30f, 0.18f, 0.08f);
    glm::vec3 woodDark   = glm::vec3(0.20f, 0.11f, 0.04f);

    // Socle principal (légèrement en relief)
    renderElement(glm::vec3(2.0f, -0.32f, 2.0f), glm::vec3(5.40f, 0.42f, 5.40f), stoneColor, 1.0f);
    // Sous-socle plus large (effet de base)
    renderElement(glm::vec3(2.0f, -0.60f, 2.0f), glm::vec3(5.80f, 0.22f, 5.80f), woodDark, 1.0f);

    // Cadre en bois (4 côtés)
    float fw = 5.80f, ft = 0.22f, fh = 0.18f, fy = -0.16f;
    renderElement(glm::vec3(2.0f,  fy, -0.68f), glm::vec3(fw, fh, ft), woodColor, 1.0f);
    renderElement(glm::vec3(2.0f,  fy,  4.68f), glm::vec3(fw, fh, ft), woodColor, 1.0f);
    renderElement(glm::vec3(-0.68f,fy,  2.0f),  glm::vec3(ft, fh, 5.36f), woodColor, 1.0f);
    renderElement(glm::vec3( 4.68f,fy,  2.0f),  glm::vec3(ft, fh, 5.36f), woodColor, 1.0f);

    // ==========================================================================
    // CASES + BÂTIMENTS + PIONS
    // ==========================================================================

    // Palette de couleurs par étage (matériaux distincts et lisibles)
    static const glm::vec3 floorColors[4] = {
        glm::vec3(0.80f, 0.52f, 0.28f), // Niveau 1 : brique terracotta chaude
        glm::vec3(0.58f, 0.72f, 0.76f), // Niveau 2 : pierre gris-bleu
        glm::vec3(0.92f, 0.92f, 0.95f), // Niveau 3 : marbre blanc
        glm::vec3(0.12f, 0.22f, 0.82f)  // Niveau 4 : dôme bleu saphir
    };
    // Chaque niveau rétrécit → effet de pyramide / vraie tour
    static const float floorSX[4] = { 0.88f, 0.74f, 0.60f, 0.50f };
    static const float floorSZ[4] = { 0.88f, 0.74f, 0.60f, 0.50f };
    static const float floorSY[4] = { 0.50f, 0.50f, 0.50f, 0.22f }; // Dôme plus plat

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            int floors = b_->getCase(x, y)->getFloor();

            // --- Dalle en damier ---
            bool isLight  = (x + y) % 2 == 0;
            glm::vec3 tileColor = isLight
                ? glm::vec3(0.84f, 0.76f, 0.62f)  // Sable clair
                : glm::vec3(0.40f, 0.30f, 0.18f);  // Terre sombre
            renderElement(glm::vec3(x, -0.10f, y), glm::vec3(0.97f, 0.09f, 0.97f), tileColor, 1.0f);

            // --- Indicateur de case atteignable ---
            bool isReachable = false;
            if (lock_ && lockBuilder_ != nullptr) {
                int dx = abs(x - lockBuilder_->getPosition()->getX());
                int dy = abs(y - lockBuilder_->getPosition()->getY());
                bool adjacent = (dx <= 1 && dy <= 1 && !(dx == 0 && dy == 0));
                Case* target  = b_->getCase(x, y);
                if (!build_)
                    isReachable = adjacent && !target->getBuilder()
                                  && target->getFloor() <= lockBuilder_->getPosition()->getFloor() + 1
                                  && target->getFloor() < 4;
                else
                    isReachable = adjacent && !target->getBuilder() && target->getFloor() < 4;
            }

            if (isReachable) {
                float pulse = 0.45f + 0.55f * (sin(time * 5.5f) * 0.5f + 0.5f);
                glm::vec3 hlColor = build_
                    ? glm::vec3(0.10f, 0.28f, 1.00f)   // Bleu construction
                    : glm::vec3(0.10f, 0.92f, 0.32f);   // Vert déplacement
                float baseH = floors * 0.5f + 0.02f;
                // Disque plat + léger effet emissif
                s_->setVec3("emissive", hlColor.x * pulse, hlColor.y * pulse, hlColor.z * pulse);
                s_->setFloat("emissiveStrength", 0.55f);
                renderElement(glm::vec3(x, baseH, y),
                              glm::vec3(0.86f, 0.05f, 0.86f),
                              hlColor, 0.28f + pulse * 0.22f);
                s_->setVec3("emissive", 0.0f, 0.0f, 0.0f);
                s_->setFloat("emissiveStrength", 0.0f);
            }

            // --- Bâtiments (étages empilés avec rétrécissement) ---
            for (int f = 0; f < floors; f++) {
                // Chaque étage se pose au-dessus du précédent
                float posY  = f * 0.5f + floorSY[f] * 0.5f;
                glm::vec3 pos   = glm::vec3((float)x, posY, (float)y);
                glm::vec3 scale = glm::vec3(floorSX[f], floorSY[f], floorSZ[f]);
                glm::vec3 color = floorColors[f];

                // --- Animation de construction ---
                if (buildAnimActive_ && x == buildAnimX_ && y == buildAnimY_ && f == buildAnimFloor_ - 1) {
                    float t = (float)(glfwGetTime() - buildAnimStartTime_) / 0.4f;

                    if (t < 2.0f) {
                        if (t <= 1.0f) {
                            // Phase 1 : apparition
                            if (buildAnimFloor_ == 1) {
                                scale.y *= t;
                                pos.y   -= (1.0f - t) * 0.25f;
                            } else if (buildAnimFloor_ == 2) {
                                float s = 0.5f + 0.5f * t;
                                scale.x *= s; scale.z *= s;
                            } else if (buildAnimFloor_ == 3) {
                                pos.y += std::abs(std::cos(t * 3.14159f * 1.5f)) * (1.0f - t) * 1.4f;
                            } else if (buildAnimFloor_ == 4) {
                                pos.y  += (1.0f - t) * 3.5f;
                                float flash = 1.0f - t * 2.0f;
                                if (flash > 0) color = color + glm::vec3(flash * 0.6f);
                            }
                        } else {
                            // Phase 2 : effet "upgrade" scintillant
                            if (buildAnimFloor_ < 4) {
                                if (!buildUpgradeSoundPlayed_) {
                                    playSoundFX("sounds/upgrade.wav");
                                    buildUpgradeSoundPlayed_ = true;
                                }
                                float shine = std::sin((t - 1.0f) * 3.14159f);
                                color += glm::vec3(shine * 0.55f);
                                scale += glm::vec3(shine * 0.07f);
                                s_->setVec3("emissive", shine * 0.6f, shine * 0.5f, shine * 0.2f);
                                s_->setFloat("emissiveStrength", shine * 0.7f);
                            }
                        }
                    } else {
                        buildAnimActive_ = false;
                    }
                }

                // Dôme : lueur magique permanente
                if (f == 3) {
                    float domeGlow = 0.25f + 0.20f * std::sin(time * 2.2f);
                    s_->setVec3("emissive", 0.08f, 0.18f, domeGlow + 0.35f);
                    s_->setFloat("emissiveStrength", 0.50f);
                }

                renderElement(pos, scale, color, 1.0f);

                // Réinitialiser l'emissif après chaque étage
                s_->setVec3("emissive", 0.0f, 0.0f, 0.0f);
                s_->setFloat("emissiveStrength", 0.0f);
            }

            // --- Pions ---
            if (b_->getCase(x, y)->getBuilder()) {
                Builder* p    = b_->getCase(x, y)->getBuilder();
                bool isSel    = (p == lockBuilder_);

                if (moveAnimActive_ && p == movingBuilder_) {
                    float animTime = (float)(glfwGetTime() - moveAnimStartTime_) / 0.4f;
                    if (animTime < 1.0f) {
                        float t    = animTime;
                        float curX = moveAnimStartX_ + (moveAnimEndX_ - moveAnimStartX_) * t;
                        float curZ = moveAnimStartY_ + (moveAnimEndY_ - moveAnimStartY_) * t;
                        float sH   = moveAnimStartFloor_ * 0.5f + 0.25f;
                        float eH   = moveAnimEndFloor_   * 0.5f + 0.25f;
                        float curH = sH + (eH - sH) * t + std::sin(t * 3.14159f) * 1.3f;
                        renderPawn(glm::vec3(curX, curH, curZ), p->getPlayer(), isSel);
                    } else {
                        renderPawn(glm::vec3((float)x, floors * 0.5f + 0.25f, (float)y), p->getPlayer(), isSel);
                        moveAnimActive_ = false;
                    }
                } else {
                    renderPawn(glm::vec3((float)x, floors * 0.5f + 0.25f, (float)y), p->getPlayer(), isSel);
                }
            }

            // --- Curseur : 4 coins en "bracket" animés ---
            if (x == cursorX_ && y == cursorY_) {
                float intensity  = 0.65f + 0.35f * std::sin(time * 7.0f);
                float baseH      = floors * 0.5f + 0.07f;
                glm::vec3 cColor = glm::vec3(1.0f, 0.68f, 0.0f) * intensity;

                s_->setVec3("emissive", cColor.x, cColor.y, 0.0f);
                s_->setFloat("emissiveStrength", 0.9f);

                // Épaisseur et longueur des coins
                float bw = 0.04f, bd = 0.28f, bh = 0.14f;
                float off = 0.37f; // distance du centre

                // Coin haut-gauche
                renderElement(glm::vec3(x - off, baseH, y - off + bd * 0.5f - 0.02f), glm::vec3(bw, bh, bd), cColor, intensity);
                renderElement(glm::vec3(x - off + bd * 0.5f - 0.02f, baseH, y - off), glm::vec3(bd, bh, bw), cColor, intensity);
                // Coin haut-droit
                renderElement(glm::vec3(x + off, baseH, y - off + bd * 0.5f - 0.02f), glm::vec3(bw, bh, bd), cColor, intensity);
                renderElement(glm::vec3(x + off - bd * 0.5f + 0.02f, baseH, y - off), glm::vec3(bd, bh, bw), cColor, intensity);
                // Coin bas-gauche
                renderElement(glm::vec3(x - off, baseH, y + off - bd * 0.5f + 0.02f), glm::vec3(bw, bh, bd), cColor, intensity);
                renderElement(glm::vec3(x - off + bd * 0.5f - 0.02f, baseH, y + off), glm::vec3(bd, bh, bw), cColor, intensity);
                // Coin bas-droit
                renderElement(glm::vec3(x + off, baseH, y + off - bd * 0.5f + 0.02f), glm::vec3(bw, bh, bd), cColor, intensity);
                renderElement(glm::vec3(x + off - bd * 0.5f + 0.02f, baseH, y + off), glm::vec3(bd, bh, bw), cColor, intensity);

                s_->setVec3("emissive", 0.0f, 0.0f, 0.0f);
                s_->setFloat("emissiveStrength", 0.0f);
            }
        }
    }
}

// =============================================================================
// RENDU D'UN PION (forme meeple : base disque + corps + col + tête)
// =============================================================================
void View::renderPawn(const glm::vec3& pos, int player, bool isSelected) {
    // Joueur 0 : rouge écarlate   |   Joueur 1 : bleu royal
    glm::vec3 mainColor   = (player == 0) ? glm::vec3(0.88f, 0.12f, 0.12f) : glm::vec3(0.15f, 0.35f, 0.90f);
    glm::vec3 accentColor = (player == 0) ? glm::vec3(1.00f, 0.72f, 0.08f) : glm::vec3(0.08f, 0.82f, 0.90f);

    // Légère oscillation individuelle pour donner vie aux pions
    float bob = std::sin((float)glfwGetTime() * 2.2f + player * 1.8f) * 0.035f;

    if (isSelected) {
        float glow = 0.55f + 0.45f * std::sin((float)glfwGetTime() * 9.0f);
        s_->setVec3("emissive", mainColor.x * glow, mainColor.y * glow, mainColor.z * glow);
        s_->setFloat("emissiveStrength", 0.65f);
    }

    glm::vec3 p = pos + glm::vec3(0.0f, bob, 0.0f);

    // Base disque (large, plat)
    renderElement(p + glm::vec3(0, 0.040f, 0), glm::vec3(0.46f, 0.075f, 0.46f), mainColor,   1.0f);
    // Corps bas (légèrement plus étroit)
    renderElement(p + glm::vec3(0, 0.155f, 0), glm::vec3(0.33f, 0.200f, 0.33f), mainColor,   1.0f);
    // Ceinture / col (couleur accent)
    renderElement(p + glm::vec3(0, 0.280f, 0), glm::vec3(0.28f, 0.085f, 0.28f), accentColor, 1.0f);
    // Tête
    renderElement(p + glm::vec3(0, 0.390f, 0), glm::vec3(0.26f, 0.250f, 0.26f), mainColor,   1.0f);

    if (isSelected) {
        s_->setVec3("emissive", 0.0f, 0.0f, 0.0f);
        s_->setFloat("emissiveStrength", 0.0f);
    }
}

// =============================================================================
// RENDU D'UN ÉLÉMENT (cube transformé)
// =============================================================================
void View::renderElement(const glm::vec3& position, const glm::vec3& scale,
                         const glm::vec3& color, float alpha, glm::vec3 rotation)
{
    glm::mat4 model = glm::mat4(1.0f);

    // Légère respiration douce de toute la scène (île flottante)
    float floatOffset = win_ ? 0.0f : std::sin((float)glfwGetTime() * 1.4f) * 0.025f;

    model = glm::translate(model, position + glm::vec3(0.0f, floatOffset, 0.0f));
    if (rotation.x != 0.0f) model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    if (rotation.y != 0.0f) model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    if (rotation.z != 0.0f) model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, scale);

    s_->setVec3("caseColor", color.x, color.y, color.z);
    s_->setFloat("alpha", alpha);
    s_->setMat4("model", model);
    m_->draw(*s_);
}

void View::winner(bool is3D, int p) {
    // Toute la magie est dans viewBoard() (fond doré pulsant).
    // Ici on ajoute les confettis/particules flottantes au-dessus du plateau.
    if (!win_ || winAnimStartTime_ < 0.0f) return;

    float t    = (float)glfwGetTime() - winAnimStartTime_;
    float time = (float)glfwGetTime();

    // Couleur gagnant
    glm::vec3 winColor = (winnerId_ == 0)
        ? glm::vec3(1.0f, 0.18f, 0.18f)   // rouge
        : glm::vec3(0.25f, 0.45f, 1.0f);  // bleu

    // --- Pions gagnants : rebond exagéré vers le haut ---
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            Builder* b = b_->getCase(x, y)->getBuilder();
            if (!b || b->getPlayer() != winnerId_) continue;
            int fl = b_->getCase(x, y)->getFloor();
            float baseH = fl * 0.5f + 0.25f;
            // Rebond sinusoïdal, amplitude décroissante avec le temps
            float bounce = std::abs(std::sin(time * 4.5f + x)) * std::max(0.0f, 1.0f - t * 0.15f) * 2.0f;
            // Emissive fort sur les gagnants
            s_->setVec3("emissive", winColor.x, winColor.y, winColor.z);
            s_->setFloat("emissiveStrength", 1.0f + 0.5f * std::sin(time * 8.0f));
            renderPawn(glm::vec3((float)x, baseH + bounce, (float)y), winnerId_, false);
            s_->setVec3("emissive", 0.0f, 0.0f, 0.0f);
            s_->setFloat("emissiveStrength", 0.0f);
        }
    }

    // --- Particules confettis (cubes colorés qui tombent) ---
    // Seed fixe par index pour des trajectoires stables
    static const int NUM_PARTICLES = 24;
    for (int i = 0; i < NUM_PARTICLES; i++) {
        float seed  = (float)i * 1.618f;
        float orbitR = 1.5f + std::fmod(seed * 0.7f, 2.5f);
        float speed  = 0.6f + std::fmod(seed * 0.3f, 1.0f);
        float phase  = seed * 2.0f;
        float height = 2.5f + std::sin(time * speed + phase) * 1.5f + t * 0.3f;
        float cx     = 2.0f + orbitR * std::cos(time * speed * 0.8f + phase);
        float cz     = 2.0f + orbitR * std::sin(time * speed * 0.8f + phase);

        // Alterner entre couleur gagnant et or/blanc
        glm::vec3 pColor;
        int mod = i % 3;
        if      (mod == 0) pColor = winColor;
        else if (mod == 1) pColor = glm::vec3(1.0f, 0.85f, 0.0f);  // or
        else               pColor = glm::vec3(1.0f, 1.0f, 1.0f);   // blanc

        float pulse = 0.7f + 0.3f * std::sin(time * 3.0f + seed);
        s_->setVec3("emissive", pColor.x * pulse, pColor.y * pulse, pColor.z * pulse);
        s_->setFloat("emissiveStrength", 0.8f);
        renderElement(glm::vec3(cx, height, cz),
                      glm::vec3(0.08f, 0.08f, 0.08f),
                      pColor, 0.9f,
                      glm::vec3(time * 90.0f * speed, time * 60.0f * speed, 0.0f));
    }
    s_->setVec3("emissive", 0.0f, 0.0f, 0.0f);
    s_->setFloat("emissiveStrength", 0.0f);
}

GLFWwindow* View::getWindow() { return window_; }

// =============================================================================
// GESTION DES ENTRÉES (inchangé fonctionnellement, titre mis à jour)
// =============================================================================
void View::processInput(GLFWwindow* window, santorini::Controller& c)
{
    // --- INTRO MULTI : afficher pendant 5s qui on est ---
    if (c.isOnlineMode() && !introShown_) {
        if (introStartTime_ < 0.0f) introStartTime_ = (float)glfwGetTime();
        float elapsed = (float)glfwGetTime() - introStartTime_;
        if (elapsed < 5.0f) {
            std::string role  = (c.getCurrentPlayer() == 0) ? "ROUGE (Joueur 1)" : "BLEU (Joueur 2)";
            std::string order = (c.getCurrentPlayer() == 0) ? "vous commencez" : "l'adversaire commence";
            std::string msg   = "Santorini  |  Vous etes : " + role + "  —  " + order
                              + "  (" + std::to_string(5 - (int)elapsed) + "s)";
            glfwSetWindowTitle(window, msg.c_str());
            return;
        }
        introShown_ = true;
    }

    if (win_) {
        std::string winnerName = c.getPlayerName(winnerId_);
        std::string title = "✦ VICTOIRE DE " + winnerName + " ✦  (Echap / Q / A pour quitter)";
        glfwSetWindowTitle(window, title.c_str());
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_Q)      == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_A)      == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        return;
    }

    // Caméra (ZQAD / WASD)
    float camSpeed = 0.9f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camAngle_ -= camSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) camAngle_ += camSpeed;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) camElevation_ += camSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camElevation_ -= camSpeed;
    if (camElevation_ > 88.0f) camElevation_ = 88.0f;
    if (camElevation_ < 8.0f)  camElevation_ = 8.0f;

    // Titre de fenêtre contextuel
    std::string curPlayer = c.getPlayerName(c.getCurrentPlayer());
    if (c.isOnlineMode() && !c.isMyTurn())
        glfwSetWindowTitle(window, ("Tour adversaire (" + c.getPlayerName(c.getCurrentPlayer() == 0 ? 1 : 0) + ") — Patientez…").c_str());
    else if (!lock_ && !build_)
        glfwSetWindowTitle(window, ("Santorini  |  " + curPlayer + " — Sélectionnez un pion [ENTRÉE]").c_str());
    else if (lock_ && !build_)
        glfwSetWindowTitle(window, ("Santorini  |  " + curPlayer + " — Déplacez-vous vers une case verte").c_str());
    else
        glfwSetWindowTitle(window, ("Santorini  |  " + curPlayer + " — Construisez sur une case bleue").c_str());

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Navigation curseur
    if (glfwGetKey(window, GLFW_KEY_UP)    == GLFW_PRESS) { if (!keyUpPressed_)    { if (cursorY_ > 0) cursorY_--; keyUpPressed_    = true; } } else keyUpPressed_    = false;
    if (glfwGetKey(window, GLFW_KEY_DOWN)  == GLFW_PRESS) { if (!keyDownPressed_)  { if (cursorY_ < 4) cursorY_++; keyDownPressed_  = true; } } else keyDownPressed_  = false;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { if (!keyRightPressed_) { if (cursorX_ < 4) cursorX_++; keyRightPressed_ = true; } } else keyRightPressed_ = false;
    if (glfwGetKey(window, GLFW_KEY_LEFT)  == GLFW_PRESS) { if (!keyLeftPressed_)  { if (cursorX_ > 0) cursorX_--; keyLeftPressed_  = true; } } else keyLeftPressed_  = false;

    // Action ENTRÉE
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if (!keyEnterPressed_) {
            if (!lock_ && !build_) {
                auto tb = b_->getCase(cursorX_, cursorY_)->getBuilder();
                if (tb && tb->getPlayer() == c.getCurrentPlayer())
                { lockX_ = cursorX_; lockY_ = cursorY_; lockBuilder_ = tb; lock_ = true; }
            }
            else if (lock_ && !build_) {
                int res = c.selectMove(lockBuilder_->getId(), cursorX_, cursorY_);
                if      (res == 1) { lockX_ = cursorX_; lockY_ = cursorY_; build_ = true; }
                else if (res == 2) { setWinner(c.getCurrentPlayer()); }
            }
            else if (lock_ && build_) {
                if (c.selectBuild(lockBuilder_->getId(), cursorX_, cursorY_))
                { lock_ = false; build_ = false; lockX_ = -1; lockY_ = -1; lockBuilder_ = nullptr; }
            }
            keyEnterPressed_ = true;
        }
    } else { keyEnterPressed_ = false; }
}

GLenum View::glCheckError(const char*, int) const noexcept { return glGetError(); }
void View::debugMat(glm::mat4) const noexcept {}
void View::debugMat(glm::mat3) const noexcept {}
void View::debugMat(glm::mat2) const noexcept {}