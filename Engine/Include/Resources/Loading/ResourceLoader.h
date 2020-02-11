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
#include <Resources/Core/Texture2DResource.h>

//Sound.
#include <Sound/Native/SoundCore.h>

class ResourceLoader final
{

public:

	/*
	*	Given a file path, loads a resource collection.
	*/
	static void LoadResourceCollection(const char *RESTRICT filePath) NOEXCEPT;

	/*
	*	Returns all animated model resources.
	*/
	FORCE_INLINE static NO_DISCARD const Map<HashString, AnimatedModel> &GetAllAnimatedModelResources() NOEXCEPT
	{
		return _AnimatedModels;
	}

	/*
	*	Given a resource ID, return the corresponding animated model resource.
	*/
	FORCE_INLINE static NO_DISCARD const AnimatedModel &GetAnimatedModelResource(const HashString resource_ID) NOEXCEPT
	{
		return _AnimatedModels[resource_ID];
	}

	/*
	*	Returns all animation resources.
	*/
	FORCE_INLINE static NO_DISCARD const Map<HashString, Animation> &GetAllAnimationResources() NOEXCEPT
	{
		return _Animations;
	}

	/*
	*	Given a resource ID, return the corresponding animation resource.
	*/
	FORCE_INLINE static NO_DISCARD const Animation &GetAnimationResource(const HashString resource_ID) NOEXCEPT
	{
		return _Animations[resource_ID];
	}

	/*
	*	Returns all font resources.
	*/
	FORCE_INLINE static NO_DISCARD const Map<HashString, Font> &GetAllFontResources() NOEXCEPT
	{
		return _Fonts;
	}

	/*
	*	Given a resource ID, return the corresponding font resource.
	*/
	FORCE_INLINE static NO_DISCARD const Font &GetFontResource(const HashString resource_ID) NOEXCEPT
	{
		return _Fonts[resource_ID];
	}

	/*
	*	Returns all model resources.
	*/
	FORCE_INLINE static NO_DISCARD const Map<HashString, Model> &GetAllModelResources() NOEXCEPT
	{
		return _Models;
	}

	/*
	*	Given a resource ID, return the corresponding model resource.
	*/
	FORCE_INLINE static NO_DISCARD const Model &GetModelResource(const HashString resource_ID) NOEXCEPT
	{
		return _Models[resource_ID];
	}

	/*
	*	Returns all sound bank resources.
	*/
	FORCE_INLINE static NO_DISCARD const Map<HashString, SoundBankHandle>& GetAllSoundBankResources() NOEXCEPT
	{
		return _SoundBanks;
	}

	/*
	*	Given a resource ID, return the corresponding sound bank resource.
	*/
	FORCE_INLINE static NO_DISCARD const SoundBankHandle &GetSoundBankResource(const HashString resource_ID) NOEXCEPT
	{
		return _SoundBanks[resource_ID];
	}

	/*
	*	Returns all texture cube resources.
	*/
	FORCE_INLINE static NO_DISCARD const Map<HashString, TextureCubeHandle> &GetAllTextureCubeResources() NOEXCEPT
	{
		return _TextureCubes;
	}

	/*
	*	Given a resource ID, return the corresponding texture cube resource.
	*/
	FORCE_INLINE static NO_DISCARD const TextureCubeHandle &GetTextureCubeResource(const HashString resource_ID) NOEXCEPT
	{
		return _TextureCubes[resource_ID];
	}

	/*
	*	Returns all texture 2D resources.
	*/
	FORCE_INLINE static NO_DISCARD const Map<HashString, Texture2DResource> &GetAllTexture2DResources() NOEXCEPT
	{
		return _Texture2Ds;
	}

	/*
	*	Given a resource ID, return the corresponding texture 2D resource.
	*/
	FORCE_INLINE static NO_DISCARD const Texture2DResource &GetTexture2DResource(const HashString resource_ID) NOEXCEPT
	{
		return _Texture2Ds[resource_ID];
	}

	/*
	*	Returns all texture 3D resources.
	*/
	FORCE_INLINE static NO_DISCARD const Map<HashString, Texture3DHandle> &GetAllTexture3DResources() NOEXCEPT
	{
		return _Texture3Ds;
	}

	/*
	*	Given a resource ID, return the corresponding texture 3D resource.
	*/
	FORCE_INLINE static NO_DISCARD const Texture3DHandle& GetTexture3DResource(const HashString resource_ID) NOEXCEPT
	{
		return _Texture3Ds[resource_ID];
	}

private:

	//Container for all animated models.
	static Map<HashString, AnimatedModel> _AnimatedModels;

	//Container for all animations.
	static Map<HashString, Animation> _Animations;

	//Container for all fonts.
	static Map<HashString, Font> _Fonts;

	//Container for all models.
	static Map<HashString, Model> _Models;

	//Container for all sound banks.
	static Map<HashString, SoundBankHandle> _SoundBanks;

	//Container for all texture cubes.
	static Map<HashString, TextureCubeHandle> _TextureCubes;

	//Container for all texture 2Ds.
	static Map<HashString, Texture2DResource> _Texture2Ds;

	//Container for all texture 3Ds.
	static Map<HashString, Texture3DHandle> _Texture3Ds;

	/*
	*	Loads a resource collection, internal implementation.
	*/
	static void LoadResourceCollectionInternal(const char *RESTRICT filePath) NOEXCEPT;

	/*
	*	Given a file, load an animated model.
	*/
	static void LoadAnimatedModel(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load an animation.
	*/
	static void LoadAnimation(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a model.
	*/
	static void LoadFont(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a model.
	*/
	static void LoadModel(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a sound bank.
	*/
	static void LoadSoundBank(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a texture cube.
	*/
	static void LoadTextureCube(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a texture 2D.
	*/
	static void LoadTexture2D(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a texture 3D.
	*/
	static void LoadTexture3D(BinaryFile<IOMode::In>& file) NOEXCEPT;

};