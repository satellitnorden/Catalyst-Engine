#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class LowDetailSolidVegetationRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(LowDetailSolidVegetationRenderPass);

	/*
	*	Default constructor.
	*/
	LowDetailSolidVegetationRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the low detail solid vegetation render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the low detail solid vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};