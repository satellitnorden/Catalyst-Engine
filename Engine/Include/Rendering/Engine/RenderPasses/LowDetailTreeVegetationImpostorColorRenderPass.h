#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class LowDetailTreeVegetationImpostorColorRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(LowDetailTreeVegetationImpostorColorRenderPass);

	/*
	*	Default constructor.
	*/
	LowDetailTreeVegetationImpostorColorRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the low detail tree vegetation impostor color render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the low detail tree vegetation impostor colors.
	*/
	void RenderInternal() NOEXCEPT;

};