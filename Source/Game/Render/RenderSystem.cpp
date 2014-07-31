#include "Precompiled.hpp"
#include "RenderSystem.hpp"

#include "MainContext.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/CoreRenderer.hpp"
#include "Game/GameContext.hpp"
#include "Game/GameState.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Component/ComponentSystem.hpp"
#include "Game/Transform/TransformComponent.hpp"
#include "Game/Render/RenderComponent.hpp"

namespace
{
    // Game space size.
    float gameWidth = 1024;
    float gameHeight = 576;
}

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
    m_shader.Cleanup();
    m_vertexBuffer.Cleanup();
    m_instanceBuffer.Cleanup();
    m_vertexInput.Cleanup();

    m_bufferSize = 0;

    m_screenSpace.Cleanup();

    ClearContainer(m_sprites);

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
    if(!m_shader.Load("Data/Shaders/Sprite.glsl"))
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

    //
    // Setup Screen Space
    //

    // Get window size.
    float windowWidth = Console::windowWidth;
    float windowHeight = Console::windowHeight;

    // Setup screen space.
    m_screenSpace.SetSourceSize(windowWidth, windowHeight);
    m_screenSpace.SetTargetSize(gameWidth, gameHeight);

    //
    // Process Components
    //

    // Make sure the sprite list is clear.
    m_sprites.clear();

    // Process render components.
    for(auto it = GameState::GetComponentSystem().Begin<RenderComponent>(); it != GameState::GetComponentSystem().End<RenderComponent>(); ++it)
    {
        // Check if entity is active.
        if(!GameState::GetEntitySystem().IsHandleValid(it->first))
            continue;

        // Get the render component.
        RenderComponent& render = it->second;
    
        // Get other components.
        TransformComponent* transform = GameState::GetComponentSystem().Lookup<TransformComponent>(it->first);

        if(transform == nullptr)
            continue;

        // Add a sprite to the list.
        Sprite sprite;
        sprite.transform = transform->CalculateMatrix();
        sprite.diffuseColor = render.GetDiffuseColor();
        sprite.emissionColor = render.GetEmissionColor();
        sprite.emissionPower = render.GetEmissionPower();
        m_sprites.push_back(sprite);
    }
}

void RenderSystem::Draw()
{
    if(!m_initialized)
        return;

    //
    // Prepare Drawing
    //

    // Clear the depth.
    Main::GetCoreRenderer().SetClearDepth(1.0f);
    Main::GetCoreRenderer().Clear(ClearFlags::Depth);

    // Clear the screen.
    Main::GetCoreRenderer().SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Main::GetCoreRenderer().Clear(ClearFlags::Color);

    //
    // Draw Sprites
    //

    // Don't draw if there's no sprites.
    if(m_sprites.empty())
        return;

    // Bind render states.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(m_shader.GetHandle());
    glBindVertexArray(m_vertexInput.GetHandle());

    glUniformMatrix4fv(m_shader.GetUniform("viewTransform"), 1, GL_FALSE, glm::value_ptr(m_screenSpace.GetTransform()));
    glUniform1i(m_shader.GetUniform("texture"), 0);

    auto ResetState = MakeScopeGuard([&]()
    {
        // Unbind render states.
        glDisable(GL_BLEND);

        glUseProgram(0);
        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);
    });

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
            glBindTexture(GL_TEXTURE_2D, Main::GetBlankTexture().GetHandle());

            // Draw instanced quads.
            glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, m_vertexBuffer.GetElementCount(), instancesBatched);

            // Set the new batch index.
            currentBatchIndex += instancesBatched;

            // Reset batch counter.
            instancesBatched = 0;
        }
    }

    // Clear the shape list.
    m_sprites.clear();
}
