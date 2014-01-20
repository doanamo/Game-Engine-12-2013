#include "Precompiled.hpp"
#include "Script.hpp"

Script::Script() :
    m_function(nullptr)
{
}

Script::~Script()
{
}

void Script::SetFunction(FunctionPtr function)
{
    m_function = function;
}
