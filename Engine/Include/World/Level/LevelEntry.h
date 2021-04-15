#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/HashString.h>

//Physics.
#include <Physics/Native/ModelCollisionConfiguration.h>
#include <Physics/Native/ModelSimulationConfiguration.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//World.
#include <World/Core/WorldTransform.h>

class LevelEntry final
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		DYNAMIC_MODEL,
		STATIC_MODEL
	};

	/*
	*	Dynamic model data class definition.
	*/
	class DynamicModelData final
	{

	public:

		//The world transform.
		WorldTransform _WorldTransform;

		//The model resource identifier.
		HashString _ModelResourceIdentifier;

		//The material resourceidentifiers.
		StaticArray<HashString, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _MaterialResourceIdentifiers;

		//The model collision configuration.
		ModelCollisionConfiguration _ModelCollisionConfiguration;

		//The model simulation configuration.
		ModelSimulationConfiguration _ModelSimulationConfiguration;

	};

	/*
	*	Static model data class definition.
	*/
	class StaticModelData final
	{

	public:

		//The world transform.
		WorldTransform _WorldTransform;

		//The model resource identifier.
		HashString _ModelResourceIdentifier;

		//The material resourceidentifiers.
		StaticArray<HashString, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _MaterialResourceIdentifiers;

		//The model collision configuration.
		ModelCollisionConfiguration _ModelCollisionConfiguration;

	};

	//The type.
	Type _Type;

	union
	{
		//The dynamic model data.
		DynamicModelData _DynamicModelData;

		//The static model data.
		StaticModelData _StaticModelData;
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