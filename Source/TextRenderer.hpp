#pragma once

#include "Precompiled.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexInput.hpp"

// Forward declarations.
class Font;

//
// Text Renderer
//

class TextRenderer
{
private:
	// Vertex structure.
	struct Vertex
	{
		glm::vec2 position;
		glm::vec2 texture;
		glm::vec4 color;
	};

public:
	TextRenderer();
	~TextRenderer();

	bool Initialize(int bufferSize);
	void Cleanup();

	void Draw(Font* font, const glm::vec2& position, float maxWidth, const glm::mat4& transform, const wchar_t* text);

	void SetDebug(bool enabled);

private:
	Vertex*	m_vertexData;
	int		m_bufferSize;

	Shader			m_shader;
	VertexBuffer	m_vertexBuffer;
	IndexBuffer		m_indexBuffer;
	VertexInput     m_vertexInput;

	bool m_initialized;
	bool m_debug;
};
