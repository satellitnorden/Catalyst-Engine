#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/DebugGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Systems.
#include <Systems/System.h>

class DebugRenderPass final : public RenderPass
{
	
public:

	//Enumeration covering all modes.
	enum class Mode : uint8
	{
		NONE,
		ALBEDO,
		THICKNESS,
		NORMAL,
		DEPTH,
		ROUGHNESS,
		METALLIC,
		AMBIENT_OCCLUSION,
		EMISSIVE,
		DIFFUSE_IRRADIANCE,
		SPECULAR_IRRADIANCE
	};

	//System declaration.
	CATALYST_SYSTEM
	(
		DebugRenderPass,
		NOT_DEFINED_REQUIREMENT(CATALYST_CONFIGURATION_FINAL)
	);

	/*
	*	Default constructor.
	*/
	DebugRenderPass() NOEXCEPT;

	/*
	*	Sets the mode.
	*/
	FORCE_INLINE void SetMode(const Mode value) NOEXCEPT
	{
		_RequestedMode = value;
	}

private:

	//The requested mode.
	Mode _RequestedMode{ Mode::NONE };

	//The current mode.
	Mode _CurrentMode{ Mode::NONE };

	//The debug graphics pipeline.
	DebugGraphicsPipeline _DebugGraphicsPipeline;

	//The render pipelines.
	StaticArray<GraphicsRenderPipeline, 2> _RenderPipelines
	{
		GraphicsRenderPipeline(HashString("TerrainWireframe_RenderPipeline")),
		GraphicsRenderPipeline(HashString("StaticModelWireframe_RenderPipeline"))
	};

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
#endif