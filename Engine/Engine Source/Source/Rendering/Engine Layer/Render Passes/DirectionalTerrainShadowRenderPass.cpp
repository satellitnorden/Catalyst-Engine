//Header file.
#include <Rendering/Engine Layer/Render Passes/DirectionalTerrainShadowRenderPass.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine Layer/CommandBuffer.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DirectionalTerrainShadowRenderPass);

/*
*	Initializes the directional terrain shadow render pass.
*/
void DirectionalTerrainShadowRenderPass::Initialize() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::DirectionalTerrainShadow);

	//Set the shaders.
	SetVertexShader(Shader::TerrainVertex);
	SetTessellationControlShader(Shader::TerrainTessellationControl);
	SetTessellationEvaluationShader(Shader::DirectionalTerrainShadowTessellationEvaluation);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::ShadowMapFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::DirectionalLight);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::DirectionalPreBlurShadowMap);

	//Add the descriptor set layouts.
	SetNumberOfDescriptorSetLayouts(2);
	AddDescriptorSetLayout(DescriptorSetLayout::DynamicUniformData);
	AddDescriptorSetLayout(DescriptorSetLayout::Terrain);

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
	SetRenderResolution(Resolution(RenderingConstants::SHADOW_MAP_RESOLUTION, RenderingConstants::SHADOW_MAP_RESOLUTION));

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

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the directional terrain shadow.
*/
void DirectionalTerrainShadowRenderPass::Render() NOEXCEPT
{
	//Iterate over all terrain components and draw them all.
	const uint64 numberOfTerrainComponents{ ComponentManager::GetNumberOfTerrainComponents() };

	//If there's none to render - render none.
	if (numberOfTerrainComponents == 0)
	{
		//Don't include this render pass in the final render.
		SetIncludeInRender(false);

		return;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };
	const TerrainRenderComponent *RESTRICT component{ ComponentManager::GetTerrainRenderComponents() };
	const DescriptorSetHandle currentDynamicUniformDataDescriptorSet{ RenderingSystem::Instance->GetCurrentDynamicUniformDataDescriptorSet() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the current dynamic uniform data descriptor set.
	commandBuffer->BindDescriptorSets(this, 0, 1, &currentDynamicUniformDataDescriptorSet);

	//Wait for the terrain culling to finish.
	CullingSystem::Instance->WaitForTerrainCulling();

	//Wait for the terrain level of detail to finish.
	LevelOfDetailSystem::Instance->WaitForTerrainLevelOfDetail();

	for (uint64 i = 0; i < numberOfTerrainComponents; ++i, ++component)
	{
		const VkDeviceSize offset{ 0 };

		commandBuffer->BindDescriptorSets(this, 1, 1, &component->descriptorSet);
		commandBuffer->BindVertexBuffers(this, 1, &component->vertexAndIndexBuffer, &offset);
		commandBuffer->BindIndexBuffer(this, component->vertexAndIndexBuffer, component->indexBufferOffset);
		commandBuffer->DrawIndexed(this, component->indexCount, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}