#include "Precompiled.hpp"
#include "RenderSystem.hpp"
#include "Transform.hpp"
#include "Render.hpp"
#include "Entity.hpp"

#include "MainContext.hpp"

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

void RenderSystem::PrepareProcessing()
{
    // Get current window size.
    int windowWidth = Console::windowWidth;
    int windowHeight = Console::windowHeight;

    // Setup projection.
    m_projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);
}

void RenderSystem::Process(Entity* entity)
{
    // Check if entity contains needed components.
    Transform* transform = entity->GetComponent<Transform>();
    if(transform == nullptr) return;

    Render* render = entity->GetComponent<Render>();
    if(render == nullptr) return;

    // Add a shape to render.
    ShapeRenderer::Quad shape;
    shape.color = render->GetColor();
    shape.size = glm::vec2(50.0f, 50.0f);
    shape.position = transform->GetPosition();

    m_shapes.push_back(shape);
}

void RenderSystem::FinishProcessing()
{
}

void RenderSystem::Draw()
{
    // Draw shapes.
    Main::ShapeRenderer().DrawQuads(&m_shapes[0], m_shapes.size(), m_projection);

    // Clear the shape list.
    m_shapes.clear();
}
