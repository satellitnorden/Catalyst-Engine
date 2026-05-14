#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Systems.
#include <Systems/System.h>

class ToneMappingRenderPass final : public RenderPass
{
	
public:

	//System declaration.
	CATALYST_SYSTEM
	(
		ToneMappingRenderPass
	);

	/*
	*	Default constructor.
	*/
	ToneMappingRenderPass() NOEXCEPT;

private:

	//The copy pipeline.
	GraphicsRenderPipeline _CopyPipeline{ HashString("Copy") };

	//The tone mapping pipeline.
	GraphicsRenderPipeline _ToneMappingPipeline{ HashString("ToneMapping") };

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

	/*
	*	Terminates this render pass.
	*/
	void Terminate() NOEXCEPT;

};