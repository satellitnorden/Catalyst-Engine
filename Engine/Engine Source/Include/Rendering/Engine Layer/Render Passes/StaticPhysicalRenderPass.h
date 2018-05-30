#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class StaticPhysicalRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(StaticPhysicalRenderPass);

	/*
	*	Initializes the static physical render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Renders the static physical entities.
	*/
	void RenderInternal() NOEXCEPT;

};