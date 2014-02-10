#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"

//
// Collision Component
//

class CollisionComponent : public Component
{
public:
    // Type declarations.
    typedef unsigned int BitField;

public:
    CollisionComponent();
    ~CollisionComponent();

    void SetBoundingBox(const glm::vec4& boundingBox)
    {
        m_boundingBox = boundingBox;
    }

    const glm::vec4& GetBoundingBox() const
    {
        return m_boundingBox;
    }

    void SetType(BitField type)
    {
        m_type = type;
    }

    BitField GetType() const
    {
        return m_type;
    }

    void SetMask(BitField mask)
    {
        m_mask = mask;
    }

    BitField GetMask() const
    {
        return m_mask;
    }

    void Enable()
    {
        m_enabled = true;
    }

    void Disable()
    {
        m_enabled = false;
    }

    bool IsEnabled() const
    {
        return m_enabled;
    }

private:
    // Collision bounding box.
    glm::vec4 m_boundingBox;

    // Collision bits.
    BitField m_type;
    BitField m_mask;
    
    // Collision enabled.
    bool m_enabled;
};
