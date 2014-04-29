#pragma once

#include "Precompiled.hpp"

#include "Game/Entity/EntityHandle.hpp"

// Forward declarations.
class TransformComponent;
class CollisionComponent;

//
// Collision Object
//  - An intermediate object created during collision tests.
//

struct CollisionObject
{
    EntityHandle entity;
    TransformComponent* transform;
    CollisionComponent* collision;
    glm::vec4 worldAABB;
    bool enabled;
};
