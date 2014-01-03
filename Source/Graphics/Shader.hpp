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

    GLuint GetHandle() const
    {
        return m_handle;
    }

    GLint GetAttribute(std::string name) const;
    GLint GetUniform(std::string name) const;

    bool IsValid() const;

private:
    GLuint m_handle;
};
