#if !defined(CATALYST_FINAL)
//Header file.
#include <Rendering/Engine/RenderPasses/DebugScreenBoxRenderPass.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/DebugRenderingSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DebugScreenBoxRenderPass);

/*
*	Default constructor.
*/
DebugScreenBoxRenderPass::DebugScreenBoxRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		DebugScreenBoxRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the debug axis-aligned bounding box render pass.
*/
void DebugScreenBoxRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Debug);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::DebugScreenBox);

	//Set the shaders.
	SetVertexShader(Shader::DebugScreenBoxVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::DebugScreenBoxFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::None);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::Scene);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(Vector4<float>));
	AddPushConstantRange(ShaderStage::Fragment, sizeof(Vector4<float>), sizeof(Vector4<float>));

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

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		DebugScreenBoxRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the debug screen boxes.
*/
void DebugScreenBoxRenderPass::RenderInternal() NOEXCEPT
{
	//Iterate over all screen box debug render data and render it all.
	const DynamicArray<DebugRenderingSystem::ScreenBoxDebugRenderData> *const RESTRICT data{ DebugRenderingSystem::Instance->GetScreenBoxDebugRenderData() };

	//If there's none to render - render none.
	if (data->Empty())
	{
		//Don't include this render pass in the final render.
		SetIncludeInRender(false);

		return;
	}

	//Cache the command buffer.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the commandbuffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	for (const DebugRenderingSystem::ScreenBoxDebugRenderData &renderData : *data)
	{
		const Vector4<float> box{ renderData._Minimum._X , renderData._Minimum._Y , renderData._Maximum._X , renderData._Maximum._Y };

		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(Vector4<float>), &box);
		commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(Vector4<float>), sizeof(Vector4<float>), &renderData._Color);

		commandBuffer->Draw(this, 4, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}
#endif