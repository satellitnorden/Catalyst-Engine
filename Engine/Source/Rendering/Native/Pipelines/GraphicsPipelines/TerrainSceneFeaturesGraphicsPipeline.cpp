//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/TerrainSceneFeaturesGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

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
class TerrainSceneFeaturesPushConstantData final
{

public:
	
	//The world position.
	Vector3<float32> _WorldPosition;

	//Some padding.
	Padding<4> _Padding;

	//The patch size.
	float32 _PatchSize;

	//The borders
	uint32 _Borders;

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

	//The map resolution.
	float32 _MapResolution;

	//The map resolution reciprocal.
	float32 _MapResolutionReciprocal;

};

/*
*	Initializes this graphics pipeline.
*/
void TerrainSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depth_buffer) NOEXCEPT
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
	SetDepthBuffer(depth_buffer);

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(4);
	AddOutputRenderTarget(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_1));
	AddOutputRenderTarget(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_2));
	AddOutputRenderTarget(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_3));
	AddOutputRenderTarget(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_4));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(TerrainSceneFeaturesPushConstantData));

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

	//Retrieve and set the command buffer.
	CommandBuffer *const RESTRICT command_buffer{ RenderingSystem::Instance->GetGlobalCommandBuffer(CommandBufferLevel::SECONDARY) };
	SetCommandBuffer(command_buffer);

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Draw static models.
	{
		//Cache relevant data.
		const uint64 number_of_components{ ComponentManager::GetNumberOfTerrainComponents() };
		const TerrainGeneralComponent *RESTRICT general_component{ ComponentManager::GetTerrainTerrainGeneralComponents() };
		const TerrainRenderComponent *RESTRICT render_component{ ComponentManager::GetTerrainTerrainRenderComponents() };

		for (uint64 component_index{ 0 }; component_index < number_of_components; ++component_index, ++general_component, ++render_component)
		{
			//Push constants.
			TerrainSceneFeaturesPushConstantData data;

			const Vector3<int32> grid_delta{ Vector3<int32>(0, 0, 0) - WorldSystem::Instance->GetCurrentWorldGridCell() };

			data._WorldPosition = general_component->_WorldPosition.GetRelativePosition(WorldSystem::Instance->GetCurrentWorldGridCell());
			data._PatchSize = static_cast<float32>(general_component->_PatchSize);
			data._Borders = 0;
			data._VertexBorderOffsetFirst = 1.0f / static_cast<float32>(general_component->_HeightMap.GetResolution());
			data._VertexBorderOffsetSecond = 1.0f / static_cast<float32>((general_component->_HeightMap.GetResolution()) / 2);
			data._HeightMapTextureIndex = render_component->_HeightMapTextureIndex;
			data._NormalMapTextureIndex = render_component->_NormalMapTextureIndex;
			data._IndexMapTextureIndex = render_component->_IndexMapTextureIndex;
			data._BlendMapTextureIndex = render_component->_BlendMapTextureIndex;
			data._MapResolution = static_cast<float32>(general_component->_HeightMap.GetResolution());
			data._MapResolutionReciprocal = 1.0f / data._MapResolution;

			command_buffer->PushConstants(this, ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(TerrainSceneFeaturesPushConstantData), &data);

			//Bind the vertex/index buffer.
			command_buffer->BindVertexBuffer(this, 0, render_component->_Buffer, &OFFSET);
			command_buffer->BindIndexBuffer(this, render_component->_Buffer, render_component->_IndexOffset);

			//Draw!
			command_buffer->DrawIndexed(this, render_component->_IndexCount, 1);
		}
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