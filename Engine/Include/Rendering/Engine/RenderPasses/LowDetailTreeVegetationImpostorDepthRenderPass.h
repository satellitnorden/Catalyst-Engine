#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class LowDetailTreeVegetationImpostorDepthRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(LowDetailTreeVegetationImpostorDepthRenderPass);

	/*
	*	Default constructor.
	*/
	LowDetailTreeVegetationImpostorDepthRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the low detail tree vegetation impostor depth render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the low detail tree vegetation impostor depths.
	*/
	void RenderInternal() NOEXCEPT;

};