#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Building/AnimatedModelBuildParameters.h>
#include <Resources/Building/AnimationBuildParameters.h>
#include <Resources/Building/FontBuildParameters.h>
#include <Resources/Building/MaterialBuildParameters.h>
#include <Resources/Building/ModelBuildParameters.h>
#include <Resources/Building/ResourceCollectionBuildParameters.h>
#include <Resources/Building/ShaderBuildParameters.h>
#include <Resources/Building/SoundBuildParameters.h>
#include <Resources/Building/Texture2DBuildParameters.h>
#include <Resources/Building/Texture3DBuildParameters.h>
#include <Resources/Building/TextureCubeBuildParameters.h>

class ResourceBuildingSystem final
{

public:

	/*
	*	Builds a resource collection.
	*/
	void BuildResourceCollection(const ResourceCollectionBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds an animated model.
	*/
	void BuildAnimatedModel(const AnimatedModelBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds an animation.
	*/
	void BuildAnimation(const AnimationBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a font.
	*/
	void BuildFont(const FontBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a material.
	*/
	void BuildMaterial(const MaterialBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a model.
	*/
	void BuildModel(const ModelBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a shader.
	*/
	void BuildShader(const ShaderBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a sound.
	*/
	void BuildSound(const SoundBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a texture cube
	*/
	void BuildTextureCube(const TextureCubeBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a texture 2D.
	*/
	void BuildTexture2D(const Texture2DBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a texture 3D.
	*/
	void BuildTexture3D(const Texture3DBuildParameters& parameters) NOEXCEPT;

};
#endif