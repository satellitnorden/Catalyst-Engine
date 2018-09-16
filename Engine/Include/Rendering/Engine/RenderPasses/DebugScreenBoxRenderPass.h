#if !defined(CATALYST_FINAL)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class DebugScreenBoxRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DebugScreenBoxRenderPass);

	/*
	*	Default constructor.
	*/
	DebugScreenBoxRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the debug screen boxes render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the debug screen boxes.
	*/
	void RenderInternal() NOEXCEPT;

};
#endif