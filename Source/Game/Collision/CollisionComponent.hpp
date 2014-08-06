#pragma once

#include "Precompiled.hpp"

#include "Scripting/LuaMath.hpp"
#include "Game/Component/Component.hpp"

//
// Collision Flags
//

struct CollisionFlags
{
    enum Type
    {
        None = 0,

        // This object will generate collision events.
        Enabled = 1 << 0,

        // Collision events will be generated when object doesn't collide instead.
        Reversed = 1 << 1,
    };

    static const uint32_t Default = Enabled; 
};

//
// Collision Component
//

class CollisionComponent : public Component
{
public:
    // Type declarations.
    typedef uint32_t BitField;

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

    glm::vec4 GetBoundingBoxCopy() const
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

    void SetFlags(BitField flags)
    {
        m_flags = flags;
    }

    BitField GetFlags() const
    {
        return m_flags;
    }

    void Enable()
    {
        m_flags |= CollisionFlags::Enabled;
    }

    void Disable()
    {
        m_flags &= ~CollisionFlags::Enabled;
    }

    bool IsEnabled() const
    {
        return m_flags & CollisionFlags::Enabled;
    }

private:
    // Collision bounding box.
    glm::vec4 m_boundingBox;

    // Collision bits.
    BitField m_type;
    BitField m_mask;

    // Collision flags.
    BitField m_flags;
};
