#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Math.
#include <Math/General/EulerAngles.h>

//World.
#include <World/Core/WorldPosition.h>

class UserInterfaceInitializationData final : public EntityInitializationData
{

public:

	//The user interface scene identifier.
	HashString _UserInterfaceSceneIdentifier;

	//The initial world position.
	WorldPosition _InitialWorldPosition;

	//The initial rotation.
	EulerAngles _InitialRotation;

	//The initial scale.
	Vector2<float32> _InitialScale;

	//The initial roughness.
	float32 _InitialRoughness;

	//The initial metallic.
	float32 _InitialMetallic;

	//The initial ambient occlusion.
	float32 _InitialAmbientOcclusion;

	//The initial emissive multiplier.
	float32 _InitialEmissiveMultiplier;

};