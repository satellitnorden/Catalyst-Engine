#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/Map.h>
#include <Core/General/BinaryFile.h>
#include <Core/General/HashString.h>

//Rendering.
#include <Rendering/Engine/Model.h>
#include <Rendering/Engine/TextureCube.h>
#include <Rendering/Engine/Texture2D.h>

//Resources.
#include <Resources/Core/ResourcesCore.h>

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
	static const TextureCube& GetTextureCube(const HashString resourceID) { return _TextureCubes[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding texture 2D.
	*/
	static const Texture2D<Vector4<byte>>& GetTexture2D(const HashString resourceID) { return _Texture2Ds[resourceID]; }

private:

	//Container for all models.
	static Map<HashString, Model> _Models;

	//Container for all sound banks.
	static Map<HashString, SoundBankHandle> _SoundBanks;

	//Container for all texture cubes.
	static Map<HashString, TextureCube> _TextureCubes;

	//Container for all texture 2Ds.
	static Map<HashString, Texture2D<Vector4<byte>>> _Texture2Ds;

	/*
	*	Loads a resource collection, internal implementation.
	*/
	static void LoadResourceCollectionInternal(const char *RESTRICT filePath) NOEXCEPT;

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

};