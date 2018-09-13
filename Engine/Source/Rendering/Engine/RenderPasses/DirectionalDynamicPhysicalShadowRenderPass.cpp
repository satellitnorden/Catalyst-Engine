//Header file.
#include <Rendering/Engine/RenderPasses/DirectionalDynamicPhysicalShadowRenderPass.h>

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
	SetMainStage(RenderPassMainStage::DirectionalShadow);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::DirectionalDynamicPhysicalShadow);

	//Set the sub stage index.
	SetSubStageIndex(1);

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
	SetRenderResolution(Resolution(EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution, EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution));

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetCullMode(CullMode::Back);
	SetDepthCompareOperator(CompareOperator::Less);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
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

	//Bind the render data table.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetCurrentDynamicUniformDataRenderDataTable());

	//Track the previous state, so if two dynamic physical entities share the same state, it doesn't have to be rebound.
	ConstantBufferHandle previousBuffer{ nullptr };
	RenderDataTableHandle previousRenderDataTable{ nullptr };

	for (uint64 i = 0; i < numberOfDynamicPhysicalComponents; ++i, ++renderComponent, ++transformComponent)
	{
		const uint64 offset{ 0 };

		Matrix4 modelMatrix{ transformComponent->_Position, transformComponent->_Rotation, transformComponent->_Scale };
		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(Matrix4), &modelMatrix);

		if (previousBuffer != renderComponent->_Buffer)
		{
			previousBuffer = renderComponent->_Buffer;

			commandBuffer->BindVertexBuffers(this, 1, &renderComponent->_Buffer, &offset);
			commandBuffer->BindIndexBuffer(this, renderComponent->_Buffer, renderComponent->_IndexOffset);
		}

		if (previousRenderDataTable != renderComponent->_RenderDataTable)
		{
			previousRenderDataTable = renderComponent->_RenderDataTable;

			commandBuffer->BindRenderDataTable(this, 1, renderComponent->_RenderDataTable);
		}

		commandBuffer->DrawIndexed(this, renderComponent->_IndexCount, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}