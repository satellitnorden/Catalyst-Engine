#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class DirectionalTerrainShadowRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DirectionalTerrainShadowRenderPass);

	/*
	*	Default constructor.
	*/
	DirectionalTerrainShadowRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the directional terrain shadow render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the directional terrain shadow.
	*/
	void RenderInternal() NOEXCEPT;

};