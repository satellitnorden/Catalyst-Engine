#if defined(CATALYST_EDITOR)
//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/EditorSelectedModelGraphicsPipeline.h>

//Entities.
#include <Entities/Types/DynamicModelEntity.h>
#include <Entities/Types/StaticModelEntity.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Vertex.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Editor selected model vertex push constant data definition.
*/
class EditorSelectedModelVertexPushConstantData final
{

public:

	//The current world transform.
	Matrix4x4 _CurrentWorldTransform;

};

/*
*	Initializes this graphics pipeline.
*/
void EditorSelectedModelGraphicsPipeline::Initialize(const DepthBufferHandle depth_buffer) NOEXCEPT
{
	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("EditorSelectedModelVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("EditorSelectedModelFragmentShader")));

	//Set the depth buffer.
	SetDepthBuffer(depth_buffer);

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(1);
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::VERTEX, 0, sizeof(EditorSelectedModelVertexPushConstantData));

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
	SetBlendEnabled(true);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::None);
	SetDepthCompareOperator(CompareOperator::Equal);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(false);
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
void EditorSelectedModelGraphicsPipeline::Execute() NOEXCEPT
{
	//Define constants.
	constexpr uint64 OFFSET{ 0 };

	//Don't draw if the engine is in game.
	if (CatalystEditorSystem::Instance->IsInGame())
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

	//Retrieve the currently selected entity.
	const Entity *const RESTRICT currently_selected_entity{ CatalystEditorSystem::Instance->GetEditorSelectionSystem()->GetCurrentlySelectedEntity() };

	if (currently_selected_entity && currently_selected_entity->_Initialized)
	{
		switch (currently_selected_entity->_Type)
		{
			case EntityType::DynamicModel:
			{
				//Cache the dynamic model entity.
				const DynamicModelEntity *const RESTRICT dynamic_model_entity{ static_cast<const DynamicModelEntity *const RESTRICT>(currently_selected_entity) };

				//Draw all meshes.
				for (uint64 i{ 0 }, size{ dynamic_model_entity->GetModelResource()->_Meshes.Size() }; i < size; ++i)
				{
					//Cache the mesh.
					const Mesh& mesh{ dynamic_model_entity->GetModelResource()->_Meshes[i] };

					//Push constants.
					EditorSelectedModelVertexPushConstantData data;

					data._CurrentWorldTransform = dynamic_model_entity->GetWorldTransform()->ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());

					command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(EditorSelectedModelVertexPushConstantData), &data);

					//Bind the vertex/inder buffer.
					command_buffer->BindVertexBuffer(this, 0, mesh._MeshLevelOfDetails[dynamic_model_entity->GetLevelOfDetailindex(i)]._VertexBuffer, &OFFSET);
					command_buffer->BindIndexBuffer(this, mesh._MeshLevelOfDetails[dynamic_model_entity->GetLevelOfDetailindex(i)]._IndexBuffer, OFFSET);

					//Draw!
					command_buffer->DrawIndexed(this, mesh._MeshLevelOfDetails[dynamic_model_entity->GetLevelOfDetailindex(i)]._IndexCount, 1);
				}

				break;
			}

			case EntityType::StaticModel:
			{
				//Cache the dynamic model entity.
				const StaticModelEntity *const RESTRICT static_model_entity{ static_cast<const StaticModelEntity *const RESTRICT>(currently_selected_entity) };

				//Draw all meshes.
				for (uint64 i{ 0 }, size{ static_model_entity->GetModelResource()->_Meshes.Size() }; i < size; ++i)
				{
					//Cache the mesh.
					const Mesh& mesh{ static_model_entity->GetModelResource()->_Meshes[i] };

					//Push constants.
					EditorSelectedModelVertexPushConstantData data;

					data._CurrentWorldTransform = static_model_entity->GetWorldTransform()->ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());

					command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(EditorSelectedModelVertexPushConstantData), &data);

					//Bind the vertex/inder buffer.
					command_buffer->BindVertexBuffer(this, 0, mesh._MeshLevelOfDetails[static_model_entity->GetLevelOfDetailindex(i)]._VertexBuffer, &OFFSET);
					command_buffer->BindIndexBuffer(this, mesh._MeshLevelOfDetails[static_model_entity->GetLevelOfDetailindex(i)]._IndexBuffer, OFFSET);

					//Draw!
					command_buffer->DrawIndexed(this, mesh._MeshLevelOfDetails[static_model_entity->GetLevelOfDetailindex(i)]._IndexCount, 1);
				}

				break;
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
void EditorSelectedModelGraphicsPipeline::Terminate() NOEXCEPT
{

}
#endif