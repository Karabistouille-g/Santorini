#include <shaders/shader.hpp>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {

    // =========================================================================
    // VERTEX SHADER
    // =========================================================================
    const char* vShaderCode = R"GLSL(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

void main() {
    FragPos  = vec3(model * vec4(aPos, 1.0));
    Normal   = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)GLSL";

    // =========================================================================
    // FRAGMENT SHADER
    // Améliorations vs version originale :
    //   • Spéculaire Blinn-Phong (plus réaliste que Phong classique)
    //   • Deux sources lumineuses (soleil chaud + lumière de remplissage froide)
    //   • Éclairage Fresnel/rim sur les bords des objets (donne du volume)
    //   • Emissive uniform (pour les sélections, dômes, surbrillances)
    //   • viewPos passé depuis le CPU pour un specular précis selon la caméra
    // =========================================================================
    const char* fShaderCode = R"GLSL(
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3  caseColor;
uniform float alpha;
uniform vec3  emissive;
uniform float emissiveStrength;
uniform vec3  viewPos;
uniform float time;

void main() {
    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // --- Lumière 1 : Soleil principal (chaud, vient d'en haut à droite) ---
    vec3 lightDir1   = normalize(vec3(1.2, 2.5, 0.8));
    vec3 lightColor1 = vec3(1.00, 0.96, 0.88);

    // --- Lumière 2 : Remplissage (froide, subtile, vient de la gauche basse) ---
    vec3 lightDir2   = normalize(vec3(-0.9, 0.4, -0.6));
    vec3 lightColor2 = vec3(0.28, 0.38, 0.65);

    // Ambiante
    vec3 ambient = 0.22 * caseColor;

    // Diffuse (deux lampes, la seconde atténuée)
    float diff1  = max(dot(norm, lightDir1), 0.0);
    float diff2  = max(dot(norm, lightDir2), 0.0) * 0.28;
    vec3  diffuse = (diff1 * lightColor1 + diff2 * lightColor2) * caseColor;

    // Spéculaire Blinn-Phong (vecteur à mi-chemin, plus propre que Phong)
    vec3  halfway = normalize(lightDir1 + viewDir);
    float spec    = pow(max(dot(norm, halfway), 0.0), 80.0);
    vec3  specular = 0.30 * spec * lightColor1;

    // Éclairage Fresnel/Rim (bords lumineux bleutés, donne de la profondeur)
    float rim      = 1.0 - max(dot(viewDir, norm), 0.0);
    rim            = pow(rim, 3.8) * 0.40;
    vec3  rimLight = rim * vec3(0.35, 0.50, 1.00);

    // Emissive (sélections, dôme magique, curseur...)
    vec3 emissiveGlow = emissive * emissiveStrength;

    vec3 finalColor = ambient + diffuse + specular + rimLight + emissiveGlow;
    FragColor = vec4(finalColor, alpha);
}
)GLSL";

    unsigned int vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    compileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    compileErrors(fragment, "FRAGMENT");

    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    compileErrors(id, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::activate()                                              const noexcept { glUseProgram(id); }
void Shader::setFloat(const char* name, float value)                const noexcept { glUniform1f(glGetUniformLocation(id, name), value); }
void Shader::setInt(const char* name, int value)                    const noexcept { glUniform1i(glGetUniformLocation(id, name), value); }
void Shader::setBool(const char* name, bool value)                  const noexcept { glUniform1i(glGetUniformLocation(id, name), (int)value); }
void Shader::setIntArray(const char* name, int* value, unsigned int count) const noexcept { glUniform1iv(glGetUniformLocation(id, name), count, value); }
void Shader::setVec2(const char* name, float x, float y)            const noexcept { glUniform2f(glGetUniformLocation(id, name), x, y); }
void Shader::setVec3(const char* name, float x, float y, float z)   const noexcept { glUniform3f(glGetUniformLocation(id, name), x, y, z); }
void Shader::setVec4(const char* name, float x, float y, float z, float w) const noexcept { glUniform4f(glGetUniformLocation(id, name), x, y, z, w); }
void Shader::setVec2(const char* name, glm::vec2& v)                const noexcept { glUniform2fv(glGetUniformLocation(id, name), 1, &v[0]); }
void Shader::setVec3(const char* name, glm::vec3& v)                const noexcept { glUniform3fv(glGetUniformLocation(id, name), 1, &v[0]); }
void Shader::setVec4(const char* name, glm::vec4& v)                const noexcept { glUniform4fv(glGetUniformLocation(id, name), 1, &v[0]); }
void Shader::setMat2(const char* name, glm::mat2& m)                const noexcept { glUniformMatrix2fv(glGetUniformLocation(id, name), 1, GL_FALSE, &m[0][0]); }
void Shader::setMat3(const char* name, glm::mat3& m)                const noexcept { glUniformMatrix3fv(glGetUniformLocation(id, name), 1, GL_FALSE, &m[0][0]); }
void Shader::setMat4(const char* name, glm::mat4& m)                const noexcept { glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, &m[0][0]); }
void Shader::setMat4Array(const char* name, glm::mat4* m, unsigned int count) const noexcept { glUniformMatrix4fv(glGetUniformLocation(id, name), count, GL_FALSE, &m[0][0][0]); }

void Shader::compileErrors(unsigned int shader, const char* type) {
    int  success;
    char infoLog[1024];
    if (std::string(type) == "PROGRAM") {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR - Link : " << type << "\n" << infoLog << std::endl;
        }
    } else {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR - Compile : " << type << "\n" << infoLog << std::endl;
        }
    }
}