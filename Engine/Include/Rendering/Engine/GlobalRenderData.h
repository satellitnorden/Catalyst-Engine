#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class GlobalRenderData final
{

public:

	//The render data tables.
	DynamicArray<RenderDataTableHandle> _RenderDataTables;

	//The dynamic uniform data buffers.
	DynamicArray<UniformBufferHandle> _DynamicUniformDataBuffers;

};