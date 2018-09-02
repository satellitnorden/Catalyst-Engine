#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>

//Rendering.
#include <Rendering/Engine/PhysicalModel.h>

class DynamicPhysicalInitializationData final
{

public:

	//The model.
	PhysicalModel model;

	//The position.
	Vector3 position;

	//The rotation.
	Vector3 rotation;

	//The scale.
	Vector3 scale;

};