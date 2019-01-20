#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class LowDetailTreeVegetationTrunkRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(LowDetailTreeVegetationTrunkRenderPass);

	/*
	*	Default constructor.
	*/
	LowDetailTreeVegetationTrunkRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the low detail tree vegetation trunk render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the low detail tree vegetation trunks.
	*/
	void RenderInternal() NOEXCEPT;

};