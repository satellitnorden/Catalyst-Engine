#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Math.
#include <Math/General/EulerAngles.h>
#include <Math/General/Vector.h>

class LightInitializationData final : public ComponentInitializationData
{

public:

	union
	{
		struct
		{
			//The rotation.
			EulerAngles _Rotation;
		} _DirectionalLightData;

		struct
		{
			//The radius.
			float32 _Radius;

			//The size.
			float32 _Size;
		} _PointLightData;
	};

	//The color.
	Vector3<float32> _Color;

	//The light type.
	LightType _LightType;

	//The light properties.
	uint32 _LightProperties;

	//The intensity.
	float32 _Intensity;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE LightInitializationData() NOEXCEPT
	{
		Memory::Set(this, 0, sizeof(LightInitializationData));
	}

};

class LightInstanceData final
{

public:

	union
	{
		struct
		{
			//The rotation.
			EulerAngles _Rotation;
		} _DirectionalLightData;

		struct
		{
			//The radius.
			float32 _Radius;

			//The size.
			float32 _Size;
		} _PointLightData;
	};

	//The color.
	Vector3<float32> _Color;

	//The light type.
	LightType _LightType;

	//The light properties.
	uint32 _LightProperties;

	//The intensity.
	float32 _Intensity;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE LightInstanceData() NOEXCEPT
	{
		Memory::Set(this, 0, sizeof(LightInstanceData));
	}

};

class LightComponent final : public Component
{

	//Component declaration.
	CATALYST_COMPONENT
	(
		Light
	);

public:
	void DefaultInitializationData(ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT override;

};