#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class VegetationColorRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(VegetationColorRenderPass);

	/*
	*	Default constructor.
	*/
	VegetationColorRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the vegetation color render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the color of the vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};