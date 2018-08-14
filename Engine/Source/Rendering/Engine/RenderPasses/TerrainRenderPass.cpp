//Header file.
#include <Rendering/Engine/RenderPasses/TerrainRenderPass.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(TerrainRenderPass);

/*
*	Default constructor.
*/
TerrainRenderPass::TerrainRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		TerrainRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the terrain render pass.
*/
void TerrainRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the sub stage.
	SetSubStage(RenderPassSubStage::Terrain);

	//Set the shaders.
	SetVertexShader(Shader::TerrainVertex);
	SetTessellationControlShader(Shader::TerrainTessellationControl);
	SetTessellationEvaluationShader(Shader::TerrainTessellationEvaluation);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::TerrainFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(3);
	AddRenderTarget(RenderTarget::SceneBufferAlbedo);
	AddRenderTarget(RenderTarget::SceneBufferNormalDepth);
	AddRenderTarget(RenderTarget::SceneBufferMaterialProperties);

	//Add the descriptor set layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(CommonRenderDataTableLayout::DynamicUniformData);
	AddRenderDataTableLayout(CommonRenderDataTableLayout::Terrain);

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(2);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										0);
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										sizeof(float) * 3);

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(float) * 5, VertexInputBindingDescription::InputRate::Vertex);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::Clear);
	SetColorAttachmentStoreOperator(AttachmentStoreOperator::Store);
	SetCullMode(CullMode::Back);
	SetDepthAttachmentLoadOperator(AttachmentLoadOperator::Clear);
	SetDepthAttachmentStoreOperator(AttachmentStoreOperator::Store);
	SetDepthCompareOperator(CompareOperator::Less);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
	SetTopology(Topology::PatchList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		TerrainRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the terrain.
*/
void TerrainRenderPass::RenderInternal() NOEXCEPT
{
	//Iterate over all terrain components and draw them all.
	const uint64 numberOfTerrainEntityComponents{ ComponentManager::GetNumberOfTerrainComponents() };

	//If there's none to render - render none.
	if (numberOfTerrainEntityComponents == 0)
	{
		//Don't include this render pass in the final render.
		SetIncludeInRender(false);

		return;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };
	const RenderDataTableHandle currentDynamicUniformDataDescriptorSet{ RenderingSystem::Instance->GetCurrentDynamicUniformDataDescriptorSet() };
	const TerrainRenderComponent *RESTRICT component{ ComponentManager::GetTerrainRenderComponents() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the current dynamic uniform data descriptor set.
	commandBuffer->BindRenderDataTables(this, 0, 1, &currentDynamicUniformDataDescriptorSet);

	//Wait for the terrain culling to finish.
	CullingSystem::Instance->WaitForTerrainCulling();

	//Wait for the terrain level of detail to finish.
	LevelOfDetailSystem::Instance->WaitForTerrainLevelOfDetail();

	for (uint64 i = 0; i < numberOfTerrainEntityComponents; ++i, ++component)
	{
		if (!component->isInViewFrustum)
		{
			continue;
		}

		const uint64 offset{ 0 };

		commandBuffer->BindRenderDataTables(this, 1, 1, &component->renderDataTable);
		commandBuffer->BindVertexBuffers(this, 1, &component->vertexAndIndexBuffer, &offset);
		commandBuffer->BindIndexBuffer(this, component->vertexAndIndexBuffer, component->indexBufferOffset);
		commandBuffer->DrawIndexed(this, component->indexCount, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}