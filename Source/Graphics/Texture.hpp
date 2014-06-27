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

    // Updates the texture with new data.
    void Update(const void* data);

    // Return the texture handle.
    GLuint GetHandle() const
    {
        return m_handle;
    }

    // Returns the texture width.
    int GetWidth() const
    {
        return m_width;
    }

    // Returns the texture height.
    int GetHeight() const
    {
        return m_height;
    }

    // Checks if instance is valid.
    bool IsValid() const;

private:
    GLuint m_handle;
    GLenum m_format;

    int m_width;
    int m_height;
};
