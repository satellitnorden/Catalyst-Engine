#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/RenderingCore.h>
#include <Rendering/Engine Layer/WaterUniformData.h>

class WaterComponent final
{

public:

	//The water uniform data.
	WaterUniformData waterUniformData;

	//The uniform buffer.
	UniformBufferHandle uniformBuffer;

};