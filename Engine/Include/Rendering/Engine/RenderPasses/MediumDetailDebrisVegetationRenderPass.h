#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class MediumDetailDebrisVegetationRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(MediumDetailDebrisVegetationRenderPass);

	/*
	*	Default constructor.
	*/
	MediumDetailDebrisVegetationRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the medium detail debris vegetation render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the medium detail debris vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};