#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class LowDetailDebrisVegetationRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(LowDetailDebrisVegetationRenderPass);

	/*
	*	Default constructor.
	*/
	LowDetailDebrisVegetationRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the low detail debris vegetation render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the low detail debris vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};