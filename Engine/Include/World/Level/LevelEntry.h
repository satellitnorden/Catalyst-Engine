#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Physics.
#include <Physics/ModelCollisionData.h>
#include <Physics/ModelPhysicsSimulationData.h>

//World.
#include <World/Core/WorldTransform.h>

class LevelEntry final
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		DYNAMIC_MODEL
	};

	/*
	*	Dynamic model data class definition.
	*/
	class DynamicModelData final
	{

	public:

		//The initial world transform.
		WorldTransform _InitialWorldTransform;

		//The model resource identifier.
		HashString _ModelResourceIdentifier;

		//The material resourceidentifiers.
		StaticArray<HashString, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _MaterialResourceIdentifiers;

		//The model collision data.
		ModelCollisionData _ModelCollisionData;

		//Denotes whether or not to simulate physics.
		bool _SimulatePhysics;

		//The model physics simulation data.
		ModelPhysicsSimulationData _ModelPhysicsSimulationData;

	};

	//The type.
	Type _Type;

	union
	{
		//The dynamic model data.
		DynamicModelData _DynamicModelData;
	};

	/*
	*	Default constructor.
	*/
	FORCE_INLINE LevelEntry() NOEXCEPT
	{

	}

	/*
	*	Copy constructor.
	*/
	FORCE_INLINE LevelEntry(const LevelEntry &other) NOEXCEPT
	{
		Memory::Copy(this, &other, sizeof(LevelEntry));
	}

};