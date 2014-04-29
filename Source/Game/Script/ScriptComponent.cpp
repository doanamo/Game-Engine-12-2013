#include "Precompiled.hpp"
#include "ScriptComponent.hpp"

ScriptComponent::ScriptComponent()
{
}

ScriptComponent::~ScriptComponent()
{
}

void ScriptComponent::AddScript(ScriptPtr script)
{
    // Check if we already have this script instance.
    auto it = std::find(m_scripts.begin(), m_scripts.end(), script);

    if(it != m_scripts.end())
        return;

    // Add script to the list.
    m_scripts.push_back(script);
}

void ScriptComponent::OnCreate(EntityHandle self)
{
    for(ScriptPtr& script : m_scripts)
    {
        script->OnCreate(self);
    }
}

void ScriptComponent::OnUpdate(EntityHandle self, float timeDelta)
{
    for(ScriptPtr& script : m_scripts)
    {
        script->OnUpdate(self, timeDelta);
    }
}

void ScriptComponent::OnDamage(EntityHandle self, int value, bool alive)
{
    for(ScriptPtr& script : m_scripts)
    {
        script->OnDamage(self, value, alive);
    }
}

void ScriptComponent::OnHeal(EntityHandle self, int value)
{
    for(ScriptPtr& script : m_scripts)
    {
        script->OnHeal(self, value);
    }
}

void ScriptComponent::OnCollision(const CollisionObject& self, const CollisionObject& other)
{
    for(ScriptPtr& script : m_scripts)
    {
        script->OnCollision(self, other);
    }
}
