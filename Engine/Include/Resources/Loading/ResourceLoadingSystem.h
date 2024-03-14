#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>

//File.
#include <File/Core/BinaryFile.h>

//Resources.
#include <Resources/Core/ResourcesCore.h>
#include <Resources/Loading/AnimatedModelData.h>
#include <Resources/Loading/AnimationData.h>
#include <Resources/Loading/FontData.h>
#include <Resources/Loading/RawDataData.h>
#include <Resources/Loading/RenderPipelineData.h>
#include <Resources/Loading/ShaderData.h>
#include <Resources/Loading/SoundData.h>
#include <Resources/Loading/Texture3DData.h>
#include <Resources/Loading/VideoData.h>

//Sound.
#include <Sound/SoundCore.h>

class ResourceLoadingSystem final
{

public:

	/*
	*	Given a file, load animated model data.
	*/
	void LoadAnimatedModel(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, AnimatedModelData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load animation data.
	*/
	void LoadAnimation(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, AnimationData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load font data.
	*/
	void LoadFont(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, FontData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load raw data data.
	*/
	void LoadRawData(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, RawDataData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load render pipeline data.
	*/
	void LoadRenderPipeline(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, RenderPipelineData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load shader data.
	*/
	void LoadShader(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, ShaderData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load sound data.
	*/
	void LoadSound(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, SoundData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load texture 3D data.
	*/
	void LoadTexture3D(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, Texture3DData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load video data.
	*/
	void LoadVideo(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, VideoData *const RESTRICT data) NOEXCEPT;


};