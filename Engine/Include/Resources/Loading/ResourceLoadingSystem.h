#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>

//Resources.
#include <Resources/Core/ResourcesCore.h>
#include <Resources/Loading/AnimatedModelData.h>
#include <Resources/Loading/AnimationData.h>
#include <Resources/Loading/RawDataData.h>
#include <Resources/Loading/RenderPipelineData.h>
#include <Resources/Loading/ShaderData.h>
#include <Resources/Loading/SoundData.h>
#include <Resources/Loading/Texture3DData.h>

//Sound.
#include <Sound/SoundCore.h>

//Forward declarations.
class BinaryInputFile;

class ResourceLoadingSystem final
{

public:

	/*
	*	Given a file, load animated model data.
	*/
	void LoadAnimatedModel(BinaryInputFile *const RESTRICT file, AnimatedModelData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load animation data.
	*/
	void LoadAnimation(BinaryInputFile *const RESTRICT file, AnimationData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load raw data data.
	*/
	void LoadRawData(BinaryInputFile *const RESTRICT file, RawDataData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load render pipeline data.
	*/
	void LoadRenderPipeline(BinaryInputFile *const RESTRICT file, RenderPipelineData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load shader data.
	*/
	void LoadShader(BinaryInputFile *const RESTRICT file, ShaderData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load sound data.
	*/
	void LoadSound(BinaryInputFile *const RESTRICT file, SoundData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load texture 3D data.
	*/
	void LoadTexture3D(BinaryInputFile *const RESTRICT file, Texture3DData *const RESTRICT data) NOEXCEPT;

};