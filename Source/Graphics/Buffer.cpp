#include "Precompiled.hpp"
#include "Buffer.hpp"

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize a buffer! "
    
    // Invalid types.
    const GLuint InvalidHandle = 0;
}

Buffer::Buffer(GLenum type) :
    m_type(type),
    m_handle(InvalidHandle),
    m_elementSize(0),
    m_elementCount(0)
{
}

Buffer::~Buffer()
{
    Cleanup();
}

bool Buffer::Initialize(unsigned int elementSize, unsigned int elementCount, const void* data, GLenum usage)
{
    Cleanup();

    // Validate arguments.
    if(elementSize == 0)
    {
        Log() << LogInitializeError() << "Invalid argument - \"elementSize\" is 0.";
        return false;
    }

    if(elementCount == 0)
    {
        Log() << LogInitializeError() << "Invalid argument - \"elementCount\" is 0.";
        return false;
    }

    m_elementSize   = elementSize;
    m_elementCount  = elementCount;

    // Create a buffer.
    glGenBuffers(1, &m_handle);

    if(m_handle == InvalidHandle)
    {
        Log() << LogInitializeError() << "Couldn't create a buffer.";
        Cleanup();
        return false;
    }

    // Copy data to the buffer.
    unsigned int bufferSize = m_elementSize * m_elementCount;

    glBindBuffer(m_type, m_handle);
    glBufferData(m_type, bufferSize, data, usage);
    glBindBuffer(m_type, 0);

    Log() << "Created a buffer. (Size: " << bufferSize << " bytes)";

    return true;
}

void Buffer::Cleanup()
{
    if(m_handle != InvalidHandle)
    {
        glDeleteBuffers(1, &m_handle);
        m_handle = InvalidHandle;
    }

    m_elementSize = 0;
    m_elementCount = 0;
}

void Buffer::Update(const void* data, int count)
{
    if(m_handle == InvalidHandle)
        return;

    if(data == nullptr)
        return;

    if(count == 0)
        return;

    // Check if to upload the whole buffer.
    if(count < 0)
    {
        count = m_elementCount;
    }

    // Upload new buffer data.
    glBindBuffer(m_type, m_handle);
    glBufferSubData(m_type, 0, m_elementSize * count, data);
    glBindBuffer(m_type, 0);
}

bool Buffer::IsValid() const
{
    return m_handle != InvalidHandle;
}

GLenum IndexBuffer::GetElementType() const
{
    if(m_type == GL_ELEMENT_ARRAY_BUFFER)
    {
        switch(m_elementSize)
        {
            case 1: return GL_UNSIGNED_BYTE;
            case 2: return GL_UNSIGNED_SHORT;
            case 4: return GL_UNSIGNED_INT;
        }
    }

    return GL_INVALID_ENUM;
}
