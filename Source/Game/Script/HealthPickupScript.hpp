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

    void OnCollision(const CollisionObject& self, const CollisionObject& other);

private:
    int m_healValue;
};
