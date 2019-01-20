#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class HighDetailTreeVegetationTrunkRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(HighDetailTreeVegetationTrunkRenderPass);

	/*
	*	Default constructor.
	*/
	HighDetailTreeVegetationTrunkRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the high detail tree vegetation trunk render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the high detail tree vegetation trunks.
	*/
	void RenderInternal() NOEXCEPT;

};