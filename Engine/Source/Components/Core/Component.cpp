//Header file.
#include <Components/Core/Component.h>

/*
*	Returns the lock for all components.
*/
NO_DISCARD Spinlock &_AllComponentsLock() NOEXCEPT
{
	static Spinlock ALL_COMPONENTS_LOCK;

	return ALL_COMPONENTS_LOCK;
}

/*
*	Returns all components.
*/
NO_DISCARD DynamicArray<Component *RESTRICT> &_AllComponents() NOEXCEPT
{
	static DynamicArray<Component *RESTRICT> ALL_COMPONENTS;

	return ALL_COMPONENTS;
}

/*
*	Returns all components.
*/
NO_DISCARD const DynamicArray<Component *RESTRICT> &AllComponents() NOEXCEPT
{
	return _AllComponents();
}

/*
*	Adds a component to all component.
*/
void AddComponentToAllComponents(Component* const RESTRICT component) NOEXCEPT
{
	SCOPED_LOCK(_AllComponentsLock());
	_AllComponents().Emplace(component);
}