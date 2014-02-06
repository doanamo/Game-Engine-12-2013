#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"

// Forward declarations.
class Script;

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

public:
    ScriptComponent();
    ~ScriptComponent();

    void SetScript(ScriptPtr script)
    {
        m_script = script;
    }

    ScriptPtr GetScript()
    {
        return m_script;
    }
    
private:
    // Script object.
    ScriptPtr m_script;
};
