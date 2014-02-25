#pragma once

#include "Precompiled.hpp"

//
// Vertex Input
//

// Forward declarations.
class Buffer;

// Vertex attribute types.
enum class VertexAttributeTypes
{
    Invalid,

    Float1,
    Float2,
    Float3,
    Float4,

    Float4x4,

    Count,
};

// Vertex attribute structure.
struct VertexAttribute
{
    Buffer* buffer;
    VertexAttributeTypes type;
};

// Vertex input class.
class VertexInput
{
public:
    VertexInput();
    ~VertexInput();

    // Initializes the vertex input.
    bool Initialize(VertexAttribute* attributes, int count);

    // Restores class instance to it's original state.
    void Cleanup();

    // Gets the vertex array object handle.
    GLuint GetHandle() const
    {
        return m_handle;
    }

private:
    // Vertex array object.
    GLuint m_handle;
};
