#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class TreeVegetationImpostorDepthRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(TreeVegetationImpostorDepthRenderPass);

	/*
	*	Default constructor.
	*/
	TreeVegetationImpostorDepthRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the tree vegetation impostor depth render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the tree vegetation impostor depths.
	*/
	void RenderInternal() NOEXCEPT;

};