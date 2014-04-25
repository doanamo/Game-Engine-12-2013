#pragma once

#include "Precompiled.hpp"
#include "Script.hpp"

#include "Game/Component/Component.hpp"

//
// Script Component
//

class ScriptComponent : public Component
{
public:
    // Friend declarations.
    friend class ScriptSystem;

    // Type declarations.
    typedef std::shared_ptr<Script> ScriptPtr;
    typedef std::vector<ScriptPtr> ScriptList;

public:
    ScriptComponent();
    ~ScriptComponent();

    void AddScript(ScriptPtr script);

    // Script calls.
    void OnCreate(EntityHandle self);
    void OnUpdate(EntityHandle self, float timeDelta);
    void OnDamage(EntityHandle self, int value, bool alive);
    void OnHeal(EntityHandle self, int value);
    void OnCollision(CollisionObject& self, CollisionObject& other);
    
private:
    ScriptList m_scripts;
    bool       m_touched;
};
