#pragma once

#include "Precompiled.hpp"
#include "Script.hpp"

//
// Health Pickup Script
//

class HealthPickupScript : public Script
{
public:
    HealthPickupScript(int healValue);

    void OnUpdate(EntityHandle self, float timeDelta);
    void OnCollision(CollisionObject& self, CollisionObject& other);

private:
    int m_healValue;
};
