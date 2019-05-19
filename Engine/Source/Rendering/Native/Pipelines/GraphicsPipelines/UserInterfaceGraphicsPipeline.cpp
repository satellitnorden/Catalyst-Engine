//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/UserInterfaceGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Vertex push constant data definition.
*/
class VertexPushConstantData final
{

public:

	//The minimum of the screen space axis aligned box.
	Vector2<float> _Minimum;

	//The maximum of the screen space axis aligned box.
	Vector2<float> _Maximum;

};

/*
*	Fragment push constant data definition.
*/
class FragmentPushConstantData final
{

public:

	//The texture index.
	int32 _TextureIndex;

};

/*
*	Initializes this graphics pipeline.
*/
void UserInterfaceGraphicsPipeline::Initialize() NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(Shader::UserInterfaceVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::UserInterfaceFragment);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(VertexPushConstantData));
	AddPushConstantRange(ShaderStage::Fragment, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(true);
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
	SetTopology(Topology::TriangleFan);
}

/*
*	Executes this graphics pipeline.
*/
void UserInterfaceGraphicsPipeline::Execute() NOEXCEPT
{
	//Get the number of user interface elements.
	const uint64 numberOfUserInterfaceElements{ ComponentManager::ReadSingletonComponent<UserInterfaceComponent>()->_UserInterfaceElements.Size() };

	//If there's no to render, then... Don't.
	if (numberOfUserInterfaceElements == 0)
	{
		SetIncludeInRender(false);

		return;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data table.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Render all user interface elements.
	for (const UserInterfaceElement *const RESTRICT element : ComponentManager::ReadSingletonComponent<UserInterfaceComponent>()->_UserInterfaceElements)
	{
		//Push constants.
		VertexPushConstantData vertexData;

		vertexData._Minimum = element->_Minimum;
		vertexData._Maximum = element->_Maximum;

		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(VertexPushConstantData), &vertexData);

		FragmentPushConstantData fragmentData;

		fragmentData._TextureIndex = element->_ActiveTextureIndex;

		commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData), &fragmentData);

		//Draw!
		commandBuffer->Draw(this, 4, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}