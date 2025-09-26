#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Systems.
#include <Systems/System.h>

class DebugRenderPass final : public RenderPass
{
	
public:

	//Enumeration covering all visualization modes.
	enum class VisualizationMode : uint8
	{
		NONE,
		ONLY_LIGHTING,
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
		SYSTEM_NOT_DEFINED_REQUIREMENT(CATALYST_CONFIGURATION_FINAL)
	);

	/*
	*	Default constructor.
	*/
	DebugRenderPass() NOEXCEPT;

	/*
	*	Returns the visualization mode.
	*/
	FORCE_INLINE NO_DISCARD VisualizationMode GetVisualizationMode() const NOEXCEPT
	{
		return _RequestedVisualizationMode;
	}

	/*
	*	Sets the visualization mode.
	*/
	FORCE_INLINE void SetVisualizationMode(const VisualizationMode value) NOEXCEPT
	{
		_RequestedVisualizationMode = value;
	}

private:

	//The requested visualization mode.
	VisualizationMode _RequestedVisualizationMode{ VisualizationMode::NONE };

	//The current visualization mode.
	VisualizationMode _CurrentVisualizationMode{ VisualizationMode::NONE };

	//The debug visualization pipeline.
	GraphicsRenderPipeline _DebugVisualizationPipeline{ HashString("DebugVisualization_RenderPipeline") };

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