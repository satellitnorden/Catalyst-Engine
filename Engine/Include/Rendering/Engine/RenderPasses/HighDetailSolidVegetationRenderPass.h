#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class HighDetailSolidVegetationRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(HighDetailSolidVegetationRenderPass);

	/*
	*	Default constructor.
	*/
	HighDetailSolidVegetationRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the high detail solid vegetation render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the high detail solid vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};