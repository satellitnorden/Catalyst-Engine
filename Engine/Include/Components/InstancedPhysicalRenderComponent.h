#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class InstancedPhysicalRenderComponent final
{

public:

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The model buffer.
	ConstantBufferHandle _ModelBuffer;

	//The transformations buffer.
	ConstantBufferHandle _TransformationsBuffer;

	//The offset for the indices in the buffer.
	uint64 _IndexOffset;

	//The index count.
	uint32 _IndexCount;

	//The number of instances.
	uint32 _InstanceCount;

};