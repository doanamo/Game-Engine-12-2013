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

    void SetSourceSize(const glm::vec2& size);
    void SetTargetSize(const glm::vec2& size);

    void SetTargetAspect(float aspect);

    const glm::vec2& GetSourceSize() const;
    const glm::vec2& GetTargetSize() const;

    const glm::vec4& GetRectangle() const;
    const glm::vec2& GetOffset() const;

private:
    void RebuildScreenSpace() const;

private:
    glm::vec2 m_sourceSize;
    glm::vec2 m_targetSize;

    mutable glm::vec4 m_rectangle;
    mutable glm::vec2 m_offset;
    mutable bool m_rebuild;
};
