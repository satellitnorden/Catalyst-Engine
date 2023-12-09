#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Resources.
#include <Resources/Core/Resource.h>

class RenderPipelineResource final : public Resource
{

public:

	//The vertex shader handle.
	ShaderHandle _VertexShaderHandle{ EMPTY_HANDLE };

	//The fragment shader handle.
	ShaderHandle _FragmentShaderHandle{ EMPTY_HANDLE };

	//The input render targets.
	DynamicArray<HashString> _InputRenderTargets;

	//The output depth buffer.
	HashString _OutputDepthBuffer;

	//The output render targets.
	DynamicArray<HashString> _OutputRenderTargets;

	//The color load operator.
	AttachmentLoadOperator _ColorLoadOperator;

	//The color store operator.
	AttachmentStoreOperator _ColorStoreOperator;

	//The depth/stencil load operator.
	AttachmentLoadOperator _DepthStencilLoadOperator;

	//The depth/stencil store operator.
	AttachmentStoreOperator _DepthStencilStoreOperator;

};