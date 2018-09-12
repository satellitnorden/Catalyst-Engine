//Header file.
#include <Rendering/Engine/RenderPasses/DirectionalTerrainShadowRenderPass.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/EngineSystem.h>
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/RenderingSystem.h>

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

	//Set the sub stage index.
	SetSubStageIndex(0);

	//Set the shaders.
	SetVertexShader(Shader::TerrainVertex);
	SetTessellationControlShader(Shader::TerrainTessellationControl);
	SetTessellationEvaluationShader(Shader::DirectionalTerrainShadowTessellationEvaluation);
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
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Terrain));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(2);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										0);
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										sizeof(float) * 3);

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(float) * 5, VertexInputBindingDescription::InputRate::Vertex);

	//Set the render resolution.
	SetRenderResolution(Resolution(EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution, EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution));

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
	//Iterate over all terrain components and draw them all.
	const uint64 numberOfTerrainComponents{ ComponentManager::GetNumberOfTerrainComponents() };

	//If there's none to render - render none.
	if (numberOfTerrainComponents == 0)
	{
		//Don't include this render pass in the final render.
		SetIncludeInRender(false);

		return;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };
	const TerrainRenderComponent *RESTRICT component{ ComponentManager::GetTerrainTerrainRenderComponents() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data table.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetCurrentDynamicUniformDataRenderDataTable());

	//Wait for the terrain level of detail to finish.
	LevelOfDetailSystem::Instance->WaitForTerrainLevelOfDetail();

	for (uint64 i = 0; i < numberOfTerrainComponents; ++i, ++component)
	{
		const uint64 offset{ 0 };

		commandBuffer->BindRenderDataTable(this, 1, component->_RenderDataTable);
		commandBuffer->BindVertexBuffers(this, 1, &component->_Buffer, &offset);
		commandBuffer->BindIndexBuffer(this, component->_Buffer, component->_IndexOffset);
		commandBuffer->DrawIndexed(this, component->_IndexCount, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}