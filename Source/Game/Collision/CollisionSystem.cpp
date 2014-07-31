#include "Precompiled.hpp"
#include "CollisionSystem.hpp"
#include "CollisionComponent.hpp"

#include "Game/Event/EventDefinitions.hpp"
#include "Game/Event/EventSystem.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Component/ComponentSystem.hpp"
#include "Game/Transform/TransformComponent.hpp"

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

CollisionSystem::CollisionSystem() :
    m_initialized(false),
    m_eventSystem(nullptr),
    m_entitySystem(nullptr),
    m_componentSystem(nullptr)
{
}

CollisionSystem::~CollisionSystem()
{
    Cleanup();
}

void CollisionSystem::Cleanup()
{
    m_initialized = false;

    m_eventSystem = nullptr;
    m_entitySystem = nullptr;
    m_componentSystem = nullptr;

    ClearContainer(m_objects);
    ClearContainer(m_disabled);
}

bool CollisionSystem::Initialize(EventSystem* eventSystem, EntitySystem* entitySystem, ComponentSystem* componentSystem)
{
    Cleanup();

    // Validate arguments.
    if(eventSystem == nullptr)
        return false;

    if(entitySystem == nullptr)
        return false;

    if(componentSystem == nullptr)
        return false;

    m_eventSystem = eventSystem;
    m_entitySystem = entitySystem;
    m_componentSystem = componentSystem;

    // Declare required components.
    m_componentSystem->Declare<TransformComponent>();
    m_componentSystem->Declare<CollisionComponent>();

    // Success!
    return m_initialized = true;
}

void CollisionSystem::Update(float timeDelta)
{
    assert(m_initialized);

    // Update timers of disabled collision response pairs.
    for(auto it = m_disabled.begin(); it != m_disabled.end();)
    {
        const EntityHandle& sourceEntity = it->first.first;
        const EntityHandle& targetEntity = it->first.second;

        float& time = it->second;

        // Save a copy of the iterator and step forward the original.
        auto eraseIt = it++;

        // Check if entities are still valid.
        bool sourceEntityValid = m_entitySystem->IsHandleValid(sourceEntity);
        bool targetEntityValid = m_entitySystem->IsHandleValid(targetEntity);

        if(!sourceEntityValid || !targetEntityValid)
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
    auto componentsBegin = m_componentSystem->Begin<CollisionComponent>();
    auto componentsEnd = m_componentSystem->End<CollisionComponent>();

    for(auto it = componentsBegin; it != componentsEnd; ++it)
    {
        // Check if entity is active.
        if(!m_entitySystem->IsHandleValid(it->first))
            continue;

        // Get the collision component.
        CollisionComponent* collision = &it->second;
        assert(collision != nullptr);

        if(!collision->IsEnabled())
            continue;

        // Get the transform component.
        TransformComponent* transform = m_componentSystem->Lookup<TransformComponent>(it->first);

        if(transform == nullptr)
            continue;

        // Transform the bounding box to world space.
        glm::vec4 boundingBox = collision->GetBoundingBox();
        TransformBoundingBox(&boundingBox, transform);

        // Add a collision object.
        CollisionObject object;
        object.entity = it->first;
        object.transform = transform;
        object.collision = collision;
        object.worldAABB = boundingBox;
        object.enabled = true;

        m_objects.push_back(object);
    }

    // Process collision objects.
    for(auto it = m_objects.begin(); it != m_objects.end(); ++it)
    {
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
                    // Dispatch an entity collision event.
                    {
                        GameEvent::EntityCollision event(*it, *other);
                        m_eventSystem->Dispatch(event);
                    }

                    // Check if other collision object is still valid.
                    if(!m_entitySystem->IsHandleValid(other->entity) || !other->collision->IsEnabled())
                    {
                        other->enabled = false;
                    }

                    // Check if this collision object is still valid.
                    if(!m_entitySystem->IsHandleValid(it->entity) || !it->collision->IsEnabled())
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
    assert(m_initialized);

    // Check if this pair is already disabled.
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
