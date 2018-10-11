#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class SolidVegetationRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(SolidVegetationRenderPass);

	/*
	*	Default constructor.
	*/
	SolidVegetationRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the solid vegetation render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the solid vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};