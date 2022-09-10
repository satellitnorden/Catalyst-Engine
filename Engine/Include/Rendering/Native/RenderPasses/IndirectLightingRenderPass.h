#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/IndirectLightingApplicationGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/IndirectLightingTemporalDenoisingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ResampleGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ScreenSpaceIndirectLightingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ScreenSpaceIndirectLightingResolveGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/RayTracingPipelines/RayTracedIndirectLightingRayTracingPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ALIGN(8) IndirectLightingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(IndirectLightingRenderPass);

	//Denotes the size of the previous scene mip chain.
	constexpr static uint64 PREVIOUS_SCENE_MIP_CHAIN_SIZE{ 8 };

	/*
	*	Default constructor.
	*/
	IndirectLightingRenderPass() NOEXCEPT;

	/*
	*	Returns the previous scene mip chain.
	*/
	FORCE_INLINE NO_DISCARD RenderTargetHandle GetPreviousSceneMip(const uint8 index) const NOEXCEPT
	{
		return _PreviousSceneMipChain[index];
	}

	/*
	*	Returns the specular bias lookup texture index.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetSpecularBiasLookupTextureIndex() const NOEXCEPT
	{
		return _SpecularBiasLookupTextureIndex;
	}

private:

	//The previous indirect lighting mode.
	RenderingConfiguration::IndirectLightingMode _PreviousIndirectLightingMode{ RenderingConfiguration::IndirectLightingMode::NONE };

	//The current indirect lighting mode.
	RenderingConfiguration::IndirectLightingMode _CurrentIndirectLightingMode{ RenderingConfiguration::IndirectLightingMode::NONE };

	//The previous indirect lighting quality.
	RenderingConfiguration::IndirectLightingQuality _PreviousIndirectLightingQuality{ RenderingConfiguration::IndirectLightingQuality::LOW };

	//The current indirect lighting quality.
	RenderingConfiguration::IndirectLightingQuality _CurrentIndirectLightingQuality{ RenderingConfiguration::IndirectLightingQuality::LOW };

	//The previous scene mip chain.
	StaticArray<RenderTargetHandle, PREVIOUS_SCENE_MIP_CHAIN_SIZE> _PreviousSceneMipChain;

	//The temporal reprojection buffer.
	RenderTargetHandle _TemporalReprojectionBuffer;

	//The temporal indirect lighting buffers.
	StaticArray<RenderTargetHandle, 2> _TemporalIndirectLightingBuffers;

	//The specular bias lookup texture.
	Texture2DHandle _SpecularBiasLookupTexture{ EMPTY_HANDLE };

	//The specular bias lookup texture index.
	uint32 _SpecularBiasLookupTextureIndex{ UINT32_MAXIMUM };

	//The screen space indirect lighting graphics pipeline.
	ScreenSpaceIndirectLightingGraphicsPipeline _ScreenSpaceIndirectLightingGraphicsPipeline;

	//The previous scene resample graphics pipelines.
	StaticArray<ResampleGraphicsPipeline, PREVIOUS_SCENE_MIP_CHAIN_SIZE - 1> _PreviousSceneResampleGraphicsPipelines;

	//The screen space indirect lighting resolve graphics pipeline.
	ScreenSpaceIndirectLightingResolveGraphicsPipeline _ScreenSpaceIndirectLightingResolveGraphicsPipeline;

	//The ray traced indirect lighting ray tracing pipeline.
	RayTracedIndirectLightingRayTracingPipeline _RayTracedIndirectLightingRayTracingPipeline;

	//The indirect lighting temporal denoising graphics pipelines.
	StaticArray<IndirectLightingTemporalDenoisingGraphicsPipeline, 2> _IndirectLightingTemporalDenoisingGraphicsPipelines;

	//The current temporal buffer index.
	uint8 _CurrentTemporalBufferIndex{ 0 };

	//The indirect lighting application graphics pipeline.
	IndirectLightingApplicationGraphicsPipeline _IndirectLightingApplicationGraphicsPipeline;

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