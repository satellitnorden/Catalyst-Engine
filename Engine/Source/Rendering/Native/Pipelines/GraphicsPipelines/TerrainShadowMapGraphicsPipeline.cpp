//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/TerrainShadowMapGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Vertex.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/TerrainSystem.h>
#include <Systems/WorldSystem.h>

//Terrain.
#include <Terrain/TerrainVertex.h>

/*
*	Terrain shadow push constant data definition.
*/
class TerrainShadowPushConstantData final
{

public:

	//The world to light matrix.
	Matrix4x4 _WorldToLightMatrix;

	//The world position.
	Vector3<float32> _WorldPosition;

	//Some padding.
	Padding<4> _Padding;

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
void TerrainShadowMapGraphicsPipeline::Initialize(const DepthBufferHandle depth_buffer, const RenderTargetHandle render_target) NOEXCEPT
{
	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("TerrainShadowVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("TerrainShadowFragmentShader")));

	//Set the depth buffer.
	SetDepthBuffer(depth_buffer);

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(1);
	AddOutputRenderTarget(render_target);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::VERTEX, 0, sizeof(TerrainShadowPushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(2);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(TerrainVertex, _Position));
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										offsetof(TerrainVertex, _TextureCoordinate));

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(TerrainVertex), VertexInputBindingDescription::InputRate::Vertex);

	//Set the render resolution.
	SetRenderResolution(Resolution(CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution, CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution));

	//Set the properties of the render pass.
	SetDepthStencilAttachmentLoadOperator(AttachmentLoadOperator::CLEAR);
	SetDepthStencilAttachmentStoreOperator(AttachmentStoreOperator::STORE);
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::CLEAR);
	SetColorAttachmentStoreOperator(AttachmentStoreOperator::STORE);
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::Back);
	SetDepthCompareOperator(CompareOperator::Less);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
	SetStencilTestEnabled(false);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Replace);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(0);
	SetTopology(Topology::TriangleList);

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName("Terrain Shadow Map");
#endif
}

/*
*	Executes this graphics pipeline.
*/
void TerrainShadowMapGraphicsPipeline::Execute(const Matrix4x4 &world_to_light_matrix) NOEXCEPT
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

	//Draw all terrain entites.
	{
		//Cache relevant data.
		const uint64 number_of_components{ ComponentManager::GetNumberOfTerrainComponents() };
		const TerrainGeneralComponent* RESTRICT general_component{ ComponentManager::GetTerrainTerrainGeneralComponents() };
		const TerrainRenderComponent* RESTRICT render_component{ ComponentManager::GetTerrainTerrainRenderComponents() };

		for (uint64 component_index{ 0 }; component_index < number_of_components; ++component_index, ++general_component, ++render_component)
		{
			//Push constants.
			TerrainShadowPushConstantData data;

			data._WorldToLightMatrix = world_to_light_matrix;
			data._WorldPosition = general_component->_WorldPosition.GetRelativePosition(WorldSystem::Instance->GetCurrentWorldGridCell());
			data._NormalMapTextureIndex = render_component->_NormalMapTextureIndex;
			data._IndexMapTextureIndex = render_component->_IndexMapTextureIndex;
			data._BlendMapTextureIndex = render_component->_BlendMapTextureIndex;
			data._MapResolution = static_cast<float32>(general_component->_HeightMap.GetResolution());
			data._MapResolutionReciprocal = 1.0f / data._MapResolution;

			command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(TerrainShadowPushConstantData), &data);

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
void TerrainShadowMapGraphicsPipeline::Terminate() NOEXCEPT
{

}