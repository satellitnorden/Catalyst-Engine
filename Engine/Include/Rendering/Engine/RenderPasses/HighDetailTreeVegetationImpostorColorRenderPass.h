#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class HighDetailTreeVegetationImpostorColorRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(HighDetailTreeVegetationImpostorColorRenderPass);

	/*
	*	Default constructor.
	*/
	HighDetailTreeVegetationImpostorColorRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the high detail tree vegetation impostor color render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the high detail tree vegetation impostor colors.
	*/
	void RenderInternal() NOEXCEPT;

};