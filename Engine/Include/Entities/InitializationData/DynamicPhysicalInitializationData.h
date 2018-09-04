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
	PhysicalModel _Model;

	//The position.
	Vector3 _Position;

	//The rotation.
	Vector3 _Rotation;

	//The scale.
	Vector3 _Scale;

};