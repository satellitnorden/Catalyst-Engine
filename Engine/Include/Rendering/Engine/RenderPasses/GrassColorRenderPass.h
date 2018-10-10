#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class GrassColorRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(GrassColorRenderPass);

	/*
	*	Default constructor.
	*/
	GrassColorRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the grass color render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the color of the grass.
	*/
	void RenderInternal() NOEXCEPT;

};