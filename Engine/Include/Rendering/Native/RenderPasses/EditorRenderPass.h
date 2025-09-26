#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Systems.
#include <Systems/System.h>

class EditorRenderPass final : public RenderPass
{
	
public:

	//System declaration.
	CATALYST_SYSTEM
	(
		EditorRenderPass,
		SYSTEM_DEFINED_REQUIREMENT(CATALYST_EDITOR)
	);

	/*
	*	Default constructor.
	*/
	EditorRenderPass() NOEXCEPT;

private:

	//The pipelines.
	StaticArray<GraphicsRenderPipeline, 4> _Pipelines
	{
		GraphicsRenderPipeline(HashString("PlayerSpawnMarker_RenderPipeline")),
		GraphicsRenderPipeline(HashString("ModelEditorMetadata_RenderPipeline")),
		GraphicsRenderPipeline(HashString("PlaneEditorMetadata_RenderPipeline")),
		GraphicsRenderPipeline(HashString("EditorSelectedOutline_RenderPipeline"))
	};

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Pre records this render pass.
	*/
	void PreRecord(CommandBuffer *const RESTRICT command_buffer) NOEXCEPT;

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