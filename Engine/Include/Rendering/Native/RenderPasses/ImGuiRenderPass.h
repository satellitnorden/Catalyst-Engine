#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ImGuiGraphicsPipeline.h>
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
		NOT_DEFINED_REQUIREMENT(CATALYST_CONFIGURATION_FINAL)
	);

	/*
	*	Default constructor.
	*/
	ImGuiRenderPass() NOEXCEPT;

	/*
	*	Returns the editor user interface graphics pipeline.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD ImGuiGraphicsPipeline *const RESTRICT GetImGuiGraphicsPipeline() NOEXCEPT
	{
		return &_ImGuiGraphicsPipeline;
	}

private:

	//The ImGui graphics pipeline.
	ImGuiGraphicsPipeline _ImGuiGraphicsPipeline;

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