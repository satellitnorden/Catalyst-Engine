#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class DynamicOutlineRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DynamicOutlineRenderPass);

	/*
	*	Default constructor.
	*/
	DynamicOutlineRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the dynamic outline render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the dynamic physical entities.
	*/
	void RenderInternal() NOEXCEPT;

};