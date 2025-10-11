#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Systems.
#include <Systems/System.h>

class ImGuiRenderPass final : public RenderPass
{
	
public:

	//System declaration.
	CATALYST_SYSTEM
	(
		ImGuiRenderPass,
		SYSTEM_NOT_DEFINED_REQUIREMENT(CATALYST_CONFIGURATION_FINAL)
	);

	/*
	*	Default constructor.
	*/
	ImGuiRenderPass() NOEXCEPT;

	/*
	*	Returns the pipeline.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD GraphicsRenderPipeline *const RESTRICT GetPipeline() NOEXCEPT
	{
		return &_ImGuiPipeline;
	}

private:

	//The ImGui pipeline.
	GraphicsRenderPipeline _ImGuiPipeline{ HashString("ImGui_RenderPipeline") };

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Pre-records this render pass.
	*/
	void PreRecord(CommandBuffer *const RESTRICT frame_command_buffer) NOEXCEPT;

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