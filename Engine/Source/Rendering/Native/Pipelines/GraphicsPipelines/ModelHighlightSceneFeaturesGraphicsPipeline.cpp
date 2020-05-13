//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ModelHighlightSceneFeaturesGraphicsPipeline.h>

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
class ModelHighlightSceneFeaturesVertexPushConstantData final
{

public:

	Matrix4x4 _CurrentModelMatrix;

};

/*
*	Fragment push constant data definition.
*/
class ModelHighlightSceneFeaturesFragmentPushConstantData final
{

public:

	Vector3<float> _HighlightColor;
	Padding<1> _Padding;
	float _HighlightStrength;
	int32 _MaterialIndex;

};

/*
*	Initializes this graphics pipeline.
*/
void ModelHighlightSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depthBuffer) NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("ModelHighlightSceneFeaturesVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("ModelHighlightSceneFeaturesFragmentShader")));

	//Set the depth buffer.
	SetDepthBuffer(depthBuffer);

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(2);
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_1));
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_3));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::VERTEX, 0, sizeof(ModelHighlightSceneFeaturesVertexPushConstantData));
	AddPushConstantRange(ShaderStage::FRAGMENT, sizeof(ModelHighlightSceneFeaturesVertexPushConstantData), sizeof(ModelHighlightSceneFeaturesFragmentPushConstantData));

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
	SetShouldClear(false);
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::Back);
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
void ModelHighlightSceneFeaturesGraphicsPipeline::Execute() NOEXCEPT
{
	//Define constants.
	constexpr uint64 OFFSET{ 0 };

	//Iterate over all highlighted models and draw them all.
	const DynamicArray<HighlightedModel>* const RESTRICT highlighted_models{ RenderingSystem::Instance->GetModelSystem()->GetHighlightedModels() };

	//If there's none to render - render none.
	if (highlighted_models->Empty())
	{
		//Don't include this render pass in the final render.
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

	//Wait for level of detail to finish.
	LevelOfDetailSystem::Instance->WaitForDynamicModelsLevelOfDetail();

	//Draw all models.
	for (const HighlightedModel &highlighted_model : *highlighted_models)
	{
		//Retrieve the component.
		const DynamicModelComponent& component{ ComponentManager::GetDynamicModelDynamicModelComponents()[highlighted_model._ComponentsIndex] };

		//Draw all meshes.
		for (uint64 i{ 0 }, size{ component._ModelResource->_Meshes.Size() }; i < size; ++i)
		{
			//Cache the mesh.
			const Mesh &mesh{ component._ModelResource->_Meshes[i] };

			//Push constants.
			ModelHighlightSceneFeaturesVertexPushConstantData vertex_data;

			vertex_data._CurrentModelMatrix = component._CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());

			command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(ModelHighlightSceneFeaturesVertexPushConstantData), &vertex_data);

			ModelHighlightSceneFeaturesFragmentPushConstantData fragment_data;

			fragment_data._HighlightColor = highlighted_model._HighlightColor;
			fragment_data._HighlightStrength = highlighted_model._HighlightStrength;
			fragment_data._MaterialIndex = component._MaterialIndexCollection.GetMaterialIndexAt(static_cast<uint8>(i), static_cast<uint8>(component._ModelResource->_Meshes.Size()));

			command_buffer->PushConstants(this, ShaderStage::FRAGMENT, sizeof(ModelHighlightSceneFeaturesVertexPushConstantData), sizeof(ModelHighlightSceneFeaturesFragmentPushConstantData), &fragment_data);

			//Bind the vertex/inder buffer.
			command_buffer->BindVertexBuffer(this, 0, mesh._VertexBuffers[component._LevelOfDetailIndices[i]], &OFFSET);
			command_buffer->BindIndexBuffer(this, mesh._IndexBuffers[component._LevelOfDetailIndices[i]], OFFSET);

			//Draw!
			command_buffer->DrawIndexed(this, mesh._IndexCounts[component._LevelOfDetailIndices[i]], 1);
		}
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}