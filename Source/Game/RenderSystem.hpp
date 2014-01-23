#pragma once

#include "Precompiled.hpp"
#include "Graphics/ShapeRenderer.hpp"

// Forward declarations.
class Entity;

//
// Render System
//

class RenderSystem
{
public:
    RenderSystem();
    ~RenderSystem();

    bool Initialize();
    void Cleanup();

    void Update();
    void Draw();

private:
    // Render shapes.
    std::vector<ShapeRenderer::Quad> m_shapes;

    // Render state.
    glm::mat4 m_projection;
};
