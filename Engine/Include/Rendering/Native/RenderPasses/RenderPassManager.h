#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class RenderPassManager final
{

public:

	/*
	*	Returns the render passes.
	*/
	FORCE_INLINE NO_DISCARD static StaticArray<RenderPass *RESTRICT, UNDERLYING(RenderPassStage::NumberOfRenderPassStages)> *const RESTRICT GetRenderPasses() NOEXCEPT
	{
		static StaticArray<RenderPass *RESTRICT, UNDERLYING(RenderPassStage::NumberOfRenderPassStages)> renderPasses;

		return &renderPasses;
	}

	/*
	*	Registers a render pass.
	*/
	FORCE_INLINE static void RegisterRenderPass(RenderPass *const RESTRICT renderPass) NOEXCEPT
	{
		GetRenderPasses()->At(UNDERLYING(renderPass->GetStage())) = renderPass;
	}

};