#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"

// Forward declarations.
class TransformComponent;
class CollisionComponent;
class ScriptComponent;

//
// Collision Object
//

struct CollisionObject
{
    EntityHandle entity;
    TransformComponent* transform;
    CollisionComponent* collision;
    ScriptComponent* script;
    glm::vec4 worldAABB;
};

//
// Collision System
//

class CollisionSystem
{
public:
    CollisionSystem();
    ~CollisionSystem();

    bool Initialize();
    void Cleanup();

    void Update();

private:
    // Intermediate collision objects.
    std::vector<CollisionObject> m_objects;
};
