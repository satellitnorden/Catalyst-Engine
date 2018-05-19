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
	*	Initializes the particle system render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Renders the particle systems.
	*/
	void Render() NOEXCEPT;

};