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

	/*
	*	Initializes the particle system render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the particle systems.
	*/
	void RenderInternal() NOEXCEPT;

};