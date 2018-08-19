//Header file.
#include <Rendering/Engine/RenderPasses/StaticPhysicalRenderPass.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>
#include <Rendering/Engine/PhysicalVertex.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(StaticPhysicalRenderPass);

/*
*	Default constructor.
*/
StaticPhysicalRenderPass::StaticPhysicalRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		StaticPhysicalRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the static physical render pass.
*/
void StaticPhysicalRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::SceneBuffer);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::StaticPhysical);

	//Set the sub stage index.
	SetSubStageIndex(1);

	//Set the shaders.
	SetVertexShader(Shader::PhysicalVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::PhysicalFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(3);
	AddRenderTarget(RenderTarget::SceneBufferAlbedo);
	AddRenderTarget(RenderTarget::SceneBufferNormalDepth);
	AddRenderTarget(RenderTarget::SceneBufferMaterialProperties);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::DynamicUniformData));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Physical));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(Matrix4));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(4);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(PhysicalVertex, position));
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(PhysicalVertex, normal));
	AddVertexInputAttributeDescription(	2,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(PhysicalVertex, tangent));
	AddVertexInputAttributeDescription(	3,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										offsetof(PhysicalVertex, textureCoordinate));

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(PhysicalVertex), VertexInputBindingDescription::InputRate::Vertex);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

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
		StaticPhysicalRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the terrain.
*/
void StaticPhysicalRenderPass::RenderInternal() NOEXCEPT
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
	const StaticPhysicalRenderComponent *RESTRICT renderComponent{ ComponentManager::GetStaticPhysicalRenderComponents() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data table.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetCurrentDynamicUniformDataRenderDataTable());

	//Wait for the static physical culling to finish.
	CullingSystem::Instance->WaitForStaticPhysicalCulling();

	//Track the previous state, so if two static physical entities share the same state, it doesn't have to be rebound.
	ConstantBufferHandle previousBuffer{ nullptr };
	RenderDataTableHandle previousRenderDataTable{ nullptr };

	for (uint64 i = 0; i < numberOfStaticPhysicalComponents; ++i, ++renderComponent)
	{
		//Don't draw this static physical entity if it isn't in the view frustum.
		if (!renderComponent->isInViewFrustum)
		{
			continue;
		}

		const uint64 offset{ 0 };

		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(Matrix4), &renderComponent->modelMatrix);

		if (previousBuffer != renderComponent->buffer)
		{
			previousBuffer = renderComponent->buffer;

			commandBuffer->BindVertexBuffers(this, 1, &renderComponent->buffer, &offset);
			commandBuffer->BindIndexBuffer(this, renderComponent->buffer, renderComponent->indexOffset);
		}

		if (previousRenderDataTable != renderComponent->renderDataTable)
		{
			previousRenderDataTable = renderComponent->renderDataTable;

			commandBuffer->BindRenderDataTable(this, 1, renderComponent->renderDataTable);
		}
		
		commandBuffer->DrawIndexed(this, renderComponent->indexCount, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}