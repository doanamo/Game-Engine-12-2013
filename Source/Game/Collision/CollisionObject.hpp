#pragma once

#include "Precompiled.hpp"

#include "Game/Entity/EntityHandle.hpp"

// Forward declarations.
class TransformComponent;
class CollisionComponent;
class ScriptComponent;

//
// Collision Object
//  - An intermediate object created during collision tests.
//

struct CollisionObject
{
    EntityHandle entity;
    TransformComponent* transform;
    CollisionComponent* collision;
    ScriptComponent* script;
    glm::vec4 worldAABB;
    bool enabled;
};
