#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class InstancedPhysicalRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(InstancedPhysicalRenderPass);

	/*
	*	Default constructor.
	*/
	InstancedPhysicalRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the instanced physical render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the instanced physical entities.
	*/
	void RenderInternal() NOEXCEPT;

};