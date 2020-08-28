//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/InstancedOpaqueModelSceneFeaturesGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Vertex.h>

//Systems.
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

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
void InstancedOpaqueModelSceneFeaturesGraphicsPipeline::Initialize(const bool double_sided, const DepthBufferHandle depth_buffer) NOEXCEPT
{
	//Set double sided.
	_DoubleSided = double_sided;

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("InstancedOpaqueModelSceneFeaturesVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("InstancedOpaqueModelSceneFeaturesFragmentShader")));

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
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::FRAGMENT, 0, sizeof(FragmentPushConstantData));

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
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution(0));

	//Set the properties of the render pass.
	SetShouldClear(false);
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
void InstancedOpaqueModelSceneFeaturesGraphicsPipeline::Execute() NOEXCEPT
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
			//Bind the transformations buffer.
			command_buffer->BindVertexBuffer(this, 1, component->_TransformationsBuffer, &OFFSET);

			//Draw all meshes.
			for (uint64 i{ 0 }, size{ component->_ModelResource->_Meshes.Size() }; i < size; ++i)
			{
				//Cache the mesh.
				const Mesh& mesh{ component->_ModelResource->_Meshes[i] };

				//Skip this mesh depending on the double-sidedness.
				if (_DoubleSided != component->_MaterialResources[i]->_DoubleSided)
				{
					continue;
				}

				//Push constants.
				FragmentPushConstantData data;

				data._MaterialIndex = component->_MaterialResources[i]->_Index;

				command_buffer->PushConstants(this, ShaderStage::FRAGMENT, 0, sizeof(FragmentPushConstantData), &data);

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