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
	*	Registers a render pass for the PATH_TRACING rendering path.
	*/
	FORCE_INLINE static void RegisterPathTracingRenderPass(const PathTracingNativeRenderPassStage stage, RenderPass *const RESTRICT render_pass) NOEXCEPT
	{
		GetPathTracingRenderPasses()->At(UNDERLYING(stage)) = render_pass;
	}

	/*
	*	Registers a render pass for the MOBILE rendering path.
	*/
	FORCE_INLINE static void RegisterMobileRenderPass(const MobileNativeRenderPassStage stage, RenderPass *const RESTRICT render_pass) NOEXCEPT
	{
		GetMobileRenderPasses()->At(UNDERLYING(stage)) = render_pass;
	}

	/*
	*	Registers a render pass for the VIRTUAL_REALITY rendering path.
	*/
	FORCE_INLINE static void RegisterVirtualRealityRenderPass(const VirtualRealityNativeRenderPassStage stage, RenderPass *const RESTRICT render_pass) NOEXCEPT
	{
		GetVirtualRealityRenderPasses()->At(UNDERLYING(stage)) = render_pass;
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

			case RenderingPath::PATH_TRACING:
			{
				render_passes->Reserve(UNDERLYING(PathTracingNativeRenderPassStage::NUMBER_OF_STAGES));

				for (RenderPass *const RESTRICT render_pass : *GetPathTracingRenderPasses())
				{
					render_passes->Emplace(render_pass);
				}

				break;
			}

			case RenderingPath::MOBILE:
			{
				render_passes->Reserve(UNDERLYING(MobileNativeRenderPassStage::NUMBER_OF_STAGES));

				for (RenderPass *const RESTRICT render_pass : *GetMobileRenderPasses())
				{
					render_passes->Emplace(render_pass);
				}

				break;
			}

			case RenderingPath::VIRTUAL_REALITY:
			{
				render_passes->Reserve(UNDERLYING(VirtualRealityNativeRenderPassStage::NUMBER_OF_STAGES));

				for (RenderPass *const RESTRICT render_pass : *GetVirtualRealityRenderPasses())
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

	/*
	*	Returns the render passes for the PATH_TRACING rendering path.
	*/
	FORCE_INLINE NO_DISCARD static StaticArray<RenderPass *RESTRICT, UNDERLYING(PathTracingNativeRenderPassStage::NUMBER_OF_STAGES)> *const RESTRICT GetPathTracingRenderPasses() NOEXCEPT
	{
		static StaticArray<RenderPass *RESTRICT, UNDERLYING(PathTracingNativeRenderPassStage::NUMBER_OF_STAGES)> render_passes;

		return &render_passes;
	}

	/*
	*	Returns the render passes for the MOBILE rendering path.
	*/
	FORCE_INLINE NO_DISCARD static StaticArray<RenderPass *RESTRICT, UNDERLYING(MobileNativeRenderPassStage::NUMBER_OF_STAGES)> *const RESTRICT GetMobileRenderPasses() NOEXCEPT
	{
		static StaticArray<RenderPass *RESTRICT, UNDERLYING(MobileNativeRenderPassStage::NUMBER_OF_STAGES)> render_passes;

		return &render_passes;
	}


	/*
	*	Returns the render passes for the VIRTUAL_REALITY rendering path.
	*/
	FORCE_INLINE NO_DISCARD static StaticArray<RenderPass *RESTRICT, UNDERLYING(VirtualRealityNativeRenderPassStage::NUMBER_OF_STAGES)> *const RESTRICT GetVirtualRealityRenderPasses() NOEXCEPT
	{
		static StaticArray<RenderPass *RESTRICT, UNDERLYING(VirtualRealityNativeRenderPassStage::NUMBER_OF_STAGES)> render_passes;

		return &render_passes;
	}

};