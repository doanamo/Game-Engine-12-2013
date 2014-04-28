#pragma once

#include "Precompiled.hpp"

//
// Screen Space
//

class ScreenSpace
{
public:
    ScreenSpace();
    ~ScreenSpace();

    void Cleanup();

    void SetSourceSize(float width, float height);
    void SetTargetSize(float width, float height);

    void SetTargetAspect(float aspect);

    const glm::vec2& GetSourceSize() const;
    const glm::vec2& GetTargetSize() const;

    const glm::vec4& GetRectangle() const;
    const glm::vec2& GetOffset() const;

    const glm::mat4& GetProjection() const;
    const glm::mat4& GetView() const;
    const glm::mat4& GetTransform() const;

private:
    void Rebuild() const;
    mutable bool m_rebuild;

private:
    glm::vec2 m_sourceSize;
    glm::vec2 m_targetSize;

    mutable glm::vec4 m_rectangle;
    mutable glm::vec2 m_offset;

    mutable glm::mat4 m_projection;
    mutable glm::mat4 m_view;
    mutable glm::mat4 m_transform;
};
