//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/SimplifiedModelGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Vertex.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Simplified model push constant data class definition.
*/
class SimplifiedModelPushConstantData final
{

public:

	//The model matrix.
	Matrix4x4 _ModelMatrix;

	//The sky light luminance.
	Vector3<float32> _SkyLightLuminance;

	//Some padding.
	Padding<4> _Padding;

	//The material index.
	uint32 _MaterialIndex;

};

/*
*	Initializes this graphics pipeline.
*/
void SimplifiedModelGraphicsPipeline::Initialize(const DepthBufferHandle depth_buffer) NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("SimplifiedModelVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("SimplifiedModelFragmentShader")));

	//Set the depth buffer.
	SetDepthBuffer(depth_buffer);

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(1);
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetLightingSystem()->GetLightingDataRenderDataTableLayout());

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(SimplifiedModelPushConstantData));

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
	SetRenderResolution(RenderingSystem::Instance->GetFullResolution());

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
void SimplifiedModelGraphicsPipeline::Execute() NOEXCEPT
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
	command_buffer->BindRenderDataTable(this, 1, RenderingSystem::Instance->GetLightingSystem()->GetCurrentLightingDataRenderDataTable());

	//Cache the sky light luminance.
	Vector3<float32> sky_light_luminance{ VectorConstants::ZERO };

	{
		const uint64 number_of_light_components{ ComponentManager::GetNumberOfLightComponents() };
		const LightComponent* RESTRICT component{ ComponentManager::GetLightLightComponents() };

		for (uint64 i{ 0 }; i < number_of_light_components; ++i, ++component)
		{
			if (component->_LightType == LightType::DIRECTIONAL)
			{
				sky_light_luminance = component->_Color * component->_Intensity;

				break;
			}
		}
	}

	//Draw static models.
	{
		//Cache relevant data.
		const uint64 number_of_components{ ComponentManager::GetNumberOfStaticModelComponents() };
		const StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

		//Wait for static models culling to finish.
		CullingSystem::Instance->WaitForStaticModelsCulling();

		//Wait for static models level of detail to finish.
		LevelOfDetailSystem::Instance->WaitForStaticModelsLevelOfDetail();

		for (uint64 component_index{ 0 }; component_index < number_of_components; ++component_index, ++component)
		{
			//Skip this model depending on visibility.
			if (!component->_Visibility)
			{
				continue;
			}

			//Draw all meshes.
			for (uint64 mesh_index{ 0 }, size{ component->_ModelResource->_Meshes.Size() }; mesh_index < size; ++mesh_index)
			{
				//Skip this mesh if it's hidden.
				if (!TEST_BIT(component->_MeshesVisibleMask, BIT(mesh_index)))
				{
					continue;
				}

				//Cache the mesh.
				const Mesh& mesh{ component->_ModelResource->_Meshes[mesh_index] };

				//Push constants.
				SimplifiedModelPushConstantData data;

				data._ModelMatrix = component->_WorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
				data._SkyLightLuminance = sky_light_luminance;
				data._MaterialIndex = component->_MaterialResources[mesh_index]->_Index;

				command_buffer->PushConstants(this, ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(SimplifiedModelPushConstantData), &data);

				//Bind the vertex/inder buffer.
				command_buffer->BindVertexBuffer(this, 0, mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[mesh_index]]._VertexBuffer, &OFFSET);
				command_buffer->BindIndexBuffer(this, mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[mesh_index]]._IndexBuffer, OFFSET);

				//Draw!
				command_buffer->DrawIndexed(this, mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[mesh_index]]._IndexCount, 1);
			}
		}
	}

	//Draw dynamic models.
	{
		//Cache relevant data.
		const uint64 number_of_components{ ComponentManager::GetNumberOfDynamicModelComponents() };
		const DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

		//Wait for dynamic models culling to finish.
		CullingSystem::Instance->WaitForDynamicModelsCulling();

		//Wait for dynamic models level of detail to finish.
		LevelOfDetailSystem::Instance->WaitForDynamicModelsLevelOfDetail();

		for (uint64 i = 0; i < number_of_components; ++i, ++component)
		{
			//Skip this model depending on visibility.
			if (!component->_Visibility)
			{
				continue;
			}

			//Draw all meshes.
			for (uint64 mesh_index{ 0 }, size{ component->_ModelResource->_Meshes.Size() }; mesh_index < size; ++mesh_index)
			{
				//Skip this mesh depending on the material type.
				if (component->_MaterialResources[mesh_index]->_Type != MaterialResource::Type::OPAQUE)
				{
					continue;
				}

				//Cache the mesh.
				const Mesh& mesh{ component->_ModelResource->_Meshes[mesh_index] };

				//Push constants.
				SimplifiedModelPushConstantData data;

				data._ModelMatrix = component->_CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
				data._SkyLightLuminance = sky_light_luminance;
				data._MaterialIndex = component->_MaterialResources[mesh_index]->_Index;

				command_buffer->PushConstants(this, ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(SimplifiedModelPushConstantData), &data);

				//Bind the vertex/inder buffer.
				command_buffer->BindVertexBuffer(this, 0, mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[mesh_index]]._VertexBuffer, &OFFSET);
				command_buffer->BindIndexBuffer(this, mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[mesh_index]]._IndexBuffer, OFFSET);

				//Draw!
				command_buffer->DrawIndexed(this, mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[mesh_index]]._IndexCount, 1);
			}
		}
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}