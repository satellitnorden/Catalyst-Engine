//Header file.
#include <Rendering/Engine/RenderPasses/DirectionalTerrainShadowRenderPass.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TerrainSystem.h>

//Terrain.
#include <Terrain/TerrainVertex.h>

//Singleton definition.
DEFINE_SINGLETON(DirectionalTerrainShadowRenderPass);

/*
*	Default constructor.
*/
DirectionalTerrainShadowRenderPass::DirectionalTerrainShadowRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		DirectionalTerrainShadowRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the directional terrain shadow render pass.
*/
void DirectionalTerrainShadowRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::DirectionalShadow);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::DirectionalTerrainShadow);

	//Set the shaders.
	SetVertexShader(Shader::DirectionalTerrainShadowVertex);
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
	SetRenderResolution(RenderingSystem::Instance->GetDirectionalShadowMapResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetCullMode(CullMode::Front);
	SetDepthCompareOperator(CompareOperator::Greater);
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
		DirectionalTerrainShadowRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the directional terrain shadow.
*/
void DirectionalTerrainShadowRenderPass::RenderInternal() NOEXCEPT
{
	if (true)
	{
		SetIncludeInRender(false);

		return;
	}

	//Iterate over all terrain render informations and draw them
	const StaticArray<TerrainPatchRenderInformation, TerrainConstants::NUMBER_OF_TERRAIN_PATCHES> *const RESTRICT informations{ TerrainSystem::Instance->GetTerrainPatchRenderInformations() };

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
		if (!TEST_BIT(information._Visibility, VisibilityFlag::Viewer))
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