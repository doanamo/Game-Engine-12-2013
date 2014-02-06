#include "Precompiled.hpp"
#include "CollisionComponent.hpp"

CollisionComponent::CollisionComponent() :
    m_boundingBox(0.0f),
    m_enabled(true)
{
}

CollisionComponent::~CollisionComponent()
{
}
