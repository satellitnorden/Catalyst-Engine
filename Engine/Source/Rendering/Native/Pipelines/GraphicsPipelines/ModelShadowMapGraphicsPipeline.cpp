//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ModelShadowMapGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Vertex.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

/*
*	Model shadow map vertex push constant data definition.
*/
class ModelShadowMapVertexPushConstantData final
{

public:

	Matrix4x4 _ModelToWorldMatrix;
	Matrix4x4 _WorldToLightMatrix;

};

/*
*	Initializes this graphics pipeline.
*/
void ModelShadowMapGraphicsPipeline::Initialize(const DepthBufferHandle depth_buffer) NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("ModelShadowMapVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("ModelShadowMapFragmentShader")));

	//Set the depth buffer.
	SetDepthBuffer(depth_buffer);

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(1);
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SHADOW_MAP));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::VERTEX, 0, sizeof(ModelShadowMapVertexPushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(4);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(Vertex, _Position));
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(Vertex, _Normal));
	AddVertexInputAttributeDescription(	2,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(Vertex, _Tangent));
	AddVertexInputAttributeDescription(	3,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										offsetof(Vertex, _TextureCoordinate));

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(Vertex), VertexInputBindingDescription::InputRate::Vertex);

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
void ModelShadowMapGraphicsPipeline::Execute(const Matrix4x4 &world_to_light_matrix) NOEXCEPT
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

	//Draw static models
	{
		//Cache relevant data.
		const uint64 number_of_components{ ComponentManager::GetNumberOfStaticModelComponents() };
		const StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

		//Wait for static models level of detail to finish.
		LevelOfDetailSystem::Instance->WaitForStaticModelsLevelOfDetail();

		for (uint64 i = 0; i < number_of_components; ++i, ++component)
		{
			//Draw all meshes.
			for (uint64 i{ 0 }, size{ component->_ModelResource->_Meshes.Size() }; i < size; ++i)
			{
				//Cache the mesh.
				const Mesh& mesh{ component->_ModelResource->_Meshes[i] };

				//Push constants.
				ModelShadowMapVertexPushConstantData data;

				data._ModelToWorldMatrix = component->_WorldTransform;
				data._WorldToLightMatrix = world_to_light_matrix;

				command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(ModelShadowMapVertexPushConstantData), &data);

				//Bind the vertex/inder buffer.
				command_buffer->BindVertexBuffer(this, 0, mesh._VertexBuffers[component->_LevelOfDetailIndices[i]], &OFFSET);
				command_buffer->BindIndexBuffer(this, mesh._IndexBuffers[component->_LevelOfDetailIndices[i]], OFFSET);

				//Draw!
				command_buffer->DrawIndexed(this, mesh._IndexCounts[component->_LevelOfDetailIndices[i]], 1);
			}
		}
	}

	//Draw dynamic models.
	{
		//Cache relevant data.
		const uint64 number_of_components{ ComponentManager::GetNumberOfDynamicModelComponents() };
		const DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

		//Wait for dynamic models level of detail to finish.
		LevelOfDetailSystem::Instance->WaitForDynamicModelsLevelOfDetail();

		for (uint64 i = 0; i < number_of_components; ++i, ++component)
		{
			//Draw all meshes.
			for (uint64 i{ 0 }, size{ component->_ModelResource->_Meshes.Size() }; i < size; ++i)
			{
				//Cache the mesh.
				const Mesh& mesh{ component->_ModelResource->_Meshes[i] };

				//Push constants.
				ModelShadowMapVertexPushConstantData data;

				data._ModelToWorldMatrix = component->_CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
				data._WorldToLightMatrix = world_to_light_matrix;

				command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(ModelShadowMapVertexPushConstantData), &data);

				//Bind the vertex/inder buffer.
				command_buffer->BindVertexBuffer(this, 0, mesh._VertexBuffers[component->_LevelOfDetailIndices[i]], &OFFSET);
				command_buffer->BindIndexBuffer(this, mesh._IndexBuffers[component->_LevelOfDetailIndices[i]], OFFSET);

				//Draw!
				command_buffer->DrawIndexed(this, mesh._IndexCounts[component->_LevelOfDetailIndices[i]], 1);
			}
		}
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}