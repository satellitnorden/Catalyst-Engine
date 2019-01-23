#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class TreeVegetationImpostorColorRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(TreeVegetationImpostorColorRenderPass);

	/*
	*	Default constructor.
	*/
	TreeVegetationImpostorColorRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the tree vegetation impostor color render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the tree vegetation impostor colors.
	*/
	void RenderInternal() NOEXCEPT;

};