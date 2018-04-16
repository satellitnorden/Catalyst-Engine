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

	//Whether or not to draw a particular grid cell.
	DynamicArray<bool> shouldDrawGridCell;

	//The offsets into the transformations buffer.
	DynamicArray<uint64> transformationOffsets;

	//The number of instances.
	DynamicArray<uint32> instanceCounts;

};