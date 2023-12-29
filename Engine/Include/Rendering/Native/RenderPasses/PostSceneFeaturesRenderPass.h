#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/DepthDownsampleGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ALIGN(8) PostSceneFeaturesRenderPass final : public RenderPass
{
	
public:

	//Constants.
	constexpr static uint64 DEPTH_MIP_CHAIN_DEPTH{ 8 };

	//Singleton declaration.
	DECLARE_SINGLETON(PostSceneFeaturesRenderPass);

	/*
	*	Default constructor.
	*/
	PostSceneFeaturesRenderPass() NOEXCEPT;

	/*
	*	Returns the depth chain render target at the given mip index.
	*/
	FORCE_INLINE NO_DISCARD RenderTargetHandle GetDepthMip(const uint8 mip_index) NOEXCEPT
	{
		return _DepthMipChain[mip_index];
	}

private:

	//The depth mip chain.
	StaticArray<RenderTargetHandle, DEPTH_MIP_CHAIN_DEPTH> _DepthMipChain;

	//The depth downsample graphics pipelines.
	StaticArray<DepthDownsampleGraphicsPipeline, DEPTH_MIP_CHAIN_DEPTH> _DepthDownsampleGraphicsPipelines;

	//The scene features downsample graphics pipeline.
	GraphicsRenderPipeline _SceneFeaturesDownsampleGraphicsPipeline{ HashString("SceneFeaturesDownsample_RenderPipeline") };

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

	/*
	*	Terminates this render pass.
	*/
	void Terminate() NOEXCEPT;

};