#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderPasses/RenderPass.h>

class DirectLightingDenoisingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(DirectLightingDenoisingRenderPass);

	/*
	*	Default constructor.
	*/
	DirectLightingDenoisingRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};