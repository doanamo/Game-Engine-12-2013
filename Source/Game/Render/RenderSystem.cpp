#include "Precompiled.hpp"
#include "RenderSystem.hpp"

#include "MainContext.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/CoreRenderer.hpp"
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
    m_initialized(false),
    m_bufferSize(0),
    m_entitySystem(nullptr),
    m_componentSystem(nullptr)
{
}

RenderSystem::~RenderSystem()
{
    Cleanup();
}

void RenderSystem::Cleanup()
{
    m_initialized = false;
    m_bufferSize = 0;

    m_entitySystem = nullptr;
    m_componentSystem = nullptr;

    m_shader.Cleanup();
    m_vertexBuffer.Cleanup();
    m_instanceBuffer.Cleanup();
    m_vertexInput.Cleanup();

    m_screenSpace.Cleanup();

    // Processed render components.
    ClearContainer(m_sprites);
}

bool RenderSystem::Initialize(EntitySystem* entitySystem, ComponentSystem* componentSystem, int bufferSize)
{
    Cleanup();

    // Validate arguments.
    if(entitySystem == nullptr)
        return false;

    if(componentSystem == nullptr)
        return false;

    if(bufferSize <= 0)
        return false;

    m_entitySystem = entitySystem;
    m_componentSystem = componentSystem;
    m_bufferSize = bufferSize;

    // Declare required components.
    m_componentSystem->Declare<TransformComponent>();
    m_componentSystem->Declare<RenderComponent>();

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
    return m_initialized = true;
}

void RenderSystem::Update()
{
    assert(m_initialized);

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
    auto componentsBegin = m_componentSystem->Begin<RenderComponent>();
    auto componentsEnd = m_componentSystem->End<RenderComponent>();

    for(auto it = componentsBegin; it != componentsEnd; ++it)
    {
        // Check if entity is active.
        if(!m_entitySystem->IsHandleValid(it->first))
            continue;

        // Get the render component.
        RenderComponent& render = it->second;
    
        // Get other components.
        TransformComponent* transform = m_componentSystem->Lookup<TransformComponent>(it->first);

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
    assert(m_initialized);

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
