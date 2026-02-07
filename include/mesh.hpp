#include <glad/glad.h>
#include <shaders/shader.hpp>

class Mesh {

    public:

        Mesh();

        void draw(Shader &s);

    private:

        unsigned int VBO;
        unsigned int VAO;
        unsigned int EBO;
};