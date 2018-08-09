#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>
#include <Rendering/Engine/WaterUniformData.h>

class WaterComponent final
{

public:

	//The water uniform data.
	WaterUniformData waterUniformData;

	//The uniform buffer.
	UniformBufferHandle uniformBuffer;

};