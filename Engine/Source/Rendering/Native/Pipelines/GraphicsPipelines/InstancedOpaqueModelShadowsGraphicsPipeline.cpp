//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/InstancedOpaqueModelShadowsGraphicsPipeline.h>

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
#include <Systems/WorldSystem.h>

/*
*	Instanced opaque model shadows vertex push constant data definition.
*/
class InstancedOpaqueModelShadowsVertexPushConstantData final
{

public:

	//The world to light matrix.
	Matrix4x4 _WorldToLightMatrix;

	//The world grid delta.
	Vector3<float32> _WorldGridDelta;

	//Some padding.
	Padding<4> _Padding;

	//The model flags.
	uint32 _ModelFlags;

};

/*
*	Initializes this graphics pipeline.
*/
void InstancedOpaqueModelShadowsGraphicsPipeline::Initialize(const bool double_sided, const DepthBufferHandle depth_buffer, const RenderTargetHandle render_target) NOEXCEPT
{
	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Set double sided.
	_DoubleSided = double_sided;

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("InstancedOpaqueModelShadowsVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("InstancedOpaqueModelShadowsFragmentShader")));

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
	AddPushConstantRange(ShaderStage::VERTEX, 0, sizeof(InstancedOpaqueModelShadowsVertexPushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(5);
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
	AddVertexInputAttributeDescription(	4,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(Vector4<float32>) * 0);
	AddVertexInputAttributeDescription(	5,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(Vector4<float32>) * 1);
	AddVertexInputAttributeDescription(	6,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(Vector4<float32>) * 2);
	AddVertexInputAttributeDescription(	7,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(Vector4<float32>) * 3);

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(2);
	AddVertexInputBindingDescription(0, sizeof(Vertex), VertexInputBindingDescription::InputRate::Vertex);
	AddVertexInputBindingDescription(1, sizeof(Matrix4x4), VertexInputBindingDescription::InputRate::Instance);

	//Set the render resolution.
	SetRenderResolution(Resolution(CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution, CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution));

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
	SetName("Instanced Opaque Model Shadows");
#endif
}

/*
*	Executes this graphics pipeline.
*/
void InstancedOpaqueModelShadowsGraphicsPipeline::Execute(const uint8 cascade_index, const Matrix4x4 &world_to_light_matrix) NOEXCEPT
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

	//Draw instanced static models.
	{
		//Cache relevant data.
		const uint64 number_of_components{ ComponentManager::GetNumberOfInstancedStaticModelComponents() };
		const InstancedStaticModelComponent *RESTRICT component{ ComponentManager::GetInstancedStaticModelInstancedStaticModelComponents() };

		for (uint64 i = 0; i < number_of_components; ++i, ++component)
		{
			switch (cascade_index)
			{
				case 0:
				{
					if (!TEST_BIT(component->_ModelFlags, ModelFlags::INCLUDE_IN_SHADOW_CASCADE_1))
					{
						continue;
					}

					break;
				}

				case 1:
				{
					if (!TEST_BIT(component->_ModelFlags, ModelFlags::INCLUDE_IN_SHADOW_CASCADE_2))
					{
						continue;
					}

					break;
				}

				case 2:
				{
					if (!TEST_BIT(component->_ModelFlags, ModelFlags::INCLUDE_IN_SHADOW_CASCADE_3))
					{
						continue;
					}

					break;
				}

				case 3:
				{
					if (!TEST_BIT(component->_ModelFlags, ModelFlags::INCLUDE_IN_SHADOW_CASCADE_4))
					{
						continue;
					}

					break;
				}
			}

			//Bind the transformations buffer.
			command_buffer->BindVertexBuffer(this, 1, component->_TransformationsBuffer, &OFFSET);

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
				{
					InstancedOpaqueModelShadowsVertexPushConstantData data;

					data._WorldToLightMatrix = world_to_light_matrix;

					const Vector3<int32> delta{ component->_Cell - WorldSystem::Instance->GetCurrentWorldGridCell() };

					for (uint8 i{ 0 }; i < 3; ++i)
					{
						data._WorldGridDelta[i] = static_cast<float32>(delta[i]) * WorldSystem::Instance->GetWorldGridSize();
					}

					data._ModelFlags = static_cast<uint32>(component->_ModelFlags);

					command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(InstancedOpaqueModelShadowsVertexPushConstantData), &data);
				}

				//Bind the vertex/inder buffer.
				command_buffer->BindVertexBuffer(this, 0, mesh._MeshLevelOfDetails[0]._VertexBuffer, &OFFSET);
				command_buffer->BindIndexBuffer(this, mesh._MeshLevelOfDetails[0]._IndexBuffer, OFFSET);

				//Draw!
				command_buffer->DrawIndexed(this, mesh._MeshLevelOfDetails[0]._IndexCount, component->_NumberOfTransformations);
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
void InstancedOpaqueModelShadowsGraphicsPipeline::Terminate() NOEXCEPT
{

}