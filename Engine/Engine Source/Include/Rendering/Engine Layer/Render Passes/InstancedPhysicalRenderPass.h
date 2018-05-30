#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class InstancedPhysicalRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(InstancedPhysicalRenderPass);

	/*
	*	Initializes the instanced physical render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Renders the instanced physical entities.
	*/
	void RenderInternal() NOEXCEPT;

};