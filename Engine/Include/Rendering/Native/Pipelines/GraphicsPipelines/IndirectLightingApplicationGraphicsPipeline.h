#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class IndirectLightingApplicationGraphicsPipeline final : public GraphicsPipeline
{

public:

	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute() NOEXCEPT;

private:

	//The specular bias lookup texture.
	Texture2DHandle _SpecularBiasLookupTexture;

	//The specular bias lookup texture index.
	uint32 _SpecularBiasLookupTextureIndex;

	/*
	*	Creates the specular bias lookup texture.
	*/
	void CreateSpecularBiasLookupTexture() NOEXCEPT;

};