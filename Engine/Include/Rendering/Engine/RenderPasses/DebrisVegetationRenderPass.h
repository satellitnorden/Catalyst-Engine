#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class DebrisVegetationRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DebrisVegetationRenderPass);

	/*
	*	Default constructor.
	*/
	DebrisVegetationRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the debris vegetation render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the debris vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};