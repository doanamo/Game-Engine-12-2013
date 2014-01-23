#include "Precompiled.hpp"
#include "CollisionSystem.hpp"

#include "Transform.hpp"
#include "Collision.hpp"

#include "GameContext.hpp"
#include "EntitySystem.hpp"

namespace
{
    void TransformBoundingBox(glm::vec4* boundingBox, const Transform* transform)
    {
        // Transform only by position (temp).
        boundingBox->x += transform->GetPosition().x;
        boundingBox->y += transform->GetPosition().y;
        boundingBox->z += transform->GetPosition().x;
        boundingBox->w += transform->GetPosition().y;
    }

    bool IntersectBoundingBox(const glm::vec4& a, const glm::vec4& b)
    {
        // Checks if bounding boxes collide.
        return !(a.x > b.z || a.z < b.x || a.y > b.w || a.w < b.y);
    }
}

CollisionSystem::CollisionSystem()
{
}

CollisionSystem::~CollisionSystem()
{
    Cleanup();
}

bool CollisionSystem::Initialize()
{
    Cleanup();

    return true;
}

void CollisionSystem::Cleanup()
{
}

void CollisionSystem::Update()
{
    // Warning: The below implementation is very naive.

    // Process collision components.
    for(auto it = Game::CollisionComponents().Begin(); it != Game::CollisionComponents().End(); ++it)
    {
        // Check if entity is active.
        if(!Game::EntitySystem().IsHandleValid(it->first))
            continue;

        // Get the collision component.
        Collision& collision = it->second;

        if(!collision.IsEnabled())
            continue;

        // Get the transform component.
        Transform* transform = Game::TransformComponents().Lookup(it->first);

        if(transform == nullptr)
            continue;

        // Transform the bounding box.
        glm::vec4 boundingBox = collision.GetBoundingBox();
        TransformBoundingBox(&boundingBox, transform);

        // Check if the entity collides with other entities.
        for(auto other = Game::CollisionComponents().Begin(); other != Game::CollisionComponents().End(); ++other)
        {
            // Don't check against itself.
            if(other == it)
                continue;

            // Check if entity is active.
            if(!Game::EntitySystem().IsHandleValid(other->first))
                continue;

            // Get the collision component.
            Collision& otherCollision = other->second;

            if(!otherCollision.IsEnabled())
                continue;

            // Get the transform component.
            Transform* otherTransform = Game::TransformComponents().Lookup(other->first);

            if(transform == nullptr)
                continue;

            // Transform the bounding box.
            glm::vec4 otherBoundingBox = otherCollision.GetBoundingBox();
            TransformBoundingBox(&otherBoundingBox, otherTransform);

            // Check for collision.
            if(IntersectBoundingBox(boundingBox, otherBoundingBox))
            {
                // Disable collisions.
                collision.Disable();
                otherCollision.Disable();

                // Destroy both entities.
                Game::EntitySystem().DestroyEntity(it->first);
                Game::EntitySystem().DestroyEntity(other->first);

                // Don't check for more.
                break;
            }
        }
    }
}
