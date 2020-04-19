#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>
#include <Core/General/UpdateContext.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Forward declarations.
class AnimatedModelComponent;
class Bone;

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

	/*
	*	Updates an animated model.
	*/
	void UpdateAnimatedModel(const UpdateContext *const RESTRICT context, AnimatedModelComponent *const RESTRICT component) NOEXCEPT;

	/*
	*	Finds the bone index of the animated model with the given name.
	*/
	bool FindBoneIndex(const Bone* const RESTRICT bone, const HashString name, uint32 *const RESTRICT bone_index) NOEXCEPT;

};