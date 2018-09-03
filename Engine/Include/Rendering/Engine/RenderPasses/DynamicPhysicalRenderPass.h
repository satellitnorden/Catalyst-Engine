#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class DynamicPhysicalRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DynamicPhysicalRenderPass);

	/*
	*	Default constructor.
	*/
	DynamicPhysicalRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the dynamic physical render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the dynamic physical entities.
	*/
	void RenderInternal() NOEXCEPT;

};