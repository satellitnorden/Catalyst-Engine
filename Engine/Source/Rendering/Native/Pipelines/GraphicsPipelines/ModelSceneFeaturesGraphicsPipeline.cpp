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

	Matrix4 _ModelMatrix;

};

/*
*	Fragment push constant data definition.
*/
class FragmentPushConstantData final
{

public:

	int32 _AlbedoTextureIndex;
	int32 _NormalMapTextureIndex;
	int32 _MaterialPropertiesTextureIndex;
	int32 _MaterialProperties;
	float _LuminanceMultiplier;

};

/*
*	Initializes this graphics pipeline.
*/
void ModelSceneFeaturesGraphicsPipeline::Initialize() NOEXCEPT
{
	//Create the scene depth buffer.
	RenderingSystem::Instance->CreateDepthBuffer(RenderingSystem::Instance->GetScaledResolution(), &_SceneDepthBuffer);

	//Set the shaders.
	SetVertexShader(Shader::ModelSceneFeaturesVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::ModelSceneFeaturesFragment);

	//Set the depth buffer.
	SetDepthBuffer(_SceneDepthBuffer);

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
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::Back);
	SetDepthCompareOperator(CompareOperator::Greater);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
	SetStencilTestEnabled(false);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Keep);
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
void ModelSceneFeaturesGraphicsPipeline::Execute() NOEXCEPT
{
	//Define constants.
	constexpr uint64 OFFSET{ 0 };

	//Iterate over all model components and draw them all.
	const uint64 numberOfStaticModelComponents{ ComponentManager::GetNumberOfStaticModelComponents() };
	const uint64 numberOfDynamicModelComponents{ ComponentManager::GetNumberOfDynamicModelComponents() };

	//If there's none to render - render none.
	if (numberOfStaticModelComponents == 0 && numberOfDynamicModelComponents == 0)
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

	//Draw all static models.
	{
		const StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

		for (uint64 i = 0; i < numberOfStaticModelComponents; ++i, ++component)
		{
			//Push constants.
			VertexPushConstantData vertexData;

			vertexData._ModelMatrix = component->_WorldTransform;

			commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(VertexPushConstantData), &vertexData);

			FragmentPushConstantData fragmentData;

			fragmentData._AlbedoTextureIndex = component->_Material._FirstTextureIndex;
			fragmentData._NormalMapTextureIndex = component->_Material._SecondTextureIndex;
			fragmentData._MaterialPropertiesTextureIndex = component->_Material._ThirdTextureIndex;
			fragmentData._MaterialProperties = static_cast<int32>(component->_Material._Properties);
			fragmentData._LuminanceMultiplier = component->_Material._LuminanceMultiplier;

			commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData), &fragmentData);

			//Bind the vertex/inder buffer.
			commandBuffer->BindVertexBuffer(this, 0, component->_Model->_VertexBuffer, &OFFSET);
			commandBuffer->BindIndexBuffer(this, component->_Model->_IndexBuffer, OFFSET);

			commandBuffer->DrawIndexed(this, component->_Model->_IndexCount, 1);
		}
	}

	//Draw all dynamic models.
	{
		const DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

		for (uint64 i = 0; i < numberOfDynamicModelComponents; ++i, ++component)
		{
			//Push constants.
			VertexPushConstantData vertexData;

			vertexData._ModelMatrix = component->_WorldTransform;

			commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(VertexPushConstantData), &vertexData);

			FragmentPushConstantData fragmentData;

			fragmentData._AlbedoTextureIndex = component->_Material._FirstTextureIndex;
			fragmentData._NormalMapTextureIndex = component->_Material._SecondTextureIndex;
			fragmentData._MaterialPropertiesTextureIndex = component->_Material._ThirdTextureIndex;
			fragmentData._MaterialProperties = static_cast<int32>(component->_Material._Properties);
			fragmentData._LuminanceMultiplier = component->_Material._LuminanceMultiplier;

			commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData), &fragmentData);

			//Bind the vertex/inder buffer.
			commandBuffer->BindVertexBuffer(this, 0, component->_Model->_VertexBuffer, &OFFSET);
			commandBuffer->BindIndexBuffer(this, component->_Model->_IndexBuffer, OFFSET);

			commandBuffer->DrawIndexed(this, component->_Model->_IndexCount, 1);
		}
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}