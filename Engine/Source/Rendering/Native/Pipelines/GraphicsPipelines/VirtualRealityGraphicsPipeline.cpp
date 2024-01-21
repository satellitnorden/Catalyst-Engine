//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/VirtualRealityGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>
#include <Components/Components/StaticModelComponent.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Virtual reality push constant data.
*/
class VirtualRealityPushConstantData final
{

public:

	//The model matrix.
	Matrix4x4 _ModelMatrix;

};

/*
*	Initializes this graphics pipeline.
*/
void VirtualRealityGraphicsPipeline::Initialize() NOEXCEPT
{
	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("VirtualRealityVertexShader"))->_Handle);
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("VirtualRealityFragmentShader"))->_Handle);

	//Set whether or not this graphics pipeline is rendering directly to the screen.
	SetIsRenderingDirectlyToScreen(true);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetVirtualRealitySystem()->GetVirtualRealityDataRenderDataTableLayout());

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(VirtualRealityPushConstantData));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetFullResolution());

	//Set the properties of the render pass.
	SetDepthStencilAttachmentLoadOperator(AttachmentLoadOperator::DONT_CARE);
	SetDepthStencilAttachmentStoreOperator(AttachmentStoreOperator::DONT_CARE);
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::CLEAR);
	SetColorAttachmentStoreOperator(AttachmentStoreOperator::STORE);
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::None);
	SetDepthCompareOperator(CompareOperator::Always);
	SetDepthTestEnabled(false);
	SetDepthWriteEnabled(false);
	SetStencilTestEnabled(false);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Keep);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(0);
	SetTopology(Topology::TriangleList);
	SetMultiviewEnabled(true);

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName("Virtual Reality");
#endif

	//Finalize the pipeline.
	FinalizePipeline();
}

/*
*	Executes this graphics pipeline.
*/
void VirtualRealityGraphicsPipeline::Execute() NOEXCEPT
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
	command_buffer->BindRenderDataTable(this, 1, RenderingSystem::Instance->GetVirtualRealitySystem()->GetCurrentVirtualRealityDataRenderDataTable());

	//Draw static models.
	{
		for (StaticModelInstanceData &instance_data : StaticModelComponent::Instance->InstanceData())
		{
			//Draw all meshes.
			for (uint64 mesh_index{ 0 }, size{ instance_data._ModelResource->_Meshes.Size() }; mesh_index < size; ++mesh_index)
			{
				//Skip this mesh if it's hidden.
				if (!TEST_BIT(instance_data._MeshesVisibleMask, BIT(mesh_index)))
				{
					continue;
				}

				//Cache the mesh.
				const Mesh &mesh{ instance_data._ModelResource->_Meshes[mesh_index] };

				//Push constants.
				VirtualRealityPushConstantData data;

				data._ModelMatrix = instance_data._CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());

				command_buffer->PushConstants(this, ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(VirtualRealityPushConstantData), &data);

				//Bind the vertex/inder buffer.
				command_buffer->BindVertexBuffer(this, 0, mesh._MeshLevelOfDetails[instance_data._LevelOfDetailIndices[mesh_index]]._VertexBuffer, &OFFSET);
				command_buffer->BindIndexBuffer(this, mesh._MeshLevelOfDetails[instance_data._LevelOfDetailIndices[mesh_index]]._IndexBuffer, OFFSET);

				//Draw!
				command_buffer->DrawIndexed(this, mesh._MeshLevelOfDetails[instance_data._LevelOfDetailIndices[mesh_index]]._IndexCount, 1);
			}
		}
	}

	//Draw dynamic models.
	{
		//Cache relevant data.
		const uint64 number_of_components{ ComponentManager::GetNumberOfDynamicModelComponents() };
		const DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

		for (uint64 i = 0; i < number_of_components; ++i, ++component)
		{
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
				VirtualRealityPushConstantData data;

				data._ModelMatrix = component->_CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());

				command_buffer->PushConstants(this, ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(VirtualRealityPushConstantData), &data);

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

/*
*	Terminates this ray tracing pipeline.
*/
void VirtualRealityGraphicsPipeline::Terminate() NOEXCEPT
{
	
}