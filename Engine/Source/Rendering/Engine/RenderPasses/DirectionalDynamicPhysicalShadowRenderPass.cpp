//Header file.
#include <Rendering/Engine/RenderPasses/DirectionalDynamicPhysicalShadowRenderPass.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>
#include <Rendering/Engine/PhysicalVertex.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DirectionalDynamicPhysicalShadowRenderPass);

/*
*	Default constructor.
*/
DirectionalDynamicPhysicalShadowRenderPass::DirectionalDynamicPhysicalShadowRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		DirectionalDynamicPhysicalShadowRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the directional dynamic physical shadow render pass.
*/
void DirectionalDynamicPhysicalShadowRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::DirectionalShadowMapping);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::DirectionalDynamicPhysicalShadow);

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

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(Matrix4));

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
	SetRenderResolution(Resolution(CatalystEngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution, CatalystEngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution));

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::Back);
	SetDepthCompareOperator(CompareOperator::Less);
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
		DirectionalDynamicPhysicalShadowRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the directional dynamic physical entity shadows.
*/
void DirectionalDynamicPhysicalShadowRenderPass::RenderInternal() NOEXCEPT
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
	const DynamicPhysicalRenderComponent *RESTRICT renderComponent{ ComponentManager::GetDynamicPhysicalDynamicPhysicalRenderComponents() };
	const TransformComponent *RESTRICT transformComponent{ ComponentManager::GetDynamicPhysicalTransformComponents() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Track the previous state, so if two dynamic physical entities share the same state, it doesn't have to be rebound.
	ConstantBufferHandle previousBuffer{ nullptr };

	for (uint64 i = 0; i < numberOfDynamicPhysicalComponents; ++i, ++renderComponent, ++transformComponent)
	{
		const uint64 offset{ 0 };

		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(Matrix4), &transformComponent->_WorldTransform);

		if (previousBuffer != renderComponent->_Model._Buffers[UNDERLYING(LevelOfDetail::High)])
		{
			previousBuffer = renderComponent->_Model._Buffers[UNDERLYING(LevelOfDetail::High)];

			commandBuffer->BindVertexBuffer(this, 0, renderComponent->_Model._Buffers[UNDERLYING(LevelOfDetail::High)], &offset);
			commandBuffer->BindIndexBuffer(this, renderComponent->_Model._Buffers[UNDERLYING(LevelOfDetail::High)], renderComponent->_Model._IndexOffsets[UNDERLYING(LevelOfDetail::High)]);
		}

		commandBuffer->DrawIndexed(this, renderComponent->_Model._IndexCounts[UNDERLYING(LevelOfDetail::High)], 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}