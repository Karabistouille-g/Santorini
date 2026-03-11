#ifndef VIEW_HPP
#define VIEW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/glext.h>
#include <glm/glm.hpp>
#include <memory>
#include <atomic>

#include "mesh.hpp"
#include "shaders/shader.hpp"
#include "board.hpp"
#include "builder.hpp"
#include "controller.hpp"

#define WINDOW_BASE_WIDTH 800
#define WINDOW_BASE_HEIGHT 600

class View
{
    public :
        static View & getInstance();
        void viewBoard();
        void winner( bool is3D, int p );

        // --- Gestion du gagnant ---
        void setWinner(int id);
        void setWin(bool value) { win_ = value; }
        bool isWon() const { return win_; }
        int getWinnerId() const { return winnerId_; }
    
        GLFWwindow* getWindow();
        void processInput(GLFWwindow *window, santorini::Controller &c);
        void setSuppressAnimations(bool v);

    private :
        static View & instance_;
        GLFWwindow * window_;

        View();
        ~View();

        View( const View & ) = delete;
        View & operator=( const View & ) = delete;
        View( View && ) = delete;
        View & operator=( View && ) = delete;

        GLenum glCheckError( const char * file = __FILE__, int line = __LINE__ ) const noexcept;
        void debugMat( glm::mat4 m ) const noexcept;
        void debugMat( glm::mat3 m ) const noexcept;
        void debugMat( glm::mat2 m ) const noexcept;

        std::unique_ptr<Shader> s_;
        std::unique_ptr<Mesh> m_;

        int cursorX_;
        int cursorY_;

        int lockX_;
        int lockY_;
        bool lock_;
        bool build_;
        bool win_;
        int winnerId_; 

        Builder* lockBuilder_;

        bool keyUpPressed_;
        bool keyDownPressed_;
        bool keyRightPressed_;
        bool keyLeftPressed_;
        bool keyEnterPressed_;

        Board* b_;

        float camAngle_ = 45.0f; 
        float camElevation_ = 35.0f;
        float camRadius_ = 9.0f;
        
        bool firstFrame_ = true;
        int prevFloors_[5][5];
        Builder* prevBuilderPos_[5][5];

        bool moveAnimActive_ = false;
        float moveAnimStartTime_ = 0.0f;
        int moveAnimStartX_ = 0, moveAnimStartY_ = 0;
        int moveAnimEndX_ = 0, moveAnimEndY_ = 0;
        int moveAnimStartFloor_ = 0, moveAnimEndFloor_ = 0;
        Builder* movingBuilder_ = nullptr;

        bool buildAnimActive_ = false;
        float buildAnimStartTime_ = 0.0f;
        int buildAnimX_ = 0, buildAnimY_ = 0;
        int buildAnimFloor_ = 0;
        
        // --- NOUVEAU : Flag pour gerer le son d'upgrade ---
        bool buildUpgradeSoundPlayed_ = false;

        // --- Animation victoire ---
        float winAnimStartTime_ = -1.0f;
        bool  winSoundPlayed_   = false;

        // --- Ecran d'intro multi (qui suis-je) ---
        float introStartTime_    = -1.0f;
        bool  introShown_        = false;

        // --- Suppression animations pendant reflexion IA ---
        std::atomic<bool> suppressAnimations_{false};
        // Snapshot du plateau juste avant que l'IA commence à réfléchir.
        // Pendant la réflexion, on rend CE snapshot au lieu du board live.
        int      frozenFloors_[5][5]   = {};
        Builder* frozenBuilders_[5][5] = {};
        // --- Etat courant pour le clignotement des pions ---
        int   currentPlayerId_  = 0;
        bool  isMyTurnCached_   = true;

        void triggerMoveAnimation(int sx, int sy, int ex, int ey, int sFloor, int eFloor, Builder* b);
        void triggerBuildAnimation(int x, int y, int floor);
        
        void renderElement(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& color, float alpha, glm::vec3 rotation = glm::vec3(0.0f));
        void renderPawn(const glm::vec3& pos, int player, bool isSelected);
};

#endif