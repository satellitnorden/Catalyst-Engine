#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class MediumDetailGrassVegetationColorRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(MediumDetailGrassVegetationColorRenderPass);

	/*
	*	Default constructor.
	*/
	MediumDetailGrassVegetationColorRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the medium detail grass vegetation color render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the color of the medium detail grass vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};