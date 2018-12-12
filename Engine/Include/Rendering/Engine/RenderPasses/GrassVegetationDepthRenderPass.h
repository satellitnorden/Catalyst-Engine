#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class GrassVegetationDepthRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(GrassVegetationDepthRenderPass);

	/*
	*	Default constructor.
	*/
	GrassVegetationDepthRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the grass vegetation depth render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the depth of the grass vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};