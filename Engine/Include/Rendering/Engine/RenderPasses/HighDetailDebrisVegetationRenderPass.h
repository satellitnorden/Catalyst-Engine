#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class HighDetailDebrisVegetationRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(HighDetailDebrisVegetationRenderPass);

	/*
	*	Default constructor.
	*/
	HighDetailDebrisVegetationRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the high detail debris vegetation render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the high detail debris vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};