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

private:
    // Render system.
    std::vector<Sprite> m_sprites;

    Shader         m_shader;
    VertexBuffer   m_vertexBuffer;
    InstanceBuffer m_instanceBuffer;
    VertexInput    m_vertexInput;

    int m_bufferSize;

    // Render state.
    glm::mat4 m_projection;

    bool m_initialized;
};
