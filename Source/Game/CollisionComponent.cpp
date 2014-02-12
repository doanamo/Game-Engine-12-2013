#include "Precompiled.hpp"
#include "CollisionComponent.hpp"

CollisionComponent::CollisionComponent() :
    m_boundingBox(0.0f),
    m_type(0),
    m_mask(0),
    m_flags(CollisionFlags::Default)
{
}

CollisionComponent::~CollisionComponent()
{
}
