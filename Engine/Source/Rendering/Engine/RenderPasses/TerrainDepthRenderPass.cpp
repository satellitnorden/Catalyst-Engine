//Header file.
#include <Rendering/Engine/RenderPasses/TerrainDepthRenderPass.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TerrainSystem.h>

//Terrain.
#include <Terrain/TerrainVertex.h>

//Singleton definition.
DEFINE_SINGLETON(TerrainDepthRenderPass);

class VertexPushConstantData final
{

public:

	Vector2 _PatchWorldPosition;
	float _PatchHalfSize;
	int32 _Borders;
	int32 _HeightTextureIndex;

};

class FragmentPushConstantData final
{

public:

	int32 _NormalTextureIndex;
	int32 _LayerWeightsTextureIndex;

};

/*
*	Default constructor.
*/
TerrainDepthRenderPass::TerrainDepthRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		TerrainDepthRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the terrain depth render pass.
*/
void TerrainDepthRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::TerrainDepth);

	//Set the shaders.
	SetVertexShader(Shader::TerrainVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::TerrainDepthFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(2);
	AddRenderTarget(RenderTarget::SceneBufferNormalDepth);
	AddRenderTarget(RenderTarget::SceneIntermediate);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(VertexPushConstantData));
	AddPushConstantRange(ShaderStage::Fragment, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(2);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										0);
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32SignedInt,
										sizeof(Vector2));

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(TerrainVertex), VertexInputBindingDescription::InputRate::Vertex);

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
	SetStencilWriteMask(BIT(0) | BIT(1));
	SetStencilReferenceMask(BIT(0) | BIT(1));
	SetTopology(Topology::TriangleList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		TerrainDepthRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the depth of the terrain.
*/
void TerrainDepthRenderPass::RenderInternal() NOEXCEPT
{
	//Iterate over all terrain render informations and draw them if they're low detail.
	const DynamicArray<TerrainPatchRenderInformation> *const RESTRICT informations{ TerrainSystem::Instance->GetTerrainPatchRenderInformations() };

	//Cache the command buffer
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Bind vertex/inder buffer.
	constexpr uint64 offset{ 0 };

	commandBuffer->BindVertexBuffer(this, 0, TerrainSystem::Instance->GetTerrainProperties()->_Buffer, &offset);
	commandBuffer->BindIndexBuffer(this, TerrainSystem::Instance->GetTerrainProperties()->_Buffer, TerrainSystem::Instance->GetTerrainProperties()->_IndexOffset);

	//Wait for terrain culling to finish.
	CullingSystem::Instance->WaitForTerrainCulling();

	for (const TerrainPatchRenderInformation &information : *informations)
	{
		if (!TEST_BIT(information._Visibility, VisibilityFlag::Viewer))
		{
			continue;
		}

		//Push constants.
		VertexPushConstantData vertexData;

		vertexData._PatchWorldPosition = information._WorldPosition;
		vertexData._PatchHalfSize = information._PatchSize;
		vertexData._Borders = information._Borders;
		vertexData._HeightTextureIndex = information._HeightTextureIndex;

		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(VertexPushConstantData), &vertexData);

		FragmentPushConstantData fragmentData;

		fragmentData._NormalTextureIndex = information._NormalTextureIndex;
		fragmentData._LayerWeightsTextureIndex = information._LayerWeightsTextureIndex;

		commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData), &fragmentData);

		commandBuffer->DrawIndexed(this, TerrainSystem::Instance->GetTerrainProperties()->_IndexCount, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}