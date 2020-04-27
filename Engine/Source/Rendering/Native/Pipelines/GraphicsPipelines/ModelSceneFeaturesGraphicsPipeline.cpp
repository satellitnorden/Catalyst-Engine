//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ModelSceneFeaturesGraphicsPipeline.h>

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

	int32 _MaterialIndex;

};

/*
*	Initializes this graphics pipeline.
*/
void ModelSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depthBuffer) NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("ModelSceneFeaturesVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("ModelSceneFeaturesFragmentShader")));

	//Set the depth buffer.
	SetDepthBuffer(depthBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(4);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_1));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_2));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_3));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_4));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

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
	SetShouldClear(true);
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
void ModelSceneFeaturesGraphicsPipeline::Execute() NOEXCEPT
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
			for (uint64 i{ 0 }, size{ component->_Model->_Meshes.Size() }; i < size; ++i)
			{
				//Cache the mesh.
				const Mesh& mesh{ component->_Model->_Meshes[i] };

				//Push constants.
				VertexPushConstantData vertexData;

				vertexData._PreviousModelMatrix = component->_WorldTransform;
				vertexData._CurrentModelMatrix = component->_WorldTransform;

				command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(VertexPushConstantData), &vertexData);

				FragmentPushConstantData fragmentData;

				fragmentData._MaterialIndex = component->_MaterialIndices[i];

				command_buffer->PushConstants(this, ShaderStage::FRAGMENT, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData), &fragmentData);

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
			for (uint64 i{ 0 }, size{ component->_Model->_Meshes.Size() }; i < size; ++i)
			{
				//Cache the mesh.
				const Mesh& mesh{ component->_Model->_Meshes[i] };

				//Push constants.
				VertexPushConstantData vertexData;

				vertexData._PreviousModelMatrix = component->_PreviousWorldTransform;
				vertexData._CurrentModelMatrix = component->_CurrentWorldTransform;

				command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(VertexPushConstantData), &vertexData);

				FragmentPushConstantData fragmentData;

				fragmentData._MaterialIndex = component->_MaterialIndexCollection.GetMaterialIndexAt(static_cast<uint8>(i), static_cast<uint8>(component->_Model->_Meshes.Size()));

				command_buffer->PushConstants(this, ShaderStage::FRAGMENT, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData), &fragmentData);

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