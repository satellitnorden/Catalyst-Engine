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
	*	Sets whether or not to render directly to the screen.
	*/
	FORCE_INLINE void SetRenderDirectlyToScreen(const bool value) NOEXCEPT
	{
		_RenderDirectlyToScreen = value;
	}

	/*
	*	Returns the pipeline.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD GraphicsRenderPipeline *const RESTRICT GetPipeline() NOEXCEPT
	{
		return &_ImGuiPipeline;
	}

private:

	//Denotes whether or not to render directly to screen.
	bool _RenderDirectlyToScreen{ false };

	//The ImGui pipeline.
	GraphicsRenderPipeline _ImGuiPipeline{ HashString("ImGui") };

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