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

class PushConstantData final
{

public:

	Vector2 _PatchWorldPosition;
	float _PatchHalfSize;

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
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::DynamicUniformData));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Terrain));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(PushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(1);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										0);

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
	const StaticArray<TerrainPatchRenderInformation, TerrainConstants::NUMBER_OF_TERRAIN_PATCHES> *const RESTRICT informations{ TerrainSystem::Instance->GetTerrainPatchRenderInformations() };

	//Cache the command buffer
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the current dynamic uniform data render data table.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetCurrentDynamicUniformDataRenderDataTable());

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

		//Bind the normal texture.
		commandBuffer->BindRenderDataTable(this, 1, information._RenderDataTable);

		//Push constants.
		PushConstantData data;

		data._PatchWorldPosition = information._WorldPosition;
		data._PatchHalfSize = information._PatchSize;

		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(PushConstantData), &data);

		commandBuffer->DrawIndexed(this, TerrainSystem::Instance->GetTerrainProperties()->_IndexCount, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}