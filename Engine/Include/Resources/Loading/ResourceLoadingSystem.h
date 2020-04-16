#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/Map.h>
#include <Core/General/HashString.h>

//Animation.
#include <Animation/AnimatedModel.h>
#include <Animation/Animation.h>

//File.
#include <File/Core/BinaryFile.h>

//Rendering.
#include <Rendering/Native/Font.h>
#include <Rendering/Native/Model.h>

//Resources.
#include <Resources/Core/ResourcesCore.h>
#include <Resources/Loading/SoundData.h>
#include <Resources/Loading/Texture2DData.h>

//Sound.
#include <Sound/Native/SoundCore.h>

class ResourceLoadingSystem final
{

public:

	/*
	*	Returns all animated model resources.
	*/
	FORCE_INLINE NO_DISCARD const Map<HashString, AnimatedModel> &GetAllAnimatedModelResources() NOEXCEPT
	{
		return _AnimatedModels;
	}

	/*
	*	Given a resource ID, return the corresponding animated model resource.
	*/
	FORCE_INLINE NO_DISCARD const AnimatedModel &GetAnimatedModelResource(const HashString resource_ID) NOEXCEPT
	{
		return _AnimatedModels[resource_ID];
	}

	/*
	*	Returns all animation resources.
	*/
	FORCE_INLINE NO_DISCARD const Map<HashString, Animation> &GetAllAnimationResources() NOEXCEPT
	{
		return _Animations;
	}

	/*
	*	Given a resource ID, return the corresponding animation resource.
	*/
	FORCE_INLINE NO_DISCARD const Animation &GetAnimationResource(const HashString resource_ID) NOEXCEPT
	{
		return _Animations[resource_ID];
	}

	/*
	*	Returns all font resources.
	*/
	FORCE_INLINE NO_DISCARD const Map<HashString, Font> &GetAllFontResources() NOEXCEPT
	{
		return _Fonts;
	}

	/*
	*	Given a resource ID, return the corresponding font resource.
	*/
	FORCE_INLINE NO_DISCARD const Font &GetFontResource(const HashString resource_ID) NOEXCEPT
	{
		return _Fonts[resource_ID];
	}

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
	*	Returns all texture cube resources.
	*/
	FORCE_INLINE NO_DISCARD const Map<HashString, TextureCubeHandle> &GetAllTextureCubeResources() NOEXCEPT
	{
		return _TextureCubes;
	}

	/*
	*	Given a resource ID, return the corresponding texture cube resource.
	*/
	FORCE_INLINE NO_DISCARD const TextureCubeHandle &GetTextureCubeResource(const HashString resource_ID) NOEXCEPT
	{
		return _TextureCubes[resource_ID];
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
	*	Given a file, load an animated model.
	*/
	void LoadAnimatedModel(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load an animation.
	*/
	void LoadAnimation(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a model.
	*/
	void LoadFont(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a model.
	*/
	void LoadModel(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load sound data.
	*/
	void LoadSound(BinaryFile<IOMode::In> *const RESTRICT file, SoundData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load a texture cube.
	*/
	void LoadTextureCube(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load texture 2D data.
	*/
	void LoadTexture2D(BinaryFile<IOMode::In> *const RESTRICT file, Texture2DData *const RESTRICT data) NOEXCEPT;

	/*
	*	Given a file, load a texture 3D.
	*/
	void LoadTexture3D(BinaryFile<IOMode::In>& file) NOEXCEPT;

private:

	//Container for all animated models.
	Map<HashString, AnimatedModel> _AnimatedModels;

	//Container for all animations.
	Map<HashString, Animation> _Animations;

	//Container for all fonts.
	Map<HashString, Font> _Fonts;

	//Container for all models.
	Map<HashString, Model> _Models;

	//Container for all texture cubes.
	Map<HashString, TextureCubeHandle> _TextureCubes;

	//Container for all texture 3Ds.
	Map<HashString, Texture3DHandle> _Texture3Ds;

};