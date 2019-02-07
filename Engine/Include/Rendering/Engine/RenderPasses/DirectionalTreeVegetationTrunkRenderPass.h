#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class DirectionalTreeVegetationTrunkRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DirectionalTreeVegetationTrunkRenderPass);

	/*
	*	Default constructor.
	*/
	DirectionalTreeVegetationTrunkRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the directional tree vegetation trunk render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the directional tree vegetation trunks.
	*/
	void RenderInternal() NOEXCEPT;

};