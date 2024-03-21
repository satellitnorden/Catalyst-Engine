#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Building/AnimatedModelBuildParameters.h>
#include <Resources/Building/AnimationBuildParameters.h>
#include <Resources/Building/RawDataBuildParameters.h>
#include <Resources/Building/RenderPipelineBuildParameters.h>
#include <Resources/Building/ResourceCollectionBuildParameters.h>
#include <Resources/Building/ShaderBuildParameters.h>
#include <Resources/Building/SoundBuildParameters.h>
#include <Resources/Building/Texture3DBuildParameters.h>
#include <Resources/Building/VideoBuildParameters.h>

class ResourceBuildingSystem final
{

public:

	/*
	*	Builds resource collections.
	*/
	void BuildResourceCollections(const ResourceCollectionBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds an animated model.
	*/
	void BuildAnimatedModel(const AnimatedModelBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds an animation.
	*/
	void BuildAnimation(const AnimationBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a raw data.
	*/
	void BuildRawData(const RawDataBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a render pipeline.
	*/
	void BuildRenderPipeline(const RenderPipelineBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a shader.
	*/
	void BuildShader(const ShaderBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a sound.
	*/
	void BuildSound(const SoundBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a texture 3D.
	*/
	void BuildTexture3D(const Texture3DBuildParameters &parameters) NOEXCEPT;

	/*
	*	Builds a video.
	*/
	void BuildVideo(const VideoBuildParameters &parameters) NOEXCEPT;

};
#endif