#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class VegetationDepthRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(VegetationDepthRenderPass);

	/*
	*	Default constructor.
	*/
	VegetationDepthRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the vegetation depth render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the depth of the vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};