//Header file.
#include <Rendering/Engine/RenderPasses/TerrainRenderPass.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TerrainSystem.h>

//Terrain.
#include <Terrain/TerrainVertex.h>

//Singleton definition.
DEFINE_SINGLETON(TerrainRenderPass);

/*
*	Default constructor.
*/
TerrainRenderPass::TerrainRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		TerrainRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the terrain render pass.
*/
void TerrainRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::Terrain);

	//Set the sub stage index.
	SetSubStageIndex(0);

	//Set the shaders.
	SetVertexShader(Shader::TerrainVertex);
	SetTessellationControlShader(Shader::TerrainTessellationControl);
	SetTessellationEvaluationShader(Shader::TerrainTessellationEvaluation);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::TerrainFragment);

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
	AddVertexInputAttributeDescription(	3,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										sizeof(float) * 10);

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
		TerrainRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the terrain.
*/
void TerrainRenderPass::RenderInternal() NOEXCEPT
{
	//Iterate over all terrain render informations and draw them
	const DynamicArray<TerrainRenderInformation> *const RESTRICT informations{ TerrainSystem::Instance->GetTerrainRenderInformation() };

	//If there's none to render - render none.
	if (informations->Empty())
	{
		//Don't include this render pass in the final render.
		SetIncludeInRender(false);

		return;
	}

	//Cache the command buffer
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the current dynamic uniform data render data table.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetCurrentDynamicUniformDataRenderDataTable());

	for (const TerrainRenderInformation &information : *informations)
	{
		const uint64 offset{ 0 };

		commandBuffer->BindVertexBuffers(this, 1, &information._Buffer, &offset);
		commandBuffer->BindIndexBuffer(this, information._Buffer, information._IndexOffset);
		commandBuffer->BindRenderDataTable(this, 1, information._RenderDataTable);
		commandBuffer->PushConstants(this, ShaderStage::TessellationEvaluation, 0, sizeof(TerrainDisplacementInformation), &information._DisplacementInformation);

		commandBuffer->DrawIndexed(this, information._IndexCount, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}