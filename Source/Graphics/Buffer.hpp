#pragma once

#include "Precompiled.hpp"

//
// Buffer
//

class Buffer
{
public:
	Buffer(GLenum type);
	virtual ~Buffer();

	// Initializes the buffer.
	bool Initialize(unsigned int elementSize, unsigned int elementCount, const void* data);

	// Restores class instance to it's original state.
	void Cleanup();

	// Checks if the buffer is valid.
	bool IsValid() const;

	// Gets the buffer type.
	GLenum GetType() const
	{
		return m_type;
	}

	// Gets the buffer handle.
	GLuint GetHandle() const
	{
		return m_handle;
	}

	// Gets the buffer element size.
	unsigned int GetElementSize() const
	{
		return m_elementSize;
	}

	// Gets the buffer element count.
	unsigned int GetElementCount() const
	{
		return m_elementCount;
	}

	// Gets the buffer element type.
	GLenum GetElementType() const;

private:
	GLenum m_type;
	GLuint m_handle;

	unsigned int m_elementSize;
	unsigned int m_elementCount;
};

//
// Vertex Buffer
//

class VertexBuffer : public Buffer
{
public:
	VertexBuffer() :
		Buffer(GL_ARRAY_BUFFER)
	{
	}
};

//
// Index Buffer
//

class IndexBuffer : public Buffer
{
public:
	IndexBuffer() :
		Buffer(GL_ELEMENT_ARRAY_BUFFER)
	{
	}
};