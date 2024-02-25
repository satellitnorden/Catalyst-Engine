#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ALIGN(8) DirectLightingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(DirectLightingRenderPass);

	/*
	*	Default constructor.
	*/
	DirectLightingRenderPass() NOEXCEPT;

private:

	//The direct lighting pipeline.
	GraphicsRenderPipeline _DirectLightingPipeline{ HashString("DirectLighting_RenderPipeline") };

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