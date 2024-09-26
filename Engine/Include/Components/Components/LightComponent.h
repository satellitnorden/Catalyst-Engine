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

	//The light type.
	LightType _LightType;

	struct
	{
		//The rotation.
		EulerAngles _Rotation;
	} _DirectionalLightData;

	struct
	{
		//The radius.
		float32 _Radius{ 1.0f };

		//The size.
		float32 _Size{ 1.0f };
	} _PointLightData;

	//The color.
	Vector3<float32> _Color;

	//The light properties.
	uint32 _LightProperties;

	//The intensity.
	float32 _Intensity;

};

class LightInstanceData final
{

public:

	//The light type.
	LightType _LightType;

	struct
	{
		//The rotation.
		EulerAngles _Rotation;
	} _DirectionalLightData;

	struct
	{
		//The radius.
		float32 _Radius{ 1.0f };

		//The size.
		float32 _Size{ 1.0f };
	} _PointLightData;

	//The color.
	Vector3<float32> _Color;

	//The light properties.
	uint32 _LightProperties;

	//The intensity.
	float32 _Intensity;

};

class LightComponent final : public Component
{

	//Component declaration.
	CATALYST_COMPONENT
	(
		Light,
		COMPONENT_INITIALIZE()
		COMPONENT_DEFAULT_INITIALIZATION_DATA(LightInitializationData)
	);

};