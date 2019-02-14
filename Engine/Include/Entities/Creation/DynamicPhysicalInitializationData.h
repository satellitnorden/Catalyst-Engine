#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Math.
#include <Math/Vector3.h>

//Rendering.
#include <Rendering/Engine/PhysicalMaterial.h>
#include <Rendering/Engine/PhysicalModel.h>

class DynamicPhysicalInitializationData final : public EntityInitializationData
{

public:

	//The flags.
	PhysicalFlag _Flags;

	//The model.
	PhysicalModel _Model;

	//The model.
	PhysicalMaterial _Material;

	//The position.
	Vector3<float> _Position;

	//The rotation.
	Vector3<float> _Rotation;

	//The scale.
	Vector3<float> _Scale;

	//The outline color.
	Vector3<float> _OutlineColor;

};