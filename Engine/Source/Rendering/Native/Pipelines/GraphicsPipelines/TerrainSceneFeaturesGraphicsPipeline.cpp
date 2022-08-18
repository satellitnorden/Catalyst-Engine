//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/TerrainSceneFeaturesGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/RenderPasses/SceneFeaturesRenderPass.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/TerrainSystem.h>
#include <Systems/WorldSystem.h>

//Terrain.
#include <Terrain/TerrainVertex.h>

/*
*	Terrain push constant data definition.
*/
class TerrainPushConstantData final
{

public:
	
	//The world grid delta.
	Vector3<float32> _WorldGridDelta;

	//Some padding.
	Padding<4> _Padding;

	//The world position.
	Vector2<float32> _WorldPosition;

	//The patch size.
	float32 _PatchSize;

	//The borders
	int32 _Borders;

	//The vertex border offset first.
	float32 _VertexBorderOffsetFirst;

	//The vertex border offset second.
	float32 _VertexBorderOffsetSecond;

	//The height map texture index.
	uint32 _HeightMapTextureIndex;

	//The normal map texture index.
	uint32 _NormalMapTextureIndex;

	//The index map texture index.
	uint32 _IndexMapTextureIndex;

	//The blend map texture index.
	uint32 _BlendMapTextureIndex;

	//The height map resolution reciprocal.
	float32 _HeightMapResolutionReciprocal;

	//The meter per height map texel.
	float32 _MeterPerHgithMapTexel;

	//The material maps resolution
	float32 _MaterialMapsResolution;

};

/*
*	Initializes this graphics pipeline.
*/
void TerrainSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depthBuffer) NOEXCEPT
{
	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("TerrainSceneFeaturesVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("TerrainSceneFeaturesFragmentShader")));

	//Set the depth buffer.
	SetDepthBuffer(depthBuffer);

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(4);
	AddOutputRenderTarget(SceneFeaturesRenderPass::Instance->GetSceneFeatures1RenderTarget());
	AddOutputRenderTarget(SceneFeaturesRenderPass::Instance->GetSceneFeatures2RenderTarget());
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_3));
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_4));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(TerrainPushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(3);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										offsetof(TerrainVertex, _Position));
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										offsetof(TerrainVertex, _TextureCoordinate));
	AddVertexInputAttributeDescription(	2,
										0,
										VertexInputAttributeDescription::Format::X32SignedInt,
										offsetof(TerrainVertex, _Borders));

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(TerrainVertex), VertexInputBindingDescription::InputRate::Vertex);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution(0));

	//Set the properties of the render pass.
	SetDepthStencilAttachmentLoadOperator(AttachmentLoadOperator::LOAD);
	SetDepthStencilAttachmentStoreOperator(AttachmentStoreOperator::STORE);
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::LOAD);
	SetColorAttachmentStoreOperator(AttachmentStoreOperator::STORE);
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

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName("Terrain Scene Features");
#endif
}

/*
*	Executes this graphics pipeline.
*/
void TerrainSceneFeaturesGraphicsPipeline::Execute() NOEXCEPT
{
	//Define constants.
	constexpr uint64 OFFSET{ 0 };

	//Is everything that is needed to render terrain available?
	if (!TerrainSystem::Instance->GetTerrainProperties()->_TerrainHeightFunction
		|| !TerrainSystem::Instance->GetTerrainProperties()->_TerrainMaterialFunction)
	{
		SetIncludeInRender(false);

		return;
	}

	//Retrieve and set the command buffer.
	CommandBuffer *const RESTRICT command_buffer{ RenderingSystem::Instance->GetGlobalCommandBuffer(CommandBufferLevel::SECONDARY) };
	SetCommandBuffer(command_buffer);

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Bind the vertex/index buffer.
	command_buffer->BindVertexBuffer(this, 0, TerrainSystem::Instance->GetTerrainProperties()->_Buffer, &OFFSET);
	command_buffer->BindIndexBuffer(this, TerrainSystem::Instance->GetTerrainProperties()->_Buffer, TerrainSystem::Instance->GetTerrainProperties()->_IndexOffset);

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

		const Vector3<int32> grid_delta{ Vector3<int32>(0, 0, 0) - WorldSystem::Instance->GetCurrentWorldGridCell() };

		data._WorldGridDelta._X = static_cast<float32>(grid_delta._X) * WorldSystem::Instance->GetWorldGridSize();
		data._WorldGridDelta._Y = static_cast<float32>(grid_delta._Y) * WorldSystem::Instance->GetWorldGridSize();
		data._WorldGridDelta._Z = static_cast<float32>(grid_delta._Z) * WorldSystem::Instance->GetWorldGridSize();
		data._WorldPosition = information._WorldPosition;
		data._PatchSize = information._PatchSize;
		data._Borders = information._Borders;
		data._VertexBorderOffsetFirst = 1.0f / static_cast<float32>(TerrainSystem::Instance->GetTerrainProperties()->_PatchResolution);
		data._VertexBorderOffsetSecond = 1.0f / static_cast<float32>((TerrainSystem::Instance->GetTerrainProperties()->_PatchResolution) / 2);
		data._NormalMapTextureIndex = information._NormalMapTextureIndex;
		data._HeightMapTextureIndex = information._HeightMapTextureIndex;
		data._IndexMapTextureIndex = information._IndexMapTextureIndex;
		data._BlendMapTextureIndex = information._BlendMapTextureIndex;
		data._HeightMapResolutionReciprocal = 1.0f / static_cast<float32>(TerrainSystem::Instance->GetTerrainProperties()->_PatchResolution);
		data._MeterPerHgithMapTexel = information._PatchSize * data._HeightMapResolutionReciprocal;
		data._MaterialMapsResolution = static_cast<float32>(information._MaterialMapsResolution);

		command_buffer->PushConstants(this, ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(TerrainPushConstantData), &data);

		//Draw the patch!
		command_buffer->DrawIndexed(this, TerrainSystem::Instance->GetTerrainProperties()->_IndexCount, 1);
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}

/*
*	Terminates this graphics pipeline.
*/
void TerrainSceneFeaturesGraphicsPipeline::Terminate() NOEXCEPT
{

}