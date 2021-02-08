//Header file.
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Rendering.
#include <Rendering/Native/RenderPasses/RenderPassManager.h>

void RenderPass::SetStage(const NativeRenderPassStage stage) NOEXCEPT
{
	//Set the stage.
	_Stage = stage;

	//Register this render pass.
	RenderPassManager::RegisterRenderPass(this);
}