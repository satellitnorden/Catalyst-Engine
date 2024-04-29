#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Systems.
#include <Systems/System.h>

class SkyRenderPass final : public RenderPass
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		SkyRenderPass
	);

	/*
	*	Default constructor.
	*/
	SkyRenderPass() NOEXCEPT;

private:

	//The sky pipeline.
	GraphicsRenderPipeline _SkyPipeline{ HashString("Sky_RenderPipeline") };

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