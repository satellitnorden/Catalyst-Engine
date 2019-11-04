#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class CloudsGraphicsPipeline final : public GraphicsPipeline
{

public:

	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(const DepthBufferHandle depthBuffer) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute() NOEXCEPT;

private:

	//The cloud texture.
	Texture3DHandle _CloudTexture;

	/*
	*	Creates the cloud texture.
	*/
	void CreateCloudTexture() NOEXCEPT;

};