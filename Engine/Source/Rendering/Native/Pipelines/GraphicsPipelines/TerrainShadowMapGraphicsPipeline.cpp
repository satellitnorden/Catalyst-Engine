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

	//The world grid delta.
	Vector3<float32> _WorldGridDelta;

	//Some padding.
	Padding<4> _Padding;

	//The world position.
	Vector2<float32> _WorldPosition;

	//The height map coordinate offset.
	Vector2<float32> _HeightMapCoordinateOffset;

	//The patch size.
	float32 _PatchSize;

	//The borders
	int32 _Borders;

	//The vertex border offset first.
	float32 _VertexBorderOffsetFirst;

	//The vertex border offset second.
	float32 _VertexBorderOffsetSecond;

};

/*
*	Initializes this graphics pipeline.
*/
void TerrainShadowMapGraphicsPipeline::Initialize(const DepthBufferHandle depth_buffer, const RenderTargetHandle render_target) NOEXCEPT
{
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
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::VERTEX, 0, sizeof(TerrainShadowPushConstantData));

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
	SetRenderResolution(Resolution(CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution, CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution));

	//Set the properties of the render pass.
	SetShouldClear(true);
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::Front);
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
}

/*
*	Executes this graphics pipeline.
*/
void TerrainShadowMapGraphicsPipeline::Execute(const Matrix4x4 &world_to_light_matrix) NOEXCEPT
{
	//Define constants.
	constexpr uint64 OFFSET{ 0 };

	//Is everything that is needed to render terrain available?
	if (!TerrainSystem::Instance->GetTerrainProperties()->_HasWorldCenter
		|| !TerrainSystem::Instance->GetTerrainProperties()->_HasHeightMap
		|| !TerrainSystem::Instance->GetTerrainProperties()->_HasIndexMap
		|| !TerrainSystem::Instance->GetTerrainProperties()->_HasBlendMap)
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

	//Iterate over all terrain patches and render them.
	for (const TerrainPatchRenderInformation &information : *TerrainSystem::Instance->GetTerrainPatchRenderInformations())
	{
		//Push constants.
		TerrainShadowPushConstantData data;

		data._WorldToLightMatrix = world_to_light_matrix;
		{
			SCOPED_LOCK(TerrainSystem::Instance->GetTerrainProperties()->_WorldCenterLock);

			const Vector3<int32> grid_delta{ Vector3<int32>(0, 0, 0) - WorldSystem::Instance->GetCurrentWorldGridCell() };

			data._WorldGridDelta._X = static_cast<float32>(grid_delta._X) * WorldSystem::Instance->GetWorldGridSize();
			data._WorldGridDelta._Y = static_cast<float32>(grid_delta._Y) * WorldSystem::Instance->GetWorldGridSize();
			data._WorldGridDelta._Z = static_cast<float32>(grid_delta._Z) * WorldSystem::Instance->GetWorldGridSize();
		}
		data._WorldPosition = information._WorldPosition;
		{
			SCOPED_LOCK(TerrainSystem::Instance->GetTerrainProperties()->_WorldCenterLock);

			data._HeightMapCoordinateOffset._X = TerrainSystem::Instance->GetTerrainProperties()->_WorldCenter.GetAbsolutePosition()._X;
			data._HeightMapCoordinateOffset._Y = TerrainSystem::Instance->GetTerrainProperties()->_WorldCenter.GetAbsolutePosition()._Z;
		}
		data._PatchSize = information._PatchSize;
		data._Borders = information._Borders;
		data._VertexBorderOffsetFirst = 1.0f / static_cast<float32>(TerrainSystem::Instance->GetTerrainProperties()->_PatchResolution - 1);
		data._VertexBorderOffsetSecond = 1.0f / static_cast<float32>((TerrainSystem::Instance->GetTerrainProperties()->_PatchResolution - 1) / 2);

		command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(TerrainShadowPushConstantData), &data);

		//Draw the patch!
		command_buffer->DrawIndexed(this, TerrainSystem::Instance->GetTerrainProperties()->_IndexCount, 1);
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}