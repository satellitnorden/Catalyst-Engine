#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

/*
*	Container for graphics render pipeline information.
*/
class GraphicsRenderPipelineInformation final
{

public:

	//The output depth buffer.
	HashString _OutputDepthBuffer;

	//The render resolution.
	HashString _RenderResolution;

	//The color load operator.
	AttachmentLoadOperator _ColorLoadOperator;

	//The color store operator.
	AttachmentStoreOperator _ColorStoreOperator;

	//The depth/stencil load operator.
	AttachmentLoadOperator _DepthStencilLoadOperator;

	//The depth/stencil store operator.
	AttachmentStoreOperator _DepthStencilStoreOperator;

	//The cull mode.
	CullMode _CullMode;

	//Denotes whether or not blend is enabled.
	bool _BlendEnabled;

	//The blend color source factor.
	BlendFactor _BlendColorSourceFactor;

	//The blend color destination factor.
	BlendFactor _BlendColorDestinationFactor;

	//The blend color operator.
	BlendOperator _BlendColorOperator;

	//The blend alpha source factor.
	BlendFactor _BlendAlphaSourceFactor;

	//The blend alpha destination factor.
	BlendFactor _BlendAlphaDestinationFactor;

	//The blend alpha operator.
	BlendOperator _BlendAlphaOperator;

};

/*
*	Class for retrieving render pipeline information.
*/
class RenderPipelineInformation final
{

public:

	/*
	*	Retrieves graphics render pipeline information.
	*/
	static void RetrieveGraphicsRenderPipelineInformation(const HashString identifier, GraphicsRenderPipelineInformation *const RESTRICT information) NOEXCEPT;

};