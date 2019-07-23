//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/TerrainSceneFeaturesGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TerrainSystem.h>

//Terrain.
#include <Terrain/TerrainVertex.h>

/*
*	Terrain vertex push constant data definition.
*/
class TerrainVertexPushConstantData final
{

public:

	//The world position.
	Vector2<float> _WorldPosition;

	//The patch size.
	float _PatchSize;

	//The height texture index.
	int32 _HeightTextureIndex;

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
	SetNumberOfRenderTargets(5);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures1));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures2));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures3));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures4));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Velocity));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(TerrainVertexPushConstantData));

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

	//Iterate over all terrain patches and render them.
	for (const TerrainPatchRenderInformation &information : *TerrainSystem::Instance->GetTerrainPatchRenderInformations())
	{
		//Push constants.
		TerrainVertexPushConstantData vertexData;

		vertexData._WorldPosition = information._WorldPosition;
		vertexData._PatchSize = information._PatchSize;
		vertexData._HeightTextureIndex = information._HeightTextureIndex;

		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(TerrainVertexPushConstantData), &vertexData);

		//Draw the patch!
		commandBuffer->DrawIndexed(this, TerrainSystem::Instance->GetTerrainProperties()->_IndexCount, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}