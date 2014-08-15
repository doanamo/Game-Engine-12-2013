#include "Precompiled.hpp"
#include "Element.hpp"
#include "Root.hpp"
using namespace Interface;

Element::Element(Root* root) :
    m_root(root),
    m_parent(nullptr),
    m_next(nullptr),
    m_begin(nullptr),
    m_end(nullptr)
{
}

Element::~Element()
{
    Cleanup();
}

void Element::Cleanup()
{
    // Remove all childs.
    RemoveAllChilds();

    // Remove from the parent.
    if(m_parent != nullptr)
    {
        bool result = m_parent->RemoveChild(this);

        assert(result == true);
        assert(m_root == nullptr);
        assert(m_parent == nullptr);
        assert(m_next == nullptr);
    }
}

bool Element::Process(const SDL_Event& event)
{
    Element* iterator = this->Begin();

    while(iterator != nullptr)
    {
        // Handle input by childs.
        if(iterator->Process(event))
            return true;

        // Advance the iterator.
        iterator = iterator->GetNext();
    }

    return false;
}

void Element::Update(float timeDelta)
{
    Element* iterator = this->Begin();

    while(iterator != nullptr)
    {
        // Update childs.
        iterator->Update(timeDelta);

        // Advance the iterator.
        iterator = iterator->GetNext();
    }
}

void Element::Draw()
{
    Element* iterator = this->Begin();

    while(iterator != nullptr)
    {
        // Draw childs.
        iterator->Draw();

        // Advance the iterator.
        iterator = iterator->GetNext();
    }
}

bool Element::AddChild(Element* element)
{
    // Must have a root to add child elements.
    if(m_root == nullptr)
        return false;

    // Check for a null pointer.
    if(element == nullptr)
        return false;

    // Check if trying to add itself.
    if(element == this)
        return false;

    // Check if element already has a parent.
    if(element->m_parent != nullptr)
        return false;

    assert(element->m_root == nullptr);
    assert(element->m_next == nullptr);

    // Add an element to the linked list.
    if(m_begin == nullptr)
    {
        assert(m_end == nullptr);

        m_begin = element;
        m_end = element;
    }
    else
    {
        assert(m_end != nullptr);

        m_end->m_next = element;
        m_end = element;
    }

    // Set this as element's new parent.
    element->m_root = m_root;
    element->m_parent = this;

    return true;
}

bool Element::RemoveChild(Element* element)
{
    // Check for a null pointer.
    if(element == nullptr)
        return false;

    // Check if element is our child.
    if(element->m_parent != this)
        return false;

    // Remove element from the linked list.
    if(m_begin == element)
    {
        if(m_end == element)
        {
            m_begin = nullptr;
            m_end = nullptr;
        }
        else
        {
            m_begin = element->m_next;
        }
    }
    else
    {
        Element* previous = PreviousChild(element);
        assert(previous != nullptr);

        previous->m_next = element->m_next;

        if(m_end == element)
        {
            m_end = previous;
        }
    }

    // Orphan the removed child.
    element->m_root = nullptr;
    element->m_parent = nullptr;
    element->m_next = nullptr;

    return true;
}

void Element::RemoveAllChilds()
{
    // Remove all childs.
    Element* iterator = m_begin;
        
    while(iterator != nullptr)
    {
        Element* element = iterator;

        // Advance to the next receiver.
        iterator = iterator->m_next;

        // Unsubscribe a receiver.
        element->m_root = nullptr;
        element->m_parent = nullptr;
        element->m_next = nullptr;
    }
        
    m_begin = nullptr;
    m_end = nullptr;
}

Element* Element::PreviousChild(Element* element)
{
    // Iterate through all elements.
    Element* iterator = m_begin;
    Element* previous = nullptr;

    while(iterator != nullptr)
    {
        // Check if it's the one we are looking for.
        if(iterator == element)
            break;

        // Save the previous iterator.
        previous = iterator;

        // Iterate to the next element.
        iterator = iterator->m_next;
    }

    // Return the previous element.
    return previous;
}
