//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/MaskedModelDepthSceneFeaturesGraphicsPipeline.h>

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
*	Masked model depth vertex push constant data definition.
*/
class MaskedModelDepthVertexPushConstantData final
{

public:

	//The current world transform
	Matrix4x4 _CurrentWorldTransform;

};

/*
*	Masked model depth fragment push constant data definition.
*/
class MaskedModelDepthFragmentPushConstantData final
{

public:

	//The material index.
	uint32 _MaterialIndex;

};

/*
*	Initializes this graphics pipeline.
*/
void MaskedModelDepthSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depth_buffer, const bool double_sided) NOEXCEPT
{
	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Remember whether or not to render opaque models double-sided.
	_DoubleSided = double_sided;

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("MaskedModelDepthSceneFeaturesVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("MaskedModelDepthSceneFeaturesFragmentShader")));

	//Set the depth buffer.
	SetDepthBuffer(depth_buffer);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::VERTEX, 0, sizeof(MaskedModelDepthVertexPushConstantData));
	AddPushConstantRange(ShaderStage::FRAGMENT, sizeof(MaskedModelDepthVertexPushConstantData), sizeof(MaskedModelDepthFragmentPushConstantData));

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
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::DONT_CARE);
	SetColorAttachmentStoreOperator(AttachmentStoreOperator::DONT_CARE);
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
void MaskedModelDepthSceneFeaturesGraphicsPipeline::Execute() NOEXCEPT
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
				if (component->_MaterialResources[i]->_Type != MaterialResource::Type::MASKED)
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
				MaskedModelDepthVertexPushConstantData vertexData;

				vertexData._CurrentWorldTransform = component->_WorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());

				command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(MaskedModelDepthVertexPushConstantData), &vertexData);

				MaskedModelDepthFragmentPushConstantData fragmentData;

				fragmentData._MaterialIndex = component->_MaterialResources[i]->_Index;

				command_buffer->PushConstants(this, ShaderStage::FRAGMENT, sizeof(MaskedModelDepthVertexPushConstantData), sizeof(MaskedModelDepthFragmentPushConstantData), &fragmentData);

				//Bind the vertex/inder buffer.
				command_buffer->BindVertexBuffer(this, 0, mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[i]]._VertexBuffer, &OFFSET);
				command_buffer->BindIndexBuffer(this, mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[i]]._IndexBuffer, OFFSET);

				//Draw!
				command_buffer->DrawIndexed(this, mesh._MeshLevelOfDetails[component->_LevelOfDetailIndices[i]]._IndexCount, 1);
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
				if (component->_MaterialResources[i]->_Type != MaterialResource::Type::MASKED)
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
				MaskedModelDepthVertexPushConstantData vertexData;

				vertexData._CurrentWorldTransform = component->_CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());

				command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(MaskedModelDepthVertexPushConstantData), &vertexData);

				MaskedModelDepthFragmentPushConstantData fragmentData;

				fragmentData._MaterialIndex = component->_MaterialResources[i]->_Index;

				command_buffer->PushConstants(this, ShaderStage::FRAGMENT, sizeof(MaskedModelDepthVertexPushConstantData), sizeof(MaskedModelDepthFragmentPushConstantData), &fragmentData);

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

/*
*	Terminates this graphics pipeline.
*/
void MaskedModelDepthSceneFeaturesGraphicsPipeline::Terminate() NOEXCEPT
{

}