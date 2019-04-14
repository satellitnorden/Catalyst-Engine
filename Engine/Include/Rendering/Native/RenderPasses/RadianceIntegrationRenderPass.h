#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderPasses/RenderPass.h>

class RadianceIntegrationRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(RadianceIntegrationRenderPass);

	/*
	*	Default constructor.
	*/
	RadianceIntegrationRenderPass() NOEXCEPT;

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