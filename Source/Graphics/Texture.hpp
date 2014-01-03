#pragma once

#include "Precompiled.hpp"

//
// Texture
//

class Texture
{
public:
    Texture();
    ~Texture();

    bool Initialize(int width, int height, GLenum format, const void* data);
    void Cleanup();

    void Update(const void* data);

    GLuint GetHandle() const
    {
        return m_handle;
    }

    int GetWidth() const
    {
        return m_width;
    }

    int GetHeight() const
    {
        return m_height;
    }

private:
    GLuint m_handle;

    int m_width;
    int m_height;

    GLenum m_format;
};
