//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/TerrainSceneFeaturesGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TerrainSystem.h>

//Terrain.
#include <Terrain/TerrainVertex.h>

/*
*	Terrain push constant data definition.
*/
class TerrainPushConstantData final
{

public:

	//The world position.
	Vector2<float> _WorldPosition;

	//The patch size.
	float _PatchSize;

	//The borders
	int32 _Borders;

	//The height map texture index.
	int32 _HeightMapTextureIndex;

	//The index map texture index.
	int32 _IndexMapTextureIndex;

	//The blend map texture index.
	int32 _BlendMapTextureIndex;

	//The height map resolution
	float _HeightMapResolution;

};

/*
*	Initializes this graphics pipeline.
*/
void TerrainSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depthBuffer) NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(Shader::TerrainSceneFeaturesVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::TerrainSceneFeaturesFragment);

	//Set the depth buffer.
	SetDepthBuffer(depthBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(4);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures1));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures2));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures3));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures4));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Vertex | ShaderStage::Fragment, 0, sizeof(TerrainPushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(2);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										offsetof(TerrainVertex, _Position));
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32SignedInt,
										offsetof(TerrainVertex, _Borders));

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(TerrainVertex), VertexInputBindingDescription::InputRate::Vertex);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
	SetShouldClear(false);
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
	SetStencilWriteMask(RenderingConstants::SCENE_BUFFER_STENCIL_BIT);
	SetStencilReferenceMask(RenderingConstants::SCENE_BUFFER_STENCIL_BIT);
	SetTopology(Topology::TriangleList);
}

/*
*	Executes this graphics pipeline.
*/
void TerrainSceneFeaturesGraphicsPipeline::Execute() NOEXCEPT
{
	//Define constants.
	constexpr uint64 OFFSET{ 0 };

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Bind the vertex/index buffer.
	commandBuffer->BindVertexBuffer(this, 0, TerrainSystem::Instance->GetTerrainProperties()->_Buffer, &OFFSET);
	commandBuffer->BindIndexBuffer(this, TerrainSystem::Instance->GetTerrainProperties()->_Buffer, TerrainSystem::Instance->GetTerrainProperties()->_IndexOffset);

	//Wait for terrain culling to finish.
	CullingSystem::Instance->WaitForTerrainCulling();

	//Iterate over all terrain patches and render them.
	for (const TerrainPatchRenderInformation &information : *TerrainSystem::Instance->GetTerrainPatchRenderInformations())
	{
		//Check the visibility of this patch.
		if (!information._Visibility)
		{
			continue;
		}

		//Push constants.
		TerrainPushConstantData data;

		data._WorldPosition = information._WorldPosition;
		data._PatchSize = information._PatchSize;
		data._Borders = information._Borders;
		data._HeightMapTextureIndex = TerrainSystem::Instance->GetTerrainProperties()->_HeightMapTextureIndex;
		data._IndexMapTextureIndex = TerrainSystem::Instance->GetTerrainProperties()->_IndexMapTextureIndex;
		data._BlendMapTextureIndex = TerrainSystem::Instance->GetTerrainProperties()->_BlendMapTextureIndex;
		data._HeightMapResolution = static_cast<float>(TerrainSystem::Instance->GetTerrainProperties()->_HeightMap.GetWidth());

		commandBuffer->PushConstants(this, ShaderStage::Vertex | ShaderStage::Fragment, 0, sizeof(TerrainPushConstantData), &data);

		//Draw the patch!
		commandBuffer->DrawIndexed(this, TerrainSystem::Instance->GetTerrainProperties()->_IndexCount, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}