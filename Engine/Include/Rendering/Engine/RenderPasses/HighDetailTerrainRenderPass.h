#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class HighDetailTerrainRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(HighDetailTerrainRenderPass);

	/*
	*	Default constructor.
	*/
	HighDetailTerrainRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the high detail terrain render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the high detail terrain.
	*/
	void RenderInternal() NOEXCEPT;

};