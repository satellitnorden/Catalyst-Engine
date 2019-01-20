//Header file.
#include <Rendering/Engine/RenderPasses/DynamicPhysicalRenderPass.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>
#include <Rendering/Engine/PhysicalVertex.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DynamicPhysicalRenderPass);

/*
*	Vertex push constant data definition.
*/
class VertexPushConstantData final
{

public:

	Matrix4 _ModelMatrix;

};

/*
*	Fragment push constant data definition.
*/
class FragmentPushConstantData final
{

public:

	int32 _AlbedoTextureIndex;
	int32 _NormalMapTextureIndex;
	int32 _MaterialPropertiesTextureIndex;

};

/*
*	Default constructor.
*/
DynamicPhysicalRenderPass::DynamicPhysicalRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		DynamicPhysicalRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the dynamic physical render pass.
*/
void DynamicPhysicalRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::DynamicPhysical);

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
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(VertexPushConstantData));
	AddPushConstantRange(ShaderStage::Fragment, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData));

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
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::Back);
	SetDepthCompareOperator(CompareOperator::Greater);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
	SetStencilTestEnabled(true);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Replace);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(BIT(0));
	SetStencilReferenceMask(BIT(0));
	SetTopology(Topology::TriangleList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		DynamicPhysicalRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the dynamic physical entities.
*/
void DynamicPhysicalRenderPass::RenderInternal() NOEXCEPT
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

	//Track the previous state, so if two static physical entities share the same state, it doesn't have to be rebound.
	ConstantBufferHandle previousBuffer{ nullptr };
	RenderDataTableHandle previousRenderDataTable{ nullptr };

	for (uint64 i = 0; i < numberOfDynamicPhysicalComponents; ++i, ++renderComponent, ++transformComponent)
	{
		//Don't draw this dynamic physical entity if it isn't in the view frustum or if it's not supposed to be included in this render pass.
		if (!renderComponent->_IsInViewFrustum || !((renderComponent->_PhysicalFlags & PhysicalFlag::Physical) == PhysicalFlag::Physical))
		{
			continue;
		}

		const uint64 offset{ 0 };

		//Push constants.
		VertexPushConstantData vertexData;

		vertexData._ModelMatrix = Matrix4(transformComponent->_Position, transformComponent->_Rotation, transformComponent->_Scale);

		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(VertexPushConstantData), &vertexData);

		FragmentPushConstantData fragmentData;

		fragmentData._AlbedoTextureIndex = renderComponent->_Material._AlbedoTextureIndex;
		fragmentData._NormalMapTextureIndex = renderComponent->_Material._NormalMapTextureIndex;
		fragmentData._MaterialPropertiesTextureIndex = renderComponent->_Material._MaterialPropertiesTextureIndex;

		commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData), &fragmentData);

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