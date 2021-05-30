//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/OpaqueModelSceneFeaturesGraphicsPipeline.h>

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
*	Vertex push constant data definition.
*/
class VertexPushConstantData final
{

public:

	Matrix4x4 _PreviousModelMatrix;
	Matrix4x4 _CurrentModelMatrix;

};

/*
*	Fragment push constant data definition.
*/
class FragmentPushConstantData final
{

public:

	uint32 _MaterialIndex;

};

/*
*	Initializes this graphics pipeline.
*/
void OpaqueModelSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depth_buffer, const bool double_sided) NOEXCEPT
{
	//Remember whether or not to render opaque models double-sided.
	_DoubleSided = double_sided;

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("OpaqueModelSceneFeaturesVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("OpaqueModelSceneFeaturesFragmentShader")));

	//Set the depth buffer.
	SetDepthBuffer(depth_buffer);

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(5);
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_1));
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_2));
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_3));
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_4));
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::VERTEX, 0, sizeof(VertexPushConstantData));
	AddPushConstantRange(ShaderStage::FRAGMENT, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData));

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

	if (_DoubleSided)
	{
		SetCullMode(CullMode::None);
	}
	
	else
	{
		SetCullMode(CullMode::Back);
	}

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
void OpaqueModelSceneFeaturesGraphicsPipeline::Execute() NOEXCEPT
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

				//Skip this mesh depending on the material type.
				if (component->_MaterialResources[mesh_index]->_Type != MaterialResource::Type::OPAQUE)
				{
					continue;
				}

				//Skip this mesh depending on the double-sidedness.
				if (_DoubleSided != component->_MaterialResources[mesh_index]->_DoubleSided)
				{
					continue;
				}

				//Cache the mesh.
				const Mesh& mesh{ component->_ModelResource->_Meshes[mesh_index] };

				//Push constants.
				VertexPushConstantData vertexData;

				vertexData._PreviousModelMatrix = vertexData._CurrentModelMatrix = component->_WorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());

				command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(VertexPushConstantData), &vertexData);

				FragmentPushConstantData fragmentData;

				fragmentData._MaterialIndex = component->_MaterialResources[mesh_index]->_Index;

				command_buffer->PushConstants(this, ShaderStage::FRAGMENT, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData), &fragmentData);

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
			for (uint64 i{ 0 }, size{ component->_ModelResource->_Meshes.Size() }; i < size; ++i)
			{
				//Skip this mesh depending on the material type.
				if (component->_MaterialResources[i]->_Type != MaterialResource::Type::OPAQUE)
				{
					continue;
				}

				//Skip this mesh depending on the double-sidedness.
				if (_DoubleSided != component->_MaterialResources[i]->_DoubleSided)
				{
					continue;
				}

				//Cache the mesh.
				const Mesh& mesh{ component->_ModelResource->_Meshes[i] };

				//Push constants.
				VertexPushConstantData vertexData;

				vertexData._PreviousModelMatrix = component->_PreviousWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
				vertexData._CurrentModelMatrix = component->_CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());

				command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(VertexPushConstantData), &vertexData);

				FragmentPushConstantData fragmentData;

				fragmentData._MaterialIndex = component->_MaterialResources[i]->_Index;

				command_buffer->PushConstants(this, ShaderStage::FRAGMENT, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData), &fragmentData);

				//Bind the vertex/inder buffer.
				command_buffer->BindVertexBuffer(this, 0, mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[i]]._VertexBuffer, &OFFSET);
				command_buffer->BindIndexBuffer(this, mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[i]]._IndexBuffer, OFFSET);

				//Draw!
				command_buffer->DrawIndexed(this, mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[i]]._IndexCount, 1);
			}
		}
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}