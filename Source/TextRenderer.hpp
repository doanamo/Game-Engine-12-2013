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
	// Draw description.
	struct DrawInfo
	{
		// Default constructor.
		DrawInfo() :
			font(nullptr),
			color(0.0f, 0.0f, 0.0f, 0.0f),
			position(0.0f, 0.0f),
			size(0.0f, 0.0f),
			cursorIndex(-1),
			debug(false)
		{
		}

		// Font used.
		Font* font;

		// Color of the drawn text.
		glm::vec4 color;

		// Top left corner of the text bounding box (not base line).
		glm::vec2 position;

		// Size of the drawing area that text will be enclosed in. 
		// If a word exceeds the width, it will be wrote in a new line.
		// If a line exceeds the height, it will be cut out (possibly partially).
		// Set width or height to 0.0f to disable each.
		glm::vec2 size;

		// Text cursor index (position).
		// Set to -1 to disable.
		int cursorIndex;

		// Draws debug info.
		bool debug;
	};

public:
	TextRenderer();
	~TextRenderer();

	bool Initialize(int bufferSize);
	void Cleanup();

	void Update(float dt);
	void Draw(const DrawInfo& info, const glm::mat4& transform, const char* text);

private:
	Vertex*	m_vertexData;
	int		m_bufferSize;

	Shader			m_shader;
	VertexBuffer	m_vertexBuffer;
	IndexBuffer		m_indexBuffer;
	VertexInput     m_vertexInput;

	float m_cursorTime;

	bool m_initialized;
};
