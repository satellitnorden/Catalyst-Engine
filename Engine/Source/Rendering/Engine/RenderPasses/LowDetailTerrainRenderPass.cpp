//Header file.
#include <Rendering/Engine/RenderPasses/LowDetailTerrainRenderPass.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TerrainSystem.h>

//Terrain.
#include <Terrain/TerrainVertex.h>

//Singleton definition.
DEFINE_SINGLETON(LowDetailTerrainRenderPass);

/*
*	Default constructor.
*/
LowDetailTerrainRenderPass::LowDetailTerrainRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		LowDetailTerrainRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the low detail terrain render pass.
*/
void LowDetailTerrainRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::LowDetailTerrain);

	//Set the shaders.
	SetVertexShader(Shader::LowDetailTerrainVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
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

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(3);
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
	SetCullMode(CullMode::Front);
	SetDepthCompareOperator(CompareOperator::Less);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
	SetTopology(Topology::TriangleList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		LowDetailTerrainRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the low detail terrain.
*/
void LowDetailTerrainRenderPass::RenderInternal() NOEXCEPT
{
	//Iterate over all low detail terrain render informations and draw them
	const StaticArray<TerrainPatchRenderInformation, 32> *const RESTRICT informations{ TerrainSystem::Instance->GetLowDetailTerrainPatchRenderInformations() };

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
		if (!information._Draw)
		{
			continue;
		}

		const uint64 offset{ 0 };

		commandBuffer->BindVertexBuffer(this, 0, information._Buffer, &offset);
		commandBuffer->BindIndexBuffer(this, information._Buffer, information._IndexOffset);
		commandBuffer->BindRenderDataTable(this, 1, information._RenderDataTable);

		commandBuffer->DrawIndexed(this, information._IndexCount, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}