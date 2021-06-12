#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/HashString.h>

//Lighting.
#include <Lighting/LightingCore.h>

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
		LIGHT,
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
	*	Light data class definition.
	*/
	class LightData final
	{

	public:

		union
		{
			//The rotation. Used for directional lights.
			EulerAngles _Rotation;

			//The world position. Used for point lights.
			WorldPosition _WorldPosition;
		};

		//The color.
		Vector3<float32> _Color;

		//The light type.
		LightType _LightType;

		//The light properties.
		uint32 _LightProperties;

		//The intensity.
		float32 _Intensity;

		//The radius.
		float32 _Radius;

		//The size.
		float32 _Size;

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

		//The light data.
		LightData _LightData;

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

static_assert(sizeof(LevelEntry) == 112, "Level Entry size has changed, resources needs to be rebuilt.");