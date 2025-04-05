#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/TextureCube.h>

//Resources.
#include <Resources/Core/RawDataResource.h>
#include <Resources/Core/RenderPipelineResource.h>
#include <Resources/Core/ShaderResource.h>
#include <Resources/Core/Texture3DResource.h>
#include <Resources/Loading/RawDataData.h>
#include <Resources/Loading/RenderPipelineData.h>
#include <Resources/Loading/ShaderData.h>
#include <Resources/Loading/Texture3DData.h>

class ResourceCreationSystem final
{

public:

	/*
	*	Creates a raw data.
	*/
	void CreateRawData(RawDataData *const RESTRICT data, RawDataResource *const RESTRICT resource) NOEXCEPT;

	/*
	*	Creates a render pipeline.
	*/
	void CreateRenderPipeline(RenderPipelineData *const RESTRICT data, RenderPipelineResource *const RESTRICT resource) NOEXCEPT;

	/*
	*	Creates a shader.
	*/
	void CreateShader(ShaderData *const RESTRICT data, ShaderResource *const RESTRICT resource) NOEXCEPT;

	/*
	*	Creates a texture 3D.
	*/
	void CreateTexture3D(Texture3DData *const RESTRICT data, Texture3DResource *const RESTRICT resource) NOEXCEPT;

};