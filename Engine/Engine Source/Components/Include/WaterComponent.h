#pragma once

//Engine core.
#include <EngineCore.h>

//Rendering.
#include <RenderingCore.h>
#include <WaterUniformData.h>

class WaterComponent final
{

public:

	//The water uniform data.
	WaterUniformData waterUniformData;

	//The uniform buffer.
	UniformBufferHandle uniformBuffer;

};