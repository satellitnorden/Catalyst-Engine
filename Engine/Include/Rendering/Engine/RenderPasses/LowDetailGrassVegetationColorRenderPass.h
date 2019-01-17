#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class LowDetailGrassVegetationColorRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(LowDetailGrassVegetationColorRenderPass);

	/*
	*	Default constructor.
	*/
	LowDetailGrassVegetationColorRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the low detail grass vegetation color render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the color of the low detail grass vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};