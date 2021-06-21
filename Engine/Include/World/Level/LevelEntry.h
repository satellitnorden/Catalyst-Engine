#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Physics.
#include <Physics/Native/ModelCollisionConfiguration.h>
#include <Physics/Native/ModelSimulationConfiguration.h>

//World.
#include <World/Core/WorldTransform.h>

class LevelEntry final
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		NONE,

		DYNAMIC_MODEL,
		LIGHT,
		STATIC_MODEL,
		USER_INTERFACE
	};

	/*
	*	Dynamic model data class definition.
	*/
	class DynamicModelData final
	{

	public:

		//The current version.
		constexpr static uint64 CURRENT_VERSION{ 1 };

		//The world transform.
		WorldTransform _WorldTransform;

		//The model resource identifier.
		HashString _ModelResourceIdentifier;

		//The material resourceidentifiers.
		DynamicArray<HashString> _MaterialResourceIdentifiers;

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

		//The current version.
		constexpr static uint64 CURRENT_VERSION{ 1 };

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

		/*
		*	Default constructor.
		*/
		FORCE_INLINE LightData() NOEXCEPT
		{

		}

		/*
		*	Copy operator overload.
		*/
		FORCE_INLINE void operator=(const LightData& other) NOEXCEPT
		{
			Memory::Copy(this, &other, sizeof(LightData));
		}

	};

	/*
	*	Static model data class definition.
	*/
	class StaticModelData final
	{

	public:

		//The current version.
		constexpr static uint64 CURRENT_VERSION{ 1 };

		//The world transform.
		WorldTransform _WorldTransform;

		//The model resource identifier.
		HashString _ModelResourceIdentifier;

		//The material resourceidentifiers.
		DynamicArray<HashString> _MaterialResourceIdentifiers;

		//The model collision configuration.
		ModelCollisionConfiguration _ModelCollisionConfiguration;

	};

	/*
	*	User interface data class definition.
	*/
	class UserInterfaceData final
	{

	public:

		//The current version.
		constexpr static uint64 CURRENT_VERSION{ 1 };

		//The user interface scene identifier.
		HashString _UserInterfaceSceneIdentifier;

		//The world position.
		WorldPosition _WorldPosition;

		//The rotation.
		EulerAngles _Rotation;

		//The scale.
		Vector2<float32> _Scale;

		//The roughness.
		float32 _Roughness;

		//The metallic.
		float32 _Metallic;

		//The ambient occlusion.
		float32 _AmbientOcclusion;

		//The emissive multiplier.
		float32 _EmissiveMultiplier;
	};

	//The type.
	Type _Type;

	//The version.
	uint64 _Version;

	union
	{
		//The dynamic model data.
		DynamicModelData _DynamicModelData;

		//The light data.
		LightData _LightData;

		//The static model data.
		StaticModelData _StaticModelData;

		//The user interface data.
		UserInterfaceData _UserInterfaceData;
	};

	/*
	*	Default constructor.
	*/
	FORCE_INLINE LevelEntry() NOEXCEPT
	{
		Memory::Set(this, 0, sizeof(LevelEntry));
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~LevelEntry() NOEXCEPT
	{

	}

	/*
	*	Copy constructor.
	*/
	FORCE_INLINE LevelEntry(const LevelEntry &other) NOEXCEPT
	{
		Memory::Set(this, 0, sizeof(LevelEntry));

		_Type = other._Type;
		_Version = other._Version;

		switch (_Type)
		{
			case Type::DYNAMIC_MODEL:
			{
				_DynamicModelData = other._DynamicModelData;

				break;
			}

			case Type::LIGHT:
			{
				_LightData = other._LightData;

				break;
			}

			case Type::STATIC_MODEL:
			{
				_StaticModelData = other._StaticModelData;

				break;
			}

			case Type::USER_INTERFACE:
			{
				_UserInterfaceData = other._UserInterfaceData;

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

};