#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class MediumDetailSolidVegetationRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(MediumDetailSolidVegetationRenderPass);

	/*
	*	Default constructor.
	*/
	MediumDetailSolidVegetationRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the medium detail solid vegetation render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the medium detail solid vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};