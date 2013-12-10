#pragma once

//
// NonCopyable
//

// Non-copyable base class.
class NonCopyable
{
protected:
    // Use empty constructor and destructor.
    NonCopyable() { }
    ~NonCopyable() { }

private:
    // Make default copy constructor and copy operator private.
    NonCopyable(const NonCopyable&);
    NonCopyable& operator=(const NonCopyable&);
};
