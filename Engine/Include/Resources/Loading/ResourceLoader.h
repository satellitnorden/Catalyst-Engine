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
	*	Given a resource ID, return the corresponding animated model.
	*/
	static const AnimatedModel& GetAnimatedModel(const HashString resourceID) { return _AnimatedModels[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding animation.
	*/
	static const Animation& GetAnimation(const HashString resourceID) { return _Animations[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding font.
	*/
	static const Font& GetFont(const HashString resourceID) { return _Fonts[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding model.
	*/
	static const Model& GetModel(const HashString resourceID) { return _Models[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding sound bank.
	*/
	static const SoundBankHandle& GetSoundBank(const HashString resourceID) { return _SoundBanks[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding texture cube.
	*/
	static const TextureCubeHandle& GetTextureCube(const HashString resourceID) { return _TextureCubes[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding texture 2D.
	*/
	static const Texture2DResource& GetTexture2D(const HashString resourceID) { return _Texture2Ds[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding texture 3D.
	*/
	static const Texture3DHandle& GetTexture3D(const HashString resourceID) { return _Texture3Ds[resourceID]; }

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