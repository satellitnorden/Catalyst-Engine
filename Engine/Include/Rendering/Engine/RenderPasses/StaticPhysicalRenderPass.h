#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class StaticPhysicalRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(StaticPhysicalRenderPass);

	/*
	*	Default constructor.
	*/
	StaticPhysicalRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the static physical render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the static physical entities.
	*/
	void RenderInternal() NOEXCEPT;

};