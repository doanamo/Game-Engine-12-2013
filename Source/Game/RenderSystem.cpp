#include "Precompiled.hpp"
#include "RenderSystem.hpp"

#include "Graphics/Texture.hpp"

#include "TransformComponent.hpp"
#include "RenderComponent.hpp"

#include "MainContext.hpp"
#include "GameContext.hpp"
#include "EntitySystem.hpp"

RenderSystem::RenderSystem() :
    m_bufferSize(0),
    m_initialized(false)
{
}

RenderSystem::~RenderSystem()
{
    Cleanup();
}

void RenderSystem::Cleanup()
{
    ClearContainer(m_sprites);

    m_shader.Cleanup();
    m_vertexBuffer.Cleanup();
    m_instanceBuffer.Cleanup();
    m_vertexInput.Cleanup();

    m_bufferSize = 0;

    m_initialized = false;
}

bool RenderSystem::Initialize(int bufferSize)
{
    Cleanup();

    // Validate arguments.
    if(bufferSize <= 0)
    {
        Cleanup();
        return false;
    }

    m_bufferSize = bufferSize;

    // Load shader.
    if(!m_shader.Load(Main::WorkingDir() + "Data/Shaders/Sprite.glsl"))
    {
        Cleanup();
        return false;
    }

    // Vertex structure.
    struct Vertex
    {
        glm::vec2 position;
        glm::vec2 texture;
    };

    // Create a vertex buffer.
    Vertex vertices[4] =
    {
        { glm::vec2(-0.5f, -0.5f), glm::vec2(0.0f, 0.0f) },
        { glm::vec2( 0.5f, -0.5f), glm::vec2(1.0f, 0.0f) },
        { glm::vec2( 0.5f,  0.5f), glm::vec2(1.0f, 1.0f) },
        { glm::vec2(-0.5f,  0.5f), glm::vec2(0.0f, 1.0f) },
    };

    if(!m_vertexBuffer.Initialize(sizeof(Vertex), StaticArraySize(vertices), &vertices, GL_STATIC_DRAW))
    {
        Cleanup();
        return false;
    }

    // Create an instance buffer.
    if(!m_instanceBuffer.Initialize(sizeof(Sprite), m_bufferSize, nullptr, GL_DYNAMIC_DRAW))
    {
        Cleanup();
        return false;
    }

    // Create a vertex input.s
    VertexAttribute vertexAttributes[] =
    {
        { &m_vertexBuffer,   VertexAttributeTypes::Float2   }, // Position
        { &m_vertexBuffer,   VertexAttributeTypes::Float2   }, // Texture

        { &m_instanceBuffer, VertexAttributeTypes::Float4x4 }, // Transform
        { &m_instanceBuffer, VertexAttributeTypes::Float4   }, // Diffuse Color
        { &m_instanceBuffer, VertexAttributeTypes::Float3   }, // Emission Color
        { &m_instanceBuffer, VertexAttributeTypes::Float1   }, // Emission Power
    };

    if(!m_vertexInput.Initialize(&vertexAttributes[0], StaticArraySize(vertexAttributes)))
    {
        Cleanup();
        return false;
    }

    // Success!
    m_initialized = true;

    return true;
}

void RenderSystem::Update()
{
    if(!m_initialized)
        return;

    // Get current window size.
    int windowWidth = Console::windowWidth;
    int windowHeight = Console::windowHeight;

    // Setup projection.
    m_projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);

    // Make sure the sprite list is clear.
    m_sprites.clear();

    // Process render components.
    for(auto it = Game::RenderComponents().Begin(); it != Game::RenderComponents().End(); ++it)
    {
        // Check if entity is active.
        if(!Game::EntitySystem().IsHandleValid(it->first))
            continue;

        // Get the render component.
        RenderComponent& render = it->second;
    
        // Get other components.
        TransformComponent* transform = Game::TransformComponents().Lookup(it->first);

        if(transform == nullptr)
            continue;

        // Add a sprite to the list.
        Sprite sprite;
        sprite.transform = transform->CalculateMatrix();
        sprite.diffuseColor = render.GetColor();
        sprite.emissionColor = glm::vec3(1.0f, 1.0f, 1.0f);
        sprite.emissionPower = 0.0f;
        m_sprites.push_back(sprite);
    }
}

void RenderSystem::Draw()
{
    if(!m_initialized)
        return;

    if(m_sprites.empty())
        return;

    // Bind render states.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(m_shader.GetHandle());
    glBindVertexArray(m_vertexInput.GetHandle());

    glUniformMatrix4fv(m_shader.GetUniform("viewTransform"), 1, GL_FALSE, glm::value_ptr(m_projection));
    glUniform1i(m_shader.GetUniform("texture"), 0);

    // Batch sprites.
    uint32_t currentBatchIndex = 0;
    uint32_t instancesBatched = 0;

    for(uint32_t i = 0; i < m_sprites.size(); ++i)
    {
        instancesBatched++;

        // Determine if we have to draw the batch.
        bool drawBatch = false;

        if(instancesBatched == m_bufferSize)
        {
            // We reached the buffer size.
            drawBatch = true;
        }
        else
        if(i == m_sprites.size() - 1)
        {
            // We reached the last element.
            drawBatch = true;
        }

        // Draw the batch if needed.
        if(drawBatch)
        {
            // Upload instance data.
            m_instanceBuffer.Update(&m_sprites[currentBatchIndex], instancesBatched);

            // Bind a texture.
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Main::TextureBlank().GetHandle());

            // Draw instanced quads.
            glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, m_vertexBuffer.GetElementCount(), instancesBatched);

            // Set the new batch index.
            currentBatchIndex += instancesBatched;

            // Reset batch counter.
            instancesBatched = 0;
        }
    }

    // Unbind render states.
    glBindVertexArray(0);
    glUseProgram(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);

    // Clear the shape list.
    m_sprites.clear();
}
