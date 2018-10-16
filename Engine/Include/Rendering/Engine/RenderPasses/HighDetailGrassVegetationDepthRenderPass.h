#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class HighDetailGrassVegetationDepthRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(HighDetailGrassVegetationDepthRenderPass);

	/*
	*	Default constructor.
	*/
	HighDetailGrassVegetationDepthRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the high detail grass vegetation depth render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the high detail depth of the grass vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};