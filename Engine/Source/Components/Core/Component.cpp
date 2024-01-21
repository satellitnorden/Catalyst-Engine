//Header file.
#include <Components/Core/Component.h>

/*
*	Returns all components.
*/
NO_DISCARD DynamicArray<Component *RESTRICT> &AllComponents() NOEXCEPT
{
	static DynamicArray<Component *RESTRICT> ALL_COMPONENTS;

	return ALL_COMPONENTS;
}