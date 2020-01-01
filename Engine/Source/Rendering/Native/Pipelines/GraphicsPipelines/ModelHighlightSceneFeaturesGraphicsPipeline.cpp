//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ModelHighlightSceneFeaturesGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Vertex.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Vertex push constant data definition.
*/
class ModelHighlightSceneFeaturesVertexPushConstantData final
{

public:

	Matrix4 _CurrentModelMatrix;

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
	SetVertexShader(Shader::ModelHighlightSceneFeaturesVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::ModelHighlightSceneFeaturesFragment);

	//Set the depth buffer.
	SetDepthBuffer(depthBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(2);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures1));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures3));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(ModelHighlightSceneFeaturesVertexPushConstantData));
	AddPushConstantRange(ShaderStage::Fragment, sizeof(ModelHighlightSceneFeaturesVertexPushConstantData), sizeof(ModelHighlightSceneFeaturesFragmentPushConstantData));

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
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

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

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Draw all models.
	const ModelComponent *RESTRICT component{ ComponentManager::GetModelModelComponents() };

	for (const HighlightedModel &highlighted_model : *highlighted_models)
	{
		//Retrieve the component.
		const ModelComponent& component{ ComponentManager::GetModelModelComponents()[highlighted_model._ComponentsIndex] };

		//Push constants.
		ModelHighlightSceneFeaturesVertexPushConstantData vertex_data;

		vertex_data._CurrentModelMatrix = component._CurrentWorldTransform;

		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(ModelHighlightSceneFeaturesVertexPushConstantData), &vertex_data);

		ModelHighlightSceneFeaturesFragmentPushConstantData fragment_data;

		fragment_data._HighlightColor = highlighted_model._HighlightColor;
		fragment_data._HighlightStrength = highlighted_model._HighlightStrength;
		fragment_data._MaterialIndex = component._MaterialIndex;

		commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(ModelHighlightSceneFeaturesVertexPushConstantData), sizeof(ModelHighlightSceneFeaturesFragmentPushConstantData), &fragment_data);

		//Bind the vertex/inder buffer.
		commandBuffer->BindVertexBuffer(this, 0, component._Model->_VertexBuffer, &OFFSET);
		commandBuffer->BindIndexBuffer(this, component._Model->_IndexBuffer, OFFSET);

		commandBuffer->DrawIndexed(this, component._Model->_IndexCount, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}