#if !defined(CATALYST_SIMPLIFIED_RENDERING)
#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/EditorUserInterfaceGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class EditorUserInterfaceRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(EditorUserInterfaceRenderPass);

	/*
	*	Default constructor.
	*/
	EditorUserInterfaceRenderPass() NOEXCEPT;

	/*
	*	Returns the editor user interface graphics pipeline.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD EditorUserInterfaceGraphicsPipeline *const RESTRICT GetEditorUserInterfaceGraphicsPipeline() NOEXCEPT
	{
		return &_EditorUserInterfaceGraphicsPipeline;
	}

private:

	//The editor user interface graphics pipeline.
	EditorUserInterfaceGraphicsPipeline _EditorUserInterfaceGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};
#endif
#endif