#pragma once

#include "Precompiled.hpp"

#include "Graphics/Shader.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexInput.hpp"

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

    bool Initialize(int bufferSize);
    void Cleanup();

    void Update();
    void Draw();

    const glm::mat4& GetProjection() const
    {
        return m_projection;
    }

    const glm::mat4& GetView() const
    {
        return m_view;
    }

    const glm::mat4 GetTransform() const
    {
        return m_transform;
    }

private:
    // Render objects
    Shader         m_shader;
    VertexBuffer   m_vertexBuffer;
    InstanceBuffer m_instanceBuffer;
    VertexInput    m_vertexInput;

    int m_bufferSize;

    // Render transforms.
    glm::mat4 m_projection;
    glm::mat4 m_view;
    glm::mat4 m_transform;

    // Processed render components.
    std::vector<Sprite> m_sprites;

    bool m_initialized;
};
