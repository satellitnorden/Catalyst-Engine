#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class HighDetailTreeVegetationImpostorDepthRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(HighDetailTreeVegetationImpostorDepthRenderPass);

	/*
	*	Default constructor.
	*/
	HighDetailTreeVegetationImpostorDepthRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the high detail tree vegetation impostor depth render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the high detail tree vegetation impostor depths.
	*/
	void RenderInternal() NOEXCEPT;

};