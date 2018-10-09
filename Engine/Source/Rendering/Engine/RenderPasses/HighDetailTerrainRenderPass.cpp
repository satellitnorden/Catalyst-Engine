//Header file.
#include <Rendering/Engine/RenderPasses/HighDetailTerrainRenderPass.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TerrainSystem.h>

//Terrain.
#include <Terrain/TerrainVertex.h>

//Singleton definition.
DEFINE_SINGLETON(HighDetailTerrainRenderPass);

/*
*	Default constructor.
*/
HighDetailTerrainRenderPass::HighDetailTerrainRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		HighDetailTerrainRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the high detail terrain render pass.
*/
void HighDetailTerrainRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::HighDetailTerrain);

	//Set the shaders.
	SetVertexShader(Shader::HighDetailTerrainVertex);
	SetTessellationControlShader(Shader::HighDetailTerrainTessellationControl);
	SetTessellationEvaluationShader(Shader::HighDetailTerrainTessellationEvaluation);
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
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::TerrainMaterial));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::TessellationEvaluation, 0, sizeof(TerrainDisplacementInformation));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(4);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										0);
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										sizeof(float) * 3);
	AddVertexInputAttributeDescription(	2,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(float) * 6);

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(TerrainVertex), VertexInputBindingDescription::InputRate::Vertex);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetCullMode(CullMode::Back);
	SetDepthCompareOperator(CompareOperator::Less);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
	SetTopology(Topology::PatchList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		HighDetailTerrainRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the high detail terrain.
*/
void HighDetailTerrainRenderPass::RenderInternal() NOEXCEPT
{
	//Iterate over all high detail terrain render informations and draw them
	const StaticArray<TerrainPatchRenderInformation, 9> *const RESTRICT informations{ TerrainSystem::Instance->GetHighDetailTerrainPatchRenderInformations() };

	//Cache the command buffer
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the current dynamic uniform data render data table.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetCurrentDynamicUniformDataRenderDataTable());

	//Wait for terrain culling to finish.
	CullingSystem::Instance->WaitForTerrainCulling();

	for (const TerrainPatchRenderInformation &information : *informations)
	{
		if (!information._Draw || !information._HighDetail)
		{
			continue;
		}

		const uint64 offset{ 0 };

		commandBuffer->BindVertexBuffer(this, 0, information._Buffer, &offset);
		commandBuffer->BindIndexBuffer(this, information._Buffer, information._IndexOffset);
		commandBuffer->BindRenderDataTable(this, 1, TerrainSystem::Instance->GetTerrainProperties()->_RenderDataTable);
		commandBuffer->PushConstants(this, ShaderStage::TessellationEvaluation, 0, sizeof(TerrainDisplacementInformation), &information._DisplacementInformation);

		commandBuffer->DrawIndexed(this, information._IndexCount, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}