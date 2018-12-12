#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class GrassVegetationColorRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(GrassVegetationColorRenderPass);

	/*
	*	Default constructor.
	*/
	GrassVegetationColorRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the grass vegetation color render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the color of the grass vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};