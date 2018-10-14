#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Entities.
#include <Entities/InitializationData/EntityInitializationData.h>

//Math.
#include <Math/Vector3.h>

//Rendering.
#include <Rendering/Engine/PhysicalMaterial.h>
#include <Rendering/Engine/PhysicalModel.h>

class DynamicPhysicalInitializationData final : public EntityInitializationData
{

public:

	//The physical flags.
	PhysicalFlag _PhysicalFlags;

	//The model.
	PhysicalModel _Model;

	//The model.
	PhysicalMaterial _Material;

	//The position.
	Vector3 _Position;

	//The rotation.
	Vector3 _Rotation;

	//The scale.
	Vector3 _Scale;

	//The outline color.
	Vector3 _OutlineColor;

};