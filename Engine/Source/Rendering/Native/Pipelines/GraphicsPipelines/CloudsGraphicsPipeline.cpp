//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/CloudsGraphicsPipeline.h>

//Core.
#include <Core/General/Perceiver.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Texture3D.h>

//Resources.
#include <Resources/Loading/ResourceLoader.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Push constant data definition.
*/
class PushConstantData final
{

public:

	//The sky light view direction.
	Vector3<float> _SkyLightViewDirection;

	//Padding.
	Padding<4> _Padding1;

	//The sky light luminance.
	Vector3<float> _SkyLightLuminance;

	//Padding.
	Padding<4> _Padding2;

	//The cloud density.
	float _CloudDensity;

};

/*
*	Initializes this graphics pipeline.
*/
void CloudsGraphicsPipeline::Initialize() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::CloudsFragment);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Fragment, 0, sizeof(PushConstantData));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution() / 2);

	//Set the properties of the render pass.
	SetShouldClear(false);
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::Back);
	SetDepthCompareOperator(CompareOperator::Always);
	SetDepthTestEnabled(false);
	SetDepthWriteEnabled(false);
	SetStencilTestEnabled(false);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Keep);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::NotEqual);
	SetStencilCompareMask(0);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(0);
	SetTopology(Topology::TriangleFan);
}

/*
*	Executes this graphics pipeline.
*/
void CloudsGraphicsPipeline::Execute() NOEXCEPT
{
	//Define constants.
	constexpr float MINIMUM_CLOUD_DENSITY{ 0.3f };
	constexpr float MAXIMUM_CLOUD_DENSITY{ 0.7f };

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, _RenderDataTable);

	//Push constants.
	PushConstantData data;

	//Assume that the first light is the sky light, for now. (:
	if (ComponentManager::GetNumberOfLightComponents() > 0)
	{
		data._SkyLightViewDirection = Vector3<float>::Normalize(Perceiver::Instance->GetPosition() - ComponentManager::GetLightLightComponents()[0]._Position);
		data._SkyLightLuminance = ComponentManager::GetLightLightComponents()[0]._Color * ComponentManager::GetLightLightComponents()[0]._Strength * 0.0000000025f;
	}

	else
	{
		data._SkyLightViewDirection = VectorConstants::UP;
		data._SkyLightLuminance = VectorConstants::ONE;
	}

	data._CloudDensity = CatalystBaseMath::Scale(EnvironmentManager::GetCloudDensity(), 0.0f, 1.0f, MINIMUM_CLOUD_DENSITY, MAXIMUM_CLOUD_DENSITY);
	//data._CloudDensity = 0.5f;

	commandBuffer->PushConstants(this, ShaderStage::Fragment, 0, sizeof(PushConstantData), &data);

	//Draw!
	commandBuffer->Draw(this, 3, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}

/*
*	Creates the render data table layout.
*/
void CloudsGraphicsPipeline::CreateRenderDataTableLayout() NOEXCEPT
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
void CloudsGraphicsPipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, &_RenderDataTable, ResourceLoader::GetTexture3D(HashString("Cloud_Texture3D")), RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeRepeat));
}