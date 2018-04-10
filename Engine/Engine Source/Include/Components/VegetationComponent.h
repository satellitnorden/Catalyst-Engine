#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/RenderingCore.h>

class VegetationComponent final
{

public:

	//The descriptor set.
	DescriptorSetHandle descriptorSet;

	//The transformations buffer.
	GraphicsBufferHandle transformationsBuffer;

	//The number of instances.
	uint32 instanceCount;

};