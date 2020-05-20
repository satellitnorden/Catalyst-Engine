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
#include <Resources/Loading/MaterialData.h>
#include <Resources/Loading/ModelData.h>
#include <Resources/Loading/ShaderData.h>
#include <Resources/Loading/SoundData.h>
#include <Resources/Loading/Texture2DData.h>
#include <Resources/Loading/Texture3DData.h>
#include <Resources/Loading/TextureCubeData.h>

//Sound.
#include <Sound/SoundCore.h>

class ResourceLoadingSystem final
{

public:

	/*
	*	Given a file, load animated model data.
	*/
	void LoadAnimatedModel(BinaryFile<IOMode::In> *const RESTRICT file, AnimatedModelData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load animation data.
	*/
	void LoadAnimation(BinaryFile<IOMode::In> *const RESTRICT file, AnimationData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load font data.
	*/
	void LoadFont(BinaryFile<IOMode::In> *const RESTRICT file, FontData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load material data.
	*/
	void LoadMaterial(BinaryFile<IOMode::In> *const RESTRICT file, MaterialData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load model data.
	*/
	void LoadModel(BinaryFile<IOMode::In> *const RESTRICT file, ModelData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load shader data.
	*/
	void LoadShader(BinaryFile<IOMode::In> *const RESTRICT file, ShaderData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load sound data.
	*/
	void LoadSound(BinaryFile<IOMode::In> *const RESTRICT file, SoundData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load texture 2D data.
	*/
	void LoadTexture2D(BinaryFile<IOMode::In> *const RESTRICT file, Texture2DData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load texture 3D data.
	*/
	void LoadTexture3D(BinaryFile<IOMode::In> *const RESTRICT file, Texture3DData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load texture cube data.
	*/
	void LoadTextureCube(BinaryFile<IOMode::In> *const RESTRICT file, TextureCubeData *const RESTRICT data) NOEXCEPT;


};