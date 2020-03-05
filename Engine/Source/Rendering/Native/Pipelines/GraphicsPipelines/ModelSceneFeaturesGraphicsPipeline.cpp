//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ModelSceneFeaturesGraphicsPipeline.h>

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
	SetVertexShader(Shader::ModelSceneFeaturesVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::ModelSceneFeaturesFragment);

	//Set the depth buffer.
	SetDepthBuffer(depthBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(4);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures1));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures2));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures3));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures4));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(VertexPushConstantData));
	AddPushConstantRange(ShaderStage::Fragment, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData));

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

	//Iterate over all model components and draw them all.
	const uint64 numberOfModelComponents{ ComponentManager::GetNumberOfModelComponents() };

	//If there's none to render - render none.
	if (numberOfModelComponents == 0 && false)
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

	for (uint64 i = 0; i < numberOfModelComponents; ++i, ++component)
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

			commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(VertexPushConstantData), &vertexData);

			FragmentPushConstantData fragmentData;

			fragmentData._MaterialIndex = component->_MaterialIndices[i];

			commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData), &fragmentData);

			//Bind the vertex/inder buffer.
			commandBuffer->BindVertexBuffer(this, 0, mesh._VertexBuffers[0], &OFFSET);
			commandBuffer->BindIndexBuffer(this, mesh._IndexBuffers[0], OFFSET);

			commandBuffer->DrawIndexed(this, mesh._IndexCounts[0], 1);
		}
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}