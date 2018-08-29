#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class ParticleSystemRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ParticleSystemRenderPass);

	/*
	*	Default constructor.
	*/
	ParticleSystemRenderPass() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle renderDataTableLayout;

	//The render data table.
	RenderDataTableHandle renderDataTable;

	/*
	*	Initializes the particle system render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

	/*
	*	Renders the particle systems.
	*/
	void RenderInternal() NOEXCEPT;

};