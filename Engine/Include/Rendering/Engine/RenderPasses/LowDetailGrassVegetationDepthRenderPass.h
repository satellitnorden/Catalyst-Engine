#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class LowDetailGrassVegetationDepthRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(LowDetailGrassVegetationDepthRenderPass);

	/*
	*	Default constructor.
	*/
	LowDetailGrassVegetationDepthRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the low detail grass vegetation depth render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the depth of the low detail grass vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};