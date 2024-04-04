#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/HashString.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Resolution.h>

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
		return _SharedRenderTargetInformations[UNDERLYING(shared_render_target)]._Handle;
	}

	/*
	*	Returns the shared render target with the given identifier..
	*/
	NO_DISCARD RenderTargetHandle GetSharedRenderTarget(const HashString identifier) NOEXCEPT;

private:

	/*
	*	Shared render targets information class definition.
	*/
	class SharedRenderTargetInformation final
	{

	public:

		//The identifier.
		HashString _Identifier;

		//The resolution.
		Resolution _Resolution;

		//The texture format.
		TextureFormat _TextureFormat;

		//The rendering paths that this shared render target is needed in.
		StaticArray<bool, UNDERLYING(RenderingPath::NUMBER_OF_RENDERING_PATHS)> _IsNeeded;

		//Denotes if this is a depth buffer.
		bool _IsDepthBuffer;

		//The handle.
		RenderTargetHandle _Handle;

	};

	//The shared render target informations.
	StaticArray<SharedRenderTargetInformation, UNDERLYING(SharedRenderTarget::NUMBER_OF_SHARED_RENDER_TARGETS)> _SharedRenderTargetInformations;

};