#pragma once

//Core.
#include <Core/EngineCore.h>

//Math.
#include <Math/Vector3.h>

class TransformComponent final
{

public:

	//The position.
	Vector3 position{ 0.0f, 0.0f, 0.0f };

	//The rotation.
	Vector3 rotation{ 0.0f, 0.0f, 0.0f };

	//The scale.
	Vector3 scale{ 1.0f, 1.0f, 1.0f };

};