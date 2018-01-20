//Header file.
#include <Component.h>

/*
*	Default constructor.
*/
Component::Component() CATALYST_NOEXCEPT
{

}

/*
*	Constructor taking all values as arguments.
*/
Component::Component(const ComponentType initialComponentType, Entity *CATALYST_RESTRICT initialOwner) CATALYST_NOEXCEPT
	:
	componentType(initialComponentType),
	owner(initialOwner)
{

}

/*
*	Default destructor.
*/
Component::~Component() CATALYST_NOEXCEPT
{

}