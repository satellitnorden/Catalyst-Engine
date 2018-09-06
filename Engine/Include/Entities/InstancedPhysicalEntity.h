#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Entity.
#include <Entities/Entity.h>

//Forward declarations.
class Matrix4;
class PhysicalModel;
class PhysicalMaterial;

class InstancedPhysicalEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	InstancedPhysicalEntity() NOEXCEPT;

	/*
	*	Initializes this instanced physical entity.
	*/
	void Initialize(const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) NOEXCEPT;

};