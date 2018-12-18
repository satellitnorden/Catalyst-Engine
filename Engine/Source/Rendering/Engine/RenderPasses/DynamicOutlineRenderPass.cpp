//Header file.
#include <Rendering/Engine/RenderPasses/DynamicOutlineRenderPass.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>
#include <Rendering/Engine/PhysicalVertex.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DynamicOutlineRenderPass);

/*
*	Default constructor.
*/
DynamicOutlineRenderPass::DynamicOutlineRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		DynamicOutlineRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the dynamic outline render pass.
*/
void DynamicOutlineRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Lighting);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::DynamicOutline);

	//Set the shaders.
	SetVertexShader(Shader::PhysicalVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::OutlineFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::Scene);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(Matrix4));
	AddPushConstantRange(ShaderStage::Fragment, sizeof(Matrix4), sizeof(Vector3<float>));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(4);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(PhysicalVertex, _Position));
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(PhysicalVertex, _Normal));
	AddVertexInputAttributeDescription(	2,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(PhysicalVertex, _Tangent));
	AddVertexInputAttributeDescription(	3,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										offsetof(PhysicalVertex, _TextureCoordinate));

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(PhysicalVertex), VertexInputBindingDescription::InputRate::Vertex);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(true);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::One);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::Back);
	SetDepthCompareOperator(CompareOperator::Equal);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
	SetStencilTestEnabled(false);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Keep);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(0);
	SetTopology(Topology::TriangleList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		DynamicOutlineRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the dynamic physical entities.
*/
void DynamicOutlineRenderPass::RenderInternal() NOEXCEPT
{
	//Iterate over all dynamic physical components and draw them all.
	const uint64 numberOfDynamicPhysicalComponents{ ComponentManager::GetNumberOfDynamicPhysicalComponents() };

	//If there's none to render - render none.
	if (numberOfDynamicPhysicalComponents == 0)
	{
		//Don't include this render pass in the final render.
		SetIncludeInRender(false);

		return;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };
	const DynamicOutlineRenderComponent *RESTRICT renderComponent{ ComponentManager::GetDynamicPhysicalDynamicOutlineRenderComponents() };
	const TransformComponent *RESTRICT transformComponent{ ComponentManager::GetDynamicPhysicalTransformComponents() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Track the previous state, so if two static physical entities share the same state, it doesn't have to be rebound.
	ConstantBufferHandle previousBuffer{ nullptr };

	for (uint64 i = 0; i < numberOfDynamicPhysicalComponents; ++i, ++renderComponent, ++transformComponent)
	{
		//Don't draw this dynamic physical entity if it isn't in the view frustum or if it's not supposed to be included in this render pass.
		if (!renderComponent->_IsInViewFrustum || !((renderComponent->_PhysicalFlags & PhysicalFlag::Outline) == PhysicalFlag::Outline))
		{
			continue;
		}

		const uint64 offset{ 0 };

		const Matrix4 modelMatrix{ transformComponent->_Position, transformComponent->_Rotation, transformComponent->_Scale };
		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(Matrix4), &modelMatrix);
		commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(Matrix4), sizeof(Vector3<float>), &renderComponent->_Color);

		if (previousBuffer != renderComponent->_Buffer)
		{
			previousBuffer = renderComponent->_Buffer;

			commandBuffer->BindVertexBuffer(this, 0, renderComponent->_Buffer, &offset);
			commandBuffer->BindIndexBuffer(this, renderComponent->_Buffer, renderComponent->_IndexOffset);
		}

		commandBuffer->DrawIndexed(this, renderComponent->_IndexCount, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}