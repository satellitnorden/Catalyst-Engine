//Header file.
#include <Rendering/Engine/RenderPasses/DirectionalStaticPhysicalShadowRenderPass.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>
#include <Rendering/Engine/PhysicalVertex.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/EngineSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DirectionalStaticPhysicalShadowRenderPass);

/*
*	Default constructor.
*/
DirectionalStaticPhysicalShadowRenderPass::DirectionalStaticPhysicalShadowRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		DirectionalStaticPhysicalShadowRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the directional static physical shadow render pass.
*/
void DirectionalStaticPhysicalShadowRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the sub stage.
	SetSubStage(RenderPassSubStage::DirectionalStaticPhysicalShadow);

	//Set the shaders.
	SetVertexShader(Shader::DirectionalPhysicalShadowVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::ShadowMapFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::DirectionalLight);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::DirectionalShadowMap);

	//Add the descriptor set layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderDataTableLayout::DynamicUniformData);
	AddRenderDataTableLayout(RenderDataTableLayout::Physical);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(Matrix4));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(4);
	AddVertexInputAttributeDescription(0,
		0,
		VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
		offsetof(PhysicalVertex, position));
	AddVertexInputAttributeDescription(1,
		0,
		VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
		offsetof(PhysicalVertex, normal));
	AddVertexInputAttributeDescription(2,
		0,
		VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
		offsetof(PhysicalVertex, tangent));
	AddVertexInputAttributeDescription(3,
		0,
		VertexInputAttributeDescription::Format::X32Y32SignedFloat,
		offsetof(PhysicalVertex, textureCoordinate));

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(PhysicalVertex), VertexInputBindingDescription::InputRate::Vertex);

	//Set the render resolution.
	SetRenderResolution(Resolution(EngineSystem::Instance->GetProjectConfiguration().renderingConfiguration.shadowMapResolution, EngineSystem::Instance->GetProjectConfiguration().renderingConfiguration.shadowMapResolution));

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::Load);
	SetColorAttachmentStoreOperator(AttachmentStoreOperator::Store);
	SetCullMode(CullMode::Back);
	SetDepthAttachmentLoadOperator(AttachmentLoadOperator::Load);
	SetDepthAttachmentStoreOperator(AttachmentStoreOperator::Store);
	SetDepthCompareOperator(CompareOperator::Less);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
	SetTopology(Topology::TriangleList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		DirectionalStaticPhysicalShadowRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the directional static physical entity shadows.
*/
void DirectionalStaticPhysicalShadowRenderPass::RenderInternal() NOEXCEPT
{
	//Iterate over all static physical components and draw them all.
	const uint64 numberOfStaticPhysicalComponents{ ComponentManager::GetNumberOfStaticPhysicalComponents() };

	//If there's none to render - render none.
	if (numberOfStaticPhysicalComponents == 0)
	{
		//Don't include this render pass in the final render.
		SetIncludeInRender(false);

		return;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };
	const RenderDataTableHandle currentDynamicUniformDataDescriptorSet{ RenderingSystem::Instance->GetCurrentDynamicUniformDataDescriptorSet() };
	const StaticPhysicalRenderComponent *RESTRICT renderComponent{ ComponentManager::GetStaticPhysicalRenderComponents() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the current dynamic uniform data descriptor set.
	commandBuffer->BindRenderDataTables(this, 0, 1, &currentDynamicUniformDataDescriptorSet);

	for (uint64 i = 0; i < numberOfStaticPhysicalComponents; ++i, ++renderComponent)
	{
		const uint64 offset{ 0 };

		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(Matrix4), &renderComponent->modelMatrix);
		commandBuffer->BindRenderDataTables(this, 1, 1, &renderComponent->renderDataTable);
		commandBuffer->BindVertexBuffers(this, 1, &renderComponent->buffer, &offset);
		commandBuffer->BindIndexBuffer(this, renderComponent->buffer, renderComponent->indexOffset);
		commandBuffer->DrawIndexed(this, renderComponent->indexCount, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}