#pragma once

#include "Precompiled.hpp"

// Forward declarations.
class ConsoleDefinition;

//
// Console System
//  Resposible for managing console definitions and executing commands on them.
//

class ConsoleSystem : public NonCopyable
{
public:
    ConsoleSystem();
    ~ConsoleSystem();

    bool Initialize();
    void Cleanup();

    void Execute(std::string input);

    void RegisterDefinition(ConsoleDefinition* definition);
    void UnregisterDefinition(ConsoleDefinition* definition);

    ConsoleDefinition* FindDefinition(std::string name);
    
    template<class Type>
    Type* FindDefinition(std::string name);

    void ResetIntermediateState();

    // Accessor methods.
    bool IsValid() const
    {
        return m_initialized;
    }

public:
    // Ignore letter case when comparing two keys.
    struct CaseInsensitive : public std::binary_function<std::string, std::string, bool>
    {
        bool operator()(const std::string& left, const std::string& right) const
        {
            return strcasecmp(left.c_str(), right.c_str()) < 0;
        }
    };

private:
    // List of registered definitions.
    std::map<std::string, ConsoleDefinition*, CaseInsensitive> m_definitions;

    bool m_initialized;
};

// Template implementation.
template<class Type>
Type* ConsoleSystem::FindDefinition(std::string name)
{
    static_assert(std::is_base_of<ConsoleDefinition, Type>::value, "Not a console definition type.");
    return dynamic_cast<Type*>(this->FindDefinition(name));
}
