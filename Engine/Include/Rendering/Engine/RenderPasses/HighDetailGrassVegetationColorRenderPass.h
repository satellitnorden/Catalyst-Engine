#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class HighDetailGrassVegetationColorRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(HighDetailGrassVegetationColorRenderPass);

	/*
	*	Default constructor.
	*/
	HighDetailGrassVegetationColorRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the high detail grass vegetation color render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the high detail color of the grass vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};