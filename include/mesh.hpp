#include <glad/glad.h>
#include <shaders/shader.hpp>

class Mesh {

    public:

        Mesh();

        void draw(Shader &s);

    private:

        unsigned int vbo;
        unsigned int vao;
        unsigned int ebo;
};