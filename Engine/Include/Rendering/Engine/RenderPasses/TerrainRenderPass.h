#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class TerrainRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(TerrainRenderPass);

	/*
	*	Default constructor.
	*/
	TerrainRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the terrain render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the terrain.
	*/
	void RenderInternal() NOEXCEPT;

};