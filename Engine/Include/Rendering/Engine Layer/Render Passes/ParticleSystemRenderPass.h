#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

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

	/*
	*	Initializes the particle system render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the particle systems.
	*/
	void RenderInternal() NOEXCEPT;

};