#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class AnimationSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(AnimationSystem);

	/*
	*	Default constructor.
	*/
	AnimationSystem() NOEXCEPT
	{

	}

	/*
	*	Post initializes the animation system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Updates the animation system during the render update phase.
	*/
	void RenderUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Returns the animation data render data table layout.
	*/
	FORCE_INLINE RenderDataTableLayoutHandle GetAnimationDataRenderDataTableLayout() const NOEXCEPT
	{
		return _AnimationDataRenderDataTableLayout;
	}

private:

	//The animation data render data table layout.
	RenderDataTableLayoutHandle _AnimationDataRenderDataTableLayout;

};