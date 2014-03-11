#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"

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

//
// Collision System
//

class CollisionSystem
{
public:
    static const float Permanent;

public:
    CollisionSystem();
    ~CollisionSystem();

    bool Initialize();
    void Cleanup();

    void Update(float timeDelta);

    void DisableCollisionResponse(EntityHandle sourceEntity, EntityHandle targetEntity, float duration = Permanent);

private:
    // Intermediate collision objects.
    std::vector<CollisionObject> m_objects;

    // Disabled collision responses.
    typedef std::pair<EntityHandle, EntityHandle> EntityPair;
    typedef std::unordered_multimap<EntityPair, float> DisabledList;

    DisabledList m_disabled;
};
