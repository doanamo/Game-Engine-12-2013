#include "Precompiled.hpp"
#include "RenderSystem.hpp"

#include "TransformComponent.hpp"
#include "RenderComponent.hpp"

#include "MainContext.hpp"
#include "GameContext.hpp"
#include "EntitySystem.hpp"

RenderSystem::RenderSystem()
{
}

RenderSystem::~RenderSystem()
{
    Cleanup();
}

void RenderSystem::Cleanup()
{
    ClearContainer(m_shapes);
}

bool RenderSystem::Initialize()
{
    Cleanup();

    return true;
}

void RenderSystem::Update()
{
    // Get current window size.
    int windowWidth = Console::windowWidth;
    int windowHeight = Console::windowHeight;

    // Setup projection.
    m_projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);

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

        // Add a shape to render.
        ShapeRenderer::Quad shape;
        shape.color = render.GetColor();
        shape.size = glm::vec2(50.0f, 50.0f);
        shape.position = transform->GetPosition();

        m_shapes.push_back(shape);
    }
}

void RenderSystem::Draw()
{
    if(m_shapes.empty())
        return;

    // Draw shapes.
    Main::ShapeRenderer().DrawQuads(&m_shapes[0], m_shapes.size(), m_projection);

    // Clear the shape list.
    m_shapes.clear();
}
