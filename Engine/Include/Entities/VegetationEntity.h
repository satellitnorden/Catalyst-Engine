#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Entity.
#include <Entities/Entity.h>

//Rendering
#include <Rendering/Engine/VegetationProperties.h>
#include <Rendering/Engine/VegetationTransformation.h>

//Forward declarations.
class VegetationMaterial;

class VegetationEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	VegetationEntity() NOEXCEPT;

	/*
	*	Initializes this vegetation entity.
	*/
	void Initialize(const VegetationMaterial &material, const DynamicArray<VegetationTransformation> &transformations, const VegetationProperties &properties) NOEXCEPT;

};