#include "Precompiled.hpp"
#include "RenderSystem.hpp"

#include "Graphics/Texture.hpp"
#include "Graphics/CoreRenderer.hpp"

#include "TransformComponent.hpp"
#include "RenderComponent.hpp"

#include "MainContext.hpp"
#include "GameContext.hpp"
#include "EntitySystem.hpp"

namespace
{
    // Game space size.
    float gameWidth = 1024;
    float gameHeight = 576;
}

RenderSystem::RenderSystem() :
    m_bufferSize(0),
    m_projection(1.0f),
    m_view(1.0f),
    m_transform(1.0f),
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

    m_projection = glm::mat4(1.0f);
    m_view = glm::mat4(1.0f);
    m_transform = glm::mat4(1.0f);

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

    //
    // Setup View
    //

    // Get window size.
    float windowWidth = Console::windowWidth;
    float windowHeight = Console::windowHeight;

    // Calculate apsect ratios.
    float windowAspectRatio = windowWidth / windowHeight;
    float gameAspectRatio = gameWidth / gameHeight;

    float aspectRatio = windowAspectRatio / gameAspectRatio;

    // Setup screen space coordinates.
    glm::vec4 screenSpace(-gameWidth * 0.5f, gameWidth * 0.5f, -gameHeight * 0.5f, gameHeight * 0.5f);

    if(windowAspectRatio > gameAspectRatio)
    {
        // Scale screen space coordinates.
        screenSpace.x *= aspectRatio;
        screenSpace.y *= aspectRatio;
    }
    else
    {
        // Scale screen space coordinates.
        screenSpace.z /= aspectRatio;
        screenSpace.w /= aspectRatio;
    }

    // Setup matrices.
    m_projection = glm::ortho(screenSpace.x, screenSpace.y, screenSpace.z, screenSpace.w);
    m_view = glm::translate(glm::mat4(1.0f), glm::vec3(-gameWidth * 0.5f, -gameHeight * 0.5f, 0.0f));
    m_transform = m_projection * m_view;

    //
    // Process Components
    //

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
    Main::CoreRenderer().SetClearDepth(1.0f);
    Main::CoreRenderer().Clear(ClearFlags::Depth);

    // Clear the screen.
    Main::CoreRenderer().SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    Main::CoreRenderer().Clear(ClearFlags::Color);

    // Get window size.
    float windowWidth = Console::windowWidth;
    float windowHeight = Console::windowHeight;

    // Calculate scissor area.
    glm::vec4 viewport(0.0f, 0.0f, windowWidth, windowHeight);

    glm::vec3 position = glm::project(glm::vec3(0.0f, 0.0f, 0.0f), m_view, m_projection, viewport);
    glm::vec3 size = glm::project(glm::vec3(gameWidth, gameHeight, 0.0f), m_view, m_projection, viewport) - position;

    glScissor((int)position.x, (int)position.y, (int)size.x, (int)size.y);

    // Toggle scissor test.
    glEnable(GL_SCISSOR_TEST);
    
    SCOPE_GUARD(glDisable(GL_SCISSOR_TEST));

    // Clear the screen again.
    Main::CoreRenderer().SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Main::CoreRenderer().Clear(ClearFlags::Color);

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

    glUniformMatrix4fv(m_shader.GetUniform("viewTransform"), 1, GL_FALSE, glm::value_ptr(m_transform));
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
            glBindTexture(GL_TEXTURE_2D, Main::BlankTexture().GetHandle());

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
