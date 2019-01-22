#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class TreeVegetationImpostorRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(TreeVegetationImpostorRenderPass);

	/*
	*	Default constructor.
	*/
	TreeVegetationImpostorRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the tree vegetation impostor render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the tree vegetation impostors.
	*/
	void RenderInternal() NOEXCEPT;

};