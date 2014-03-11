#include "Precompiled.hpp"
#include "CollisionSystem.hpp"

#include "TransformComponent.hpp"
#include "CollisionComponent.hpp"
#include "ScriptComponent.hpp"
#include "Script.hpp"

#include "GameContext.hpp"
#include "EntitySystem.hpp"

namespace
{
    void TransformBoundingBox(glm::vec4* boundingBox, const TransformComponent* transform)
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

const float CollisionSystem::Permanent = -1.0f;

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
    ClearContainer(m_objects);
    ClearContainer(m_disabled);
}

void CollisionSystem::Update(float timeDelta)
{
    // Update timers of disabled collision response pairs.
    for(auto it = m_disabled.begin(); it != m_disabled.end();)
    {
        const EntityHandle& sourceEntity = it->first.first;
        const EntityHandle& targetEntity = it->first.second;

        float& time = it->second;

        // Save a copy of the iterator and step forward the original.
        auto eraseIt = it++;

        // Check if entities are still valid.
        if(!Game::EntitySystem().IsHandleValid(sourceEntity) || !Game::EntitySystem().IsHandleValid(targetEntity))
        {
            m_disabled.erase(eraseIt);
            continue;
        }

        // Skip if it has been disabled permanently.
        if(time < 0.0f)
            continue;

        // Update the timer.
        time = std::max(0.0f, time - timeDelta);

        // Erase the element if outdated.
        if(time == 0.0f)
        {
            m_disabled.erase(eraseIt);
            continue;
        }
    }

    // Create a list of collision objects.
    for(auto it = Game::CollisionComponents().Begin(); it != Game::CollisionComponents().End(); ++it)
    {
        // Check if entity is active.
        if(!Game::EntitySystem().IsHandleValid(it->first))
            continue;

        // Get the collision component.
        CollisionComponent* collision = &it->second;

        assert(collision != nullptr);

        if(!collision->IsEnabled())
            continue;

        // Get the transform component.
        TransformComponent* transform = Game::TransformComponents().Lookup(it->first);

        if(transform == nullptr)
            continue;

        // Get the script component (not required).
        ScriptComponent* script = Game::ScriptComponents().Lookup(it->first);

        // Transform the bounding box to world space.
        glm::vec4 boundingBox = collision->GetBoundingBox();
        TransformBoundingBox(&boundingBox, transform);

        // Add a collision object.
        CollisionObject object;
        object.entity = it->first;
        object.transform = transform;
        object.collision = collision;
        object.script = script;
        object.worldAABB = boundingBox;
        object.enabled = true;

        m_objects.push_back(object);
    }

    // Process collision objects.
    for(auto it = m_objects.begin(); it != m_objects.end(); ++it)
    {
        // Need a script component for collision reaction.
        if(it->script == nullptr)
            continue;

        // Check if collision object is still enabled.
        if(!it->enabled)
            continue;

        // Check if it collides with other objects.
        for(auto other = m_objects.begin(); other != m_objects.end(); ++other)
        {
            // Don't check against itself.
            if(other == it)
                continue;

            // Check if collision response with other entity has been disabled.
            // This shouldn't be here before actuall collision calculation, but
            // we only do a collision response (no physical interaction) so it's
            // totally fine for now (we skip the expensive calculation).
            EntityPair pair = { it->entity, other->entity };

            if(m_disabled.count(pair) == 1)
                continue;

            // Check if collision object is still enabled.
            if(!other->enabled)
                continue;

            // Check if an object can collide with it.
            if(it->collision->GetMask() & other->collision->GetType())
            {
                // Check if collision shapes intersect.
                // If they don't interset and collision is marked as reverted, still call it.
                if(IntersectBoundingBox(it->worldAABB, other->worldAABB) == !(it->collision->GetFlags() & CollisionFlags::Reversed))
                {
                    // Execute object script.
                    it->script->OnCollision(*it, *other);

                    // Check if other collision object is still valid.
                    if(!Game::EntitySystem().IsHandleValid(other->entity) || !other->collision->IsEnabled())
                    {
                        other->enabled = false;
                    }

                    // Check if this collision object is still valid.
                    if(!Game::EntitySystem().IsHandleValid(it->entity) || !it->collision->IsEnabled())
                    {
                        it->enabled = false;
                        break;
                    }
                }
            }
        }
    }

    // Clear intermediate collision object list.
    m_objects.clear();
}

void CollisionSystem::DisableCollisionResponse(EntityHandle sourceEntity, EntityHandle targetEntity, float duration)
{
    EntityPair pair = { sourceEntity, targetEntity };
    auto it = m_disabled.find(pair);

    if(it != m_disabled.end())
    {
        // Update the duration.
        float& time = it->second;

        if(time < duration)
        {
            time = duration;
        }
    }
    else
    {
        // Insert a new pair.
        m_disabled.emplace(std::make_pair(pair, duration));
    }
}
