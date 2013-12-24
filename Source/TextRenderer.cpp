#include "Precompiled.hpp"
#include "TextRenderer.hpp"
#include "ShapeRenderer.hpp"
#include "Font.hpp"

namespace
{
	// Log error messages.
	#define LogInitializeError() "Failed to initialize a text renderer! "

	// Index type.
	typedef unsigned int Index;
}

TextRenderer::TextRenderer() :
	m_vertexData(nullptr),
	m_bufferSize(0),
	m_shader(),
	m_vertexBuffer(),
	m_indexBuffer(),
	m_vertexInput(),
	m_initialized(false),
	m_debug(false)
{
}

TextRenderer::~TextRenderer()
{
	Cleanup();
}

bool TextRenderer::Initialize(int bufferSize)
{
	Cleanup();

	// Validate arguments.
	if(bufferSize <= 0)
	{
		Log() << LogInitializeError() << "Invalid argument - \"bufferSize\" is invalid.";
		return false;
	}

	m_bufferSize = bufferSize;

	// Allocate vertex data buffer for glyph quads.
	m_vertexData = new Vertex[m_bufferSize * 4];

	// Load text shader.
	if(!m_shader.Load(Context::workingDir + "Data/Shaders/Text.glsl"))
	{
		Log() << LogInitializeError() << "Couldn't load a shader.";
		Cleanup();
		return false;
	}

	// Create the vertex buffer.
	if(!m_vertexBuffer.Initialize(sizeof(Vertex), m_bufferSize * 4, nullptr, GL_DYNAMIC_DRAW))
	{
		Log() << LogInitializeError() << "Couldn't create a vertex buffer.";
		Cleanup();
		return false;
	}

	// Create index data for quads.
	std::vector<Index> indexData;
	indexData.resize(m_bufferSize * 6);

	for(int i = 0; i < m_bufferSize; ++i)
	{
		indexData[i * 6 + 0] = i * 4 + 0;
		indexData[i * 6 + 1] = i * 4 + 1;
		indexData[i * 6 + 2] = i * 4 + 2;

		indexData[i * 6 + 3] = i * 4 + 2;
		indexData[i * 6 + 4] = i * 4 + 3;
		indexData[i * 6 + 5] = i * 4 + 0;
	}

	// Create the index buffer.
	if(!m_indexBuffer.Initialize(sizeof(Index), indexData.size(), &indexData[0]))
	{
		Log() << LogInitializeError() << "Couldn't create an index buffer.";
		Cleanup();
		return false;
	}

	// Vertex input.
	VertexAttribute vertexAttributes[] =
	{
		{ 0, &m_vertexBuffer, VertexAttributeTypes::Float2 },
		{ 1, &m_vertexBuffer, VertexAttributeTypes::Float2 },
		{ 2, &m_vertexBuffer, VertexAttributeTypes::Float4 },
	};

	if(!m_vertexInput.Initialize(&vertexAttributes[0], StaticArraySize(vertexAttributes)))
	{
		Log() << LogInitializeError() << "Couldn't create a vertex input.";
		Cleanup();
		return false;
	}

	return m_initialized = true;
}

void TextRenderer::Cleanup()
{
	delete[] m_vertexData;
	m_vertexData = nullptr;

	m_bufferSize = 0;

	m_shader.Cleanup();
	m_vertexBuffer.Cleanup();
	m_indexBuffer.Cleanup();
	m_vertexInput.Cleanup();

	m_initialized = false;
	m_debug = false;
}

void TextRenderer::Draw(Font* font, const glm::vec2& position, float maxWidth, const glm::mat4& transform, wchar_t* text)
{
	if(!m_initialized)
		return;

	if(font == nullptr)
		return;

	if(maxWidth < 0.0f)
		return;

	if(text == nullptr)
		return;

	// Update font texture atlas.
	font->CacheGlyphs(text);
	font->UpdateAtlasTexture();

	// Bind render states.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, font->GetTexture()->GetHandle());

	glUseProgram(m_shader.GetHandle());
	glUniformMatrix4fv(m_shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(transform));
	glUniform1i(m_shader.GetUniform("texture"), 0);
	
	glBindVertexArray(m_vertexInput.GetHandle());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.GetHandle());

	// Calculate pixel size of the atlas texture.
	glm::vec2 pixelSize(1.0f / font->GetAtlasWidth(), 1.0f / font->GetAtlasHeight());

	// Current drawing position.
	glm::vec2 baselinePosition;
	baselinePosition.x = position.x;
	baselinePosition.y = position.y - font->GetAscender();

	// Debug drawing.
	glm::vec2 baselineBegin(baselinePosition);

	std::vector<ShapeRenderer::Line> debugLines;

	auto DrawDebugBaseLine = [&](const glm::vec2& baselineEnd) -> void
	{
		assert(m_debug);

		ShapeRenderer::Line line;
		line.begin = baselineBegin;
		line.end = baselineEnd;
		line.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

		debugLines.push_back(line);
	};

	// Helper methods.
	auto MoveNextLine = [&]()
	{
		// Draw debug base line.
		if(m_debug)
		{
			DrawDebugBaseLine(baselinePosition);
		}

		// Move to the next line.
		baselinePosition.x = position.x;
		baselinePosition.y -= font->GetLineSpacing();

		// Set new baseline begining.
		if(m_debug)
		{
			baselineBegin = baselinePosition;
		}
	};

	auto AdvanceBaseline = [&](const Glyph* glyph)
	{
		// Advance position for next glyph.
		baselinePosition.x += glyph->advance.x;
		baselinePosition.y += glyph->advance.y;
	};

	// Draw characters.
	size_t textLength = std::wcslen(text);
	int charactersBuffered = 0;
	bool wordProcessed = true;

	for(size_t i = 0; i < textLength; ++i)
	{
		FT_ULong character = text[i];

		// Check if it's one of the special characters.
		if(character == '\n')
		{
			// Move to the next line.
			MoveNextLine();
			continue;
		}
		else
		if(character == ' ')
		{
			// Get glyph description.
			const Glyph* glyph = font->GetGlyph(' ');
			assert(glyph != nullptr);

			// Advance drawing position.
			AdvanceBaseline(glyph);

			wordProcessed = false;
			continue;
		}

		// Check if a word will fit in the current line.
		if(wordProcessed == false)
		{
			float wordSize = 0.0f;

			for(size_t j = i; j < textLength; ++j)
			{
				FT_ULong wordCharacter = text[j];

				if(wordCharacter == ' ')
					break;

				// Get glyph description.
				const Glyph* glyph = font->GetGlyph(wordCharacter);
				assert(glyph != nullptr);

				// Get glyph kerning.
				assert(i != 0);

				int kerning = font->GetKerning(text[j - i], wordCharacter);

				// Check if the word will fit.
				wordSize += glyph->advance.x + kerning;

				if(baselinePosition.x - position.x + wordSize > maxWidth)
				{
					// Don't draw last space for debug base line.
					if(m_debug)
					{
						const Glyph* glyphSpace = font->GetGlyph(' ');
						assert(glyphSpace != nullptr);

						baselinePosition.x -= glyphSpace->advance.x;
					}

					// Move to the next line.
					MoveNextLine();

					break;
				}
			}

			wordProcessed = true;
		}

		// Get glyph description.
		const Glyph* glyph = font->GetGlyph(character);

		assert(glyph != nullptr);

		// Apply glyph kerning.
		if(i != 0)
		{
			FT_ULong previous = text[i - 1];

			if(previous != '\n' && previous != ' ')
			{
				int kerning = font->GetKerning(previous, character);
				baselinePosition.x += kerning;
			}
		}

		// Create a character quad.
		glm::vec4 rectangle;
		rectangle.x = (float)(baselinePosition.x + glyph->offset.x);
		rectangle.y = (float)(baselinePosition.y + glyph->offset.y);
		rectangle.w = rectangle.x + (float)glyph->size.x;
		rectangle.z = rectangle.y + (float)glyph->size.y;

		glm::vec4 texture;
		texture.x = glyph->position.x * pixelSize.x;
		texture.y = glyph->position.y * pixelSize.y;
		texture.w = (glyph->position.x + glyph->size.x) * pixelSize.x;
		texture.z = (glyph->position.y + glyph->size.y) * pixelSize.y;

		Vertex quad[4] =
		{
			{ glm::vec2(rectangle.x, rectangle.y), glm::vec2(texture.x, texture.y), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ glm::vec2(rectangle.w, rectangle.y), glm::vec2(texture.w, texture.y), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ glm::vec2(rectangle.w, rectangle.z), glm::vec2(texture.w, texture.z), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ glm::vec2(rectangle.x, rectangle.z), glm::vec2(texture.x, texture.z), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) },
		};

		// Copy character quad to the vertex data.
		memcpy(&m_vertexData[charactersBuffered * 4], &quad[0], sizeof(Vertex) * 4);

		++charactersBuffered;

		// Draw if we reached the buffer size or the last character.
		if(charactersBuffered == m_bufferSize || i == textLength - 1)
		{
			// Update the vertex buffer.
			m_vertexBuffer.Update(&m_vertexData[0]);

			// Draw character quads.
			glDrawElements(GL_TRIANGLES, charactersBuffered * 6, m_indexBuffer.GetElementType(), (void*)0);

			// Reset the counter.
			charactersBuffered = 0;
		}

		// Advance drawing position.
		AdvanceBaseline(glyph);
	}

	// Unbind render states.
	glBindVertexArray(0);
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);

	// Flush debug draw.
	if(m_debug)
	{
		// Draw last baseline.
		DrawDebugBaseLine(baselinePosition);

		// Draw all base lines.
		Context::shapeRenderer->DrawLines(&debugLines[0], debugLines.size(), transform);

		// Draw bounding box.
		ShapeRenderer::Rectangle rectangle;
		rectangle.position.x = position.x;
		rectangle.position.y = baselinePosition.y + font->GetDescender();
		rectangle.size.x = maxWidth;
		rectangle.size.y = position.y - rectangle.position.y;
		rectangle.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

		Context::shapeRenderer->DrawRectangles(&rectangle, 1, transform);
	}
}

void TextRenderer::SetDebug(bool enabled)
{
	m_debug = enabled;
}
