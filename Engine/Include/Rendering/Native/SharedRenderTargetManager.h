#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class SharedRenderTargetManager final
{

public:

	/*
	*	Initializes the shared render target manager.
	*/
	void Initialize(const RenderingPath initial_rendering_path) NOEXCEPT;

	/*
	*	Called when switching rendering path.
	*/
	void OnSwitchRenderingPath(const RenderingPath new_rendering_path) NOEXCEPT;

	/*
	*	Returns the given shared render target.
	*/
	FORCE_INLINE NO_DISCARD RenderTargetHandle GetSharedRenderTarget(const SharedRenderTarget shared_render_target) NOEXCEPT
	{
		return _SharedRenderTargets[UNDERLYING(shared_render_target)];
	}

private:

	/*
	*	Shared render targets information class definition.
	*/
	class SharedRenderTargetInformation final
	{

	public:

		//The resolution.
		Resolution _Resolution;

		//The texture format.
		TextureFormat _TextureFormat;

		//The rendering paths that this shared render target is needed in.
		StaticArray<bool, UNDERLYING(RenderingPath::NUMBER_OF_RENDERING_PATHS)> _IsNeeded;

	};

	//The shared render targets.
	StaticArray<RenderTargetHandle, UNDERLYING(SharedRenderTarget::NUMBER_OF_SHARED_RENDER_TARGETS)> _SharedRenderTargets;

	//The shared render target informations.
	StaticArray<SharedRenderTargetInformation, UNDERLYING(SharedRenderTarget::NUMBER_OF_SHARED_RENDER_TARGETS)> _SharedRenderTargetInformations;

};