#pragma once

#include "Precompiled.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexInput.hpp"
#include "Graphics/ScreenSpace.hpp"

// Forward declarations.
class EntitySystem;
class ComponentSystem;

//
// Render System
//

class RenderSystem
{
public:
    // Sprite structure.
    struct Sprite
    {
        glm::mat4 transform;
        glm::vec4 diffuseColor;
        glm::vec3 emissionColor;
        float     emissionPower;
    };

public:
    RenderSystem();
    ~RenderSystem();

    bool Initialize(EntitySystem* entitySystem, ComponentSystem* componentSystem, int bufferSize);
    void Cleanup();

    void Update();
    void Draw();

    const ScreenSpace& GetScreenSpace() const
    {
        return m_screenSpace;
    }

private:
    // System state.
    bool m_initialized;
    int m_bufferSize;

    // Game systems.
    EntitySystem*    m_entitySystem;
    ComponentSystem* m_componentSystem;

    // Render objects
    Shader         m_shader;
    VertexBuffer   m_vertexBuffer;
    InstanceBuffer m_instanceBuffer;
    VertexInput    m_vertexInput;

    // Screen space.
    ScreenSpace m_screenSpace;

    // Processed render components.
    std::vector<Sprite> m_sprites;
};
