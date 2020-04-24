#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/Map.h>
#include <Core/General/HashString.h>

//File.
#include <File/Core/BinaryFile.h>

//Rendering.
#include <Rendering/Native/Model.h>

//Resources.
#include <Resources/Core/ResourcesCore.h>
#include <Resources/Loading/AnimatedModelData.h>
#include <Resources/Loading/AnimationData.h>
#include <Resources/Loading/FontData.h>
#include <Resources/Loading/ShaderData.h>
#include <Resources/Loading/SoundData.h>
#include <Resources/Loading/Texture2DData.h>
#include <Resources/Loading/TextureCubeData.h>

//Sound.
#include <Sound/SoundCore.h>

class ResourceLoadingSystem final
{

public:

	/*
	*	Returns all model resources.
	*/
	FORCE_INLINE NO_DISCARD const Map<HashString, Model> &GetAllModelResources() NOEXCEPT
	{
		return _Models;
	}

	/*
	*	Given a resource ID, return the corresponding model resource.
	*/
	FORCE_INLINE NO_DISCARD const Model &GetModelResource(const HashString resource_ID) NOEXCEPT
	{
		return _Models[resource_ID];
	}

	/*
	*	Returns all texture 3D resources.
	*/
	FORCE_INLINE NO_DISCARD const Map<HashString, Texture3DHandle> &GetAllTexture3DResources() NOEXCEPT
	{
		return _Texture3Ds;
	}

	/*
	*	Given a resource ID, return the corresponding texture 3D resource.
	*/
	FORCE_INLINE NO_DISCARD const Texture3DHandle& GetTexture3DResource(const HashString resource_ID) NOEXCEPT
	{
		return _Texture3Ds[resource_ID];
	}

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
	*	Given a file, load a model.
	*/
	void LoadModel(BinaryFile<IOMode::In> &file) NOEXCEPT;

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
	*	Given a file, load a texture 3D.
	*/
	void LoadTexture3D(BinaryFile<IOMode::In>& file) NOEXCEPT;

	/*
	*	Given a file, load texture cube data.
	*/
	void LoadTextureCube(BinaryFile<IOMode::In> *const RESTRICT file, TextureCubeData *const RESTRICT data) NOEXCEPT;

private:

	//Container for all models.
	Map<HashString, Model> _Models;

	//Container for all texture 3Ds.
	Map<HashString, Texture3DHandle> _Texture3Ds;

};