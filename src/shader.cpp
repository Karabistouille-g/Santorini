/*

    SOURCE : www.learnopengl.com

*/

#include <shaders/shader.hpp>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader( const char* vertexPath, const char* fragmentPath, const char* geometryPath )
{
    // 1. retrieve the vertex/fragment/geometry source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream, gShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();		
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        if( geometryPath != nullptr )
        {
            gShaderFile.open(geometryPath);
            gShaderStream << fShaderFile.rdbuf();
            geometryCode = gShaderStream.str();
            gShaderFile.close(); 
        }
    }
    catch (std::ifstream::failure& e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char * fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment, geometry;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    compileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    compileErrors(fragment, "FRAGMENT");
    // geometry Shader
    if( geometryPath != nullptr )
    {
        const char * gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        compileErrors(geometry, "GEOMETRY");
    }
    // shader Program
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    if( geometryPath != nullptr )
        glAttachShader(id, geometry);
    glLinkProgram(id);
    compileErrors(id, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader( vertex );
    glDeleteShader( fragment );
    if( geometryPath != nullptr )
        glDeleteShader( geometry );
}

void Shader::activate() const noexcept
{
    glUseProgram( id );
}

void Shader::setFloat( const char * name, float value ) const noexcept
{
    glUniform1f( glGetUniformLocation( id, name ), value );
}

void Shader::setInt( const char * name, int value ) const noexcept
{
    glUniform1i( glGetUniformLocation( id, name ), value );
}

void Shader::setBool( const char * name, bool value ) const noexcept
{
    glUniform1i( glGetUniformLocation( id, name ), (int)value );
}

void Shader::setIntArray( const char * name, int * value, unsigned int count ) const noexcept
{
    glUniform1iv( glGetUniformLocation( id, name ), count, value );
}

void Shader::setVec2( const char * name, float x, float y ) const noexcept
{
    glUniform2f( glGetUniformLocation( id, name ), x, y );
}

void Shader::setVec3( const char * name, float x, float y, float z ) const noexcept
{
    glUniform3f( glGetUniformLocation( id, name ), x, y, z );
}

void Shader::setVec4( const char * name, float x, float y, float z, float w ) const noexcept
{
    glUniform4f( glGetUniformLocation( id, name ), x, y, z, w );
}

void Shader::setVec2( const char * name, glm::vec2 & v ) const noexcept
{
    glUniform2fv( glGetUniformLocation( id, name ), 1, &v[0] );
}

void Shader::setVec3( const char * name, glm::vec3 & v ) const noexcept
{
    glUniform3fv( glGetUniformLocation( id, name ), 1, &v[0] );
}

void Shader::setVec4( const char * name, glm::vec4 & v ) const noexcept
{
    glUniform4fv( glGetUniformLocation( id, name ), 1, &v[0] );
}

void Shader::setMat2( const char * name, glm::mat2 & m ) const noexcept
{
    glUniformMatrix2fv( glGetUniformLocation( id, name ), 1, GL_FALSE, &m[0][0] );
}

void Shader::setMat3( const char * name, glm::mat3 & m ) const noexcept
{
    glUniformMatrix3fv( glGetUniformLocation( id, name ), 1, GL_FALSE, &m[0][0] );
}

void Shader::setMat4( const char * name, glm::mat4 & m ) const noexcept
{
    glUniformMatrix4fv( glGetUniformLocation( id, name ), 1, GL_FALSE, &m[0][0] );
}

void Shader::setMat4Array( const char * name, glm::mat4 * m, unsigned int count ) const noexcept
{
    glUniformMatrix4fv( glGetUniformLocation( id, name ), count, GL_FALSE, &m[0][0][0] );
}

void Shader::compileErrors( unsigned int shader, const char * type )
{
    int success;
    char infoLog[1024];
    if( type == "PROGRAM" )
    {
        glGetProgramiv( shader, GL_LINK_STATUS, &success );
        if( !success )
        {
            glGetProgramInfoLog( shader, 1024, nullptr, infoLog );
            std::cerr << "ERROR - Could not link program : " << type << "\n" << infoLog << std::endl;
        }
    }
    else
    {
        glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
        if( !success )
        {
            glGetShaderInfoLog( shader, 1024, nullptr, infoLog );
            std::cerr << "ERROR - Could not compile shader : " << type << "\n" << infoLog << std::endl;
        }
    }
}