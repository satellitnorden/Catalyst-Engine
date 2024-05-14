#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class NativeRenderPassManager final
{

public:

	/*
	*	Registers a render pass for the DEFAULT rendering path.
	*/
	FORCE_INLINE static void RegisterDefaultRenderPass(const DefaultNativeRenderPassStage stage, RenderPass *const RESTRICT render_pass) NOEXCEPT
	{
		GetDefaultRenderPasses()->At(UNDERLYING(stage)) = render_pass;

		render_pass->SetDefaultNativeStage(stage);
	}

	/*
	*	Returns the render passes for the given rendering path.
	*/
	FORCE_INLINE static void GetRenderPasses(const RenderingPath rendering_path, DynamicArray<RenderPass *RESTRICT> *const RESTRICT render_passes) NOEXCEPT
	{
		render_passes->Clear();

		switch (rendering_path)
		{
			case RenderingPath::NONE:
			{
				//Do nothing. (:

				break;
			}

			case RenderingPath::DEFAULT:
			{
				render_passes->Reserve(UNDERLYING(DefaultNativeRenderPassStage::NUMBER_OF_STAGES));

				for (RenderPass *const RESTRICT render_pass : *GetDefaultRenderPasses())
				{
					render_passes->Emplace(render_pass);
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

private:

	/*
	*	Returns the render passes for the DEFAULT rendering path.
	*/
	FORCE_INLINE NO_DISCARD static StaticArray<RenderPass *RESTRICT, UNDERLYING(DefaultNativeRenderPassStage::NUMBER_OF_STAGES)> *const RESTRICT GetDefaultRenderPasses() NOEXCEPT
	{
		static StaticArray<RenderPass *RESTRICT, UNDERLYING(DefaultNativeRenderPassStage::NUMBER_OF_STAGES)> render_passes;

		return &render_passes;
	}

};