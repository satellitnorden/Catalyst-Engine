#pragma once

//Engine core.
#include <EngineCore.h>

//Components.
#include <PhysicalGraphicsComponent.h>

class ComponentManager
{

public:

	/*
	*	Returns the physical graphics components.
	*/
	static const DynamicArray<PhysicalGraphicsComponent>& GetPhysicalGraphicsComponents() CATALYST_NOEXCEPT;

	/*
	*	Returns the index for a new physical entity graphics component.
	*/
	static size_t GetNewPhysicalGraphicsComponent() CATALYST_NOEXCEPT;

	/*
	*	Returns the physical entity graphics component at the specified index, const.
	*/
	static const PhysicalGraphicsComponent& GetPhysicalGraphicsComponentConst(const size_t index) CATALYST_NOEXCEPT;

	/*
	*	Returns the physical entity graphics component at the specified index, non-const.
	*/
	static PhysicalGraphicsComponent& GetPhysicalGraphicsComponentNonConst(const size_t index) CATALYST_NOEXCEPT;

private:

	//The physical graphics components.
	static DynamicArray<PhysicalGraphicsComponent> physicalGraphicsComponents;

};