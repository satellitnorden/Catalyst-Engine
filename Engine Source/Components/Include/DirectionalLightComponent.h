#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <Vector3.h>

class DirectionalLightComponent final
{

public:

	//The intensity.
	float intensity{ 1.0f };

	//The rotation.
	Vector3 rotation{ 0.0f, 0.0f, 0.0f };

	//The color.
	Vector3 color{ 1.0f, 1.0f, 1.0f };

};