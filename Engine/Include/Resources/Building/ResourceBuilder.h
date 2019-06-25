#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Building/FontBuildParameters.h>
#include <Resources/Building/ModelBuildParameters.h>
#include <Resources/Building/ResourceCollectionBuildParameters.h>
#include <Resources/Building/SoundBankBuildParameters.h>
#include <Resources/Building/TextureCubeBuildParameters.h>
#include <Resources/Building/Texture2DBuildParameters.h>

class ResourceBuilder final
{

public:

	/*
	*	Builds a resource collection.
	*/
	static void BuildResourceCollection(const ResourceCollectionBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a font.
	*/
	static void BuildFont(const FontBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a model.
	*/
	static void BuildModel(const ModelBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a sound bank.
	*/
	static void BuildSoundBank(const SoundBankBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a texture cube
	*/
	static void BuildTextureCube(const TextureCubeBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a texture 2D.
	*/
	static void BuildTexture2D(const Texture2DBuildParameters &parameters) NOEXCEPT;

};
#endif