//Header file.
#include <Rendering/Engine/RenderPasses/ScreenSpaceAmbientOcclusionRenderPass.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ScreenSpaceAmbientOcclusionRenderPass);

namespace ScreenSpaceAmbientOcclusionRenderPassInternal
{
	constexpr uint8 SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES{ 8 };

	/*
	*	Push constant data.
	*/
	class PushConstantData final
	{

	public:

		StaticArray<Vector4<float>, SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES> _Offsets;

	};

	PushConstantData _PushConstantData;
}

/*
*	Default constructor.
*/
ScreenSpaceAmbientOcclusionRenderPass::ScreenSpaceAmbientOcclusionRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		ScreenSpaceAmbientOcclusionRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the screen space ambient occlusion render pass.
*/
void ScreenSpaceAmbientOcclusionRenderPass::InitializeInternal() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Initialize the push constant data.
	InitializePushConstantData();

	//Set the main stage.
	SetMainStage(RenderPassMainStage::ScreenSpaceAmbientOcclusionCalculation);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::ScreenSpaceAmbientOcclusion);

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::ScreenSpaceAmbientOcclusionFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::None);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::ScreenSpaceAmbientOcclusion);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Fragment, 0, sizeof(ScreenSpaceAmbientOcclusionRenderPassInternal::PushConstantData));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::Back);
	SetDepthCompareOperator(CompareOperator::Always);
	SetDepthTestEnabled(false);
	SetDepthWriteEnabled(false);
	SetStencilTestEnabled(true);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Keep);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Equal);
	SetStencilCompareMask(BIT(0));
	SetStencilWriteMask(0);
	SetStencilReferenceMask(BIT(0));
	SetTopology(Topology::TriangleFan);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		ScreenSpaceAmbientOcclusionRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Creates the render data table layout.
*/
void ScreenSpaceAmbientOcclusionRenderPass::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 1> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void ScreenSpaceAmbientOcclusionRenderPass::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, _RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferNormalDepth), RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
}

/*
*	Initializes the push constant data.
*/
void ScreenSpaceAmbientOcclusionRenderPass::InitializePushConstantData() NOEXCEPT
{
	for (uint8 i{ 0 }; i < ScreenSpaceAmbientOcclusionRenderPassInternal::SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES; ++i)
	{
		Vector3<float> normal{ CatalystBaseMath::RandomFloatInRange(-1.0f, 1.0f) * 0.2f,
								CatalystBaseMath::RandomFloatInRange(-1.0f, 1.0f)* 0.2f,
								CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };
		normal.Normalize();

		const float length{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };

		ScreenSpaceAmbientOcclusionRenderPassInternal::_PushConstantData._Offsets[i] = Vector4<float>(normal, length);
	}
}

/*
*	Renders the screen space ambient occlusion.
*/
void ScreenSpaceAmbientOcclusionRenderPass::RenderInternal() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, _RenderDataTable);

	//Push constants.
	commandBuffer->PushConstants(this, ShaderStage::Fragment, 0, sizeof(ScreenSpaceAmbientOcclusionRenderPassInternal::PushConstantData), &ScreenSpaceAmbientOcclusionRenderPassInternal::_PushConstantData);

	//Draw!
	commandBuffer->Draw(this, 3, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}