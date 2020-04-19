#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Font.h>
#include <Rendering/Native/Model.h>
#include <Rendering/Native/TextureCube.h>

//Resources.
#include <Resources/Core/AnimatedModelResource.h>
#include <Resources/Core/ShaderResource.h>
#include <Resources/Core/SoundResource.h>
#include <Resources/Core/Texture2DResource.h>
#include <Resources/Loading/AnimatedModelData.h>
#include <Resources/Loading/AnimationData.h>
#include <Resources/Loading/FontData.h>
#include <Resources/Loading/ModelData.h>
#include <Resources/Loading/ShaderData.h>
#include <Resources/Loading/SoundData.h>
#include <Resources/Loading/TextureCubeData.h>
#include <Resources/Loading/Texture2DData.h>
#include <Resources/Loading/Texture3DData.h>

class ResourceCreationSystem final
{

public:

	/*
	*	Creates an animated model.
	*/
	void CreateAnimatedModel(AnimatedModelData *const RESTRICT data, AnimatedModelResource *const RESTRICT resource) NOEXCEPT;

	/*
	*	Creates an animation
	*/
	void CreateAnimation(AnimationData *const RESTRICT data, Animation *const RESTRICT animation) NOEXCEPT;

	/*
	*	Creates a font.
	*/
	void CreateFont(FontData *const RESTRICT data, Font *const RESTRICT font) NOEXCEPT;

	/*
	*	Creates a model.
	*/
	void CreateModel(ModelData *const RESTRICT data, Model *const RESTRICT model) NOEXCEPT;

	/*
	*	Creates a shader.
	*/
	void CreateShader(ShaderData *const RESTRICT data, ShaderResource *const RESTRICT resource) NOEXCEPT;

	/*
	*	Creates a sound.
	*/
	void CreateSound(SoundData *const RESTRICT data, SoundResource *const RESTRICT resource) NOEXCEPT;

	/*
	*	Creates a texture cube.
	*/
	void CreateTextureCube(TextureCubeData *const RESTRICT data, TextureCubeHandle *const RESTRICT texture) NOEXCEPT;

	/*
	*	Creates a texture 2D.
	*/
	void CreateTexture2D(Texture2DData *const RESTRICT data, Texture2DResource *const RESTRICT resource) NOEXCEPT;

	/*
	*	Creates a texture 3D.
	*/
	void CreateTexture3D(Texture3DData* const RESTRICT data, Texture3DHandle* const RESTRICT texture) NOEXCEPT;

};