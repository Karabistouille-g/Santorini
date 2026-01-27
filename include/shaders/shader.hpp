#ifndef SHADER_HPP
#define SHADER_HPP

#include <glm/glm.hpp>
#include <string>

class Shader
{
    public :
        Shader( const char * vertexPath, const char * fragmentPath, const char * geometryPath = nullptr );

        void activate() const noexcept;

        void setFloat( const char * name, float value ) const noexcept;
        void setInt( const char * name, int value ) const noexcept;
        void setBool( const char * name, bool value ) const noexcept;
        void setIntArray( const char * name, int * value, unsigned int count ) const noexcept;

        void setVec2( const char * name, float x, float y ) const noexcept;
        void setVec3( const char * name, float x, float y, float z ) const noexcept;
        void setVec4( const char * name, float x, float y, float z, float w ) const noexcept;

        void setVec2( const char * name, glm::vec2 & v ) const noexcept;
        void setVec3( const char * name, glm::vec3 & v ) const noexcept;
        void setVec4( const char * name, glm::vec4 & v ) const noexcept;

        void setMat2( const char * name, glm::mat2 & m ) const noexcept;
        void setMat3( const char * name, glm::mat3 & m ) const noexcept;
        void setMat4( const char * name, glm::mat4 & m ) const noexcept;
        void setMat4Array( const char * name, glm::mat4 * m , unsigned int count ) const noexcept;

    private :
        unsigned int id;

        void compileErrors( unsigned int shader, const char * type );
};

#endif