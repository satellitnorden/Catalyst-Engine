//Header file.
#include <Rendering/Engine Layer/Render Passes/TerrainRenderPass.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine Layer/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(TerrainRenderPass);

/*
*	Initializes the terrain render pass.
*/
void TerrainRenderPass::Initialize() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::Terrain);

	//Set the shaders.
	SetVertexShader(Shader::TerrainVertex);
	SetTessellationControlShader(Shader::TerrainTessellationControl);
	SetTessellationEvaluationShader(Shader::TerrainTessellationEvaluation);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::SceneBufferFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render targets.
	SetMaximumNumberOfRenderTargets(3);
	AddRenderTarget(RenderTarget::SceneBufferAlbedo);
	AddRenderTarget(RenderTarget::SceneBufferNormalDepth);
	AddRenderTarget(RenderTarget::SceneBufferMaterialProperties);

	//Set the properties of the render pass.
	SetCullFace(CullFace::Back);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the terrain.
*/
void TerrainRenderPass::Render() NOEXCEPT
{
	//Iterate over all terrain components and draw them all.
	const uint64 numberOfTerrainEntityComponents{ ComponentManager::GetNumberOfTerrainComponents() };

	//If there's none to render - render none.
	if (numberOfTerrainEntityComponents == 0)
	{
		return;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };
	const DescriptorSetHandle currentDynamicUniformDataDescriptorSet{ RenderingSystem::Instance->GetCurrentDynamicUniformDataDescriptorSet() };
	const TerrainRenderComponent *RESTRICT component{ ComponentManager::GetTerrainRenderComponents() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the current dynamic uniform data descriptor set.
	
	commandBuffer->BindDescriptorSets(this, 0, 1, &currentDynamicUniformDataDescriptorSet);

	for (uint64 i = 0; i < numberOfTerrainEntityComponents; ++i, ++component)
	{
		const uint64 offset{ 0 };

		commandBuffer->BindDescriptorSets(this, 1, 1, &component->descriptorSet);
		commandBuffer->BindVertexBuffers(this, 1, &component->vertexAndIndexBuffer, &offset);

		/*
		commandBuffer->CommandBindIndexBuffer(*static_cast<const VulkanConstantBuffer *RESTRICT>(terrainRenderComponent->vertexAndIndexBuffer), terrainRenderComponent->indexBufferOffset);
		commandBuffer->CommandDrawIndexed(terrainRenderComponent->indexCount, 1);
		*/
	}

	//End the command buffer.
	commandBuffer->End(this);
}