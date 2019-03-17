#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class PathTracingPrototypeRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(PathTracingPrototypeRenderPass);

	/*
	*	Default constructor.
	*/
	PathTracingPrototypeRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the path tracing prototype render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the path tracing prototype.
	*/
	void RenderInternal() NOEXCEPT;

};