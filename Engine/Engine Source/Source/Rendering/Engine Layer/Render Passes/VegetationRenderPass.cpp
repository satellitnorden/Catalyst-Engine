//Header file.
#include <Rendering/Engine Layer/Render Passes/VegetationRenderPass.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine Layer/CommandBuffer.h>
#include <Rendering/Engine Layer/VegetationTransformation.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(VegetationRenderPass);

/*
*	Default constructor.
*/
VegetationRenderPass::VegetationRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		VegetationRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the vegetation render pass.
*/
void VegetationRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::Vegetation);

	//Set the shaders.
	SetVertexShader(Shader::VegetationVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::VegetationGeometry);
	SetFragmentShader(Shader::VegetationFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(3);
	AddRenderTarget(RenderTarget::SceneBufferAlbedo);
	AddRenderTarget(RenderTarget::SceneBufferNormalDepth);
	AddRenderTarget(RenderTarget::SceneBufferMaterialProperties);

	//Add the descriptor set layouts.
	SetNumberOfDescriptorSetLayouts(2);
	AddDescriptorSetLayout(RenderDataTableLayout::DynamicUniformData);
	AddDescriptorSetLayout(RenderDataTableLayout::Vegetation);

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(3);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										0);
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										sizeof(Vector3));
	AddVertexInputAttributeDescription(	2,
										0,
										VertexInputAttributeDescription::Format::X32SignedFloat,
										sizeof(Vector3) + sizeof(Vector2));

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(VegetationTransformation), VertexInputBindingDescription::InputRate::Instance);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::Load);
	SetColorAttachmentStoreOperator(AttachmentStoreOperator::Store);
	SetCullMode(CullMode::None);
	SetDepthAttachmentLoadOperator(AttachmentLoadOperator::Load);
	SetDepthAttachmentStoreOperator(AttachmentStoreOperator::Store);
	SetDepthCompareOperator(CompareOperator::Less);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
	SetTopology(Topology::PointList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		VegetationRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the vegetation entities.
*/
void VegetationRenderPass::RenderInternal() NOEXCEPT
{
	//Iterate over all vegetation components and draw them all.
	const uint64 numberOfVegetationComponents{ ComponentManager::GetNumberOfVegetationComponents() };

	//If there's none to render - render none.
	if (numberOfVegetationComponents == 0)
	{
		//Don't include this render pass in the final render.
		SetIncludeInRender(false);

		return;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };
	const RenderDataTableHandle currentDynamicUniformDataDescriptorSet{ RenderingSystem::Instance->GetCurrentDynamicUniformDataDescriptorSet() };
	const VegetationComponent *RESTRICT component{ ComponentManager::GetVegetationComponents() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the current dynamic uniform data descriptor set.
	commandBuffer->BindRenderDataTables(this, 0, 1, &currentDynamicUniformDataDescriptorSet);

	//Wait for the vegetation culling to finish.
	CullingSystem::Instance->WaitForVegetationCulling();

	for (uint64 i = 0; i < numberOfVegetationComponents; ++i, ++component)
	{
		commandBuffer->BindRenderDataTables(this, 1, 1, &component->descriptorSet);
		
		for (uint64 j = 0, size = component->shouldDrawGridCell.Size(); j < size; ++j)
		{
			if (component->shouldDrawGridCell[j])
			{
				commandBuffer->BindVertexBuffers(this, 1, &component->transformationsBuffer, &component->transformationOffsets[j]);
				commandBuffer->Draw(this, 1, component->instanceCounts[j]);
			}
		}
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}