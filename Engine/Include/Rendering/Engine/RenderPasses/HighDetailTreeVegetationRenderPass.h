#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class HighDetailTreeVegetationRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(HighDetailTreeVegetationRenderPass);

	/*
	*	Default constructor.
	*/
	HighDetailTreeVegetationRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the high detail tree vegetation render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the high detail tree vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};