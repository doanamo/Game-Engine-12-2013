#pragma once

#include "Precompiled.hpp"

//
// Shader
//

class Shader
{
public:
    Shader();
    ~Shader();

    bool Load(std::string filename);
    void Cleanup();

    // Gets the shader handle.
    GLuint GetHandle() const
    {
        return m_handle;
    }

    // Gets a shader attribute index.
    GLint GetAttribute(std::string name) const;

    // Gets a shader uniform index.
    GLint GetUniform(std::string name) const;

    // Checks if instance is valid.
    bool IsValid() const;

private:
    GLuint m_handle;
};
