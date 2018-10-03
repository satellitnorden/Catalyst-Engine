#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class LowDetailTerrainRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(LowDetailTerrainRenderPass);

	/*
	*	Default constructor.
	*/
	LowDetailTerrainRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the low detail terrain render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the low detail terrain.
	*/
	void RenderInternal() NOEXCEPT;

};