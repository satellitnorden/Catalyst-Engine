#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/TextureCube.h>

//Resources.
#include <Resources/Core/AnimatedModelResource.h>
#include <Resources/Core/AnimationResource.h>
#include <Resources/Core/RawDataResource.h>
#include <Resources/Core/RenderPipelineResource.h>
#include <Resources/Core/ShaderResource.h>
#include <Resources/Core/SoundResource.h>
#include <Resources/Core/Texture3DResource.h>
#include <Resources/Loading/AnimatedModelData.h>
#include <Resources/Loading/AnimationData.h>
#include <Resources/Loading/RawDataData.h>
#include <Resources/Loading/RenderPipelineData.h>
#include <Resources/Loading/ShaderData.h>
#include <Resources/Loading/SoundData.h>
#include <Resources/Loading/Texture3DData.h>

class ResourceCreationSystem final
{

public:

	/*
	*	Creates an animated model.
	*/
	void CreateAnimatedModel(AnimatedModelData *const RESTRICT data, AnimatedModelResource *const RESTRICT resource) NOEXCEPT;

	/*
	*	Creates an animation.
	*/
	void CreateAnimation(AnimationData *const RESTRICT data, AnimationResource *const RESTRICT resource) NOEXCEPT;

	/*
	*	Creates a raw data.
	*/
	void CreateRawData(RawDataData *const RESTRICT data, RawDataResource *const RESTRICT resource) NOEXCEPT;

	/*
	*	Creates a render pipeline.
	*/
	void CreateRenderPipeline(RenderPipelineData *const RESTRICT data, RenderPipelineResource *const RESTRICT resource) NOEXCEPT;

	/*
	*	Creates a shader.
	*/
	void CreateShader(ShaderData *const RESTRICT data, ShaderResource *const RESTRICT resource) NOEXCEPT;

	/*
	*	Creates a sound.
	*/
	void CreateSound(SoundData *const RESTRICT data, SoundResource *const RESTRICT resource) NOEXCEPT;

	/*
	*	Creates a texture 3D.
	*/
	void CreateTexture3D(Texture3DData *const RESTRICT data, Texture3DResource *const RESTRICT resource) NOEXCEPT;

};