//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/VolumetricLightingGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

/*
*	Volumetric lighting application push constant data definition.
*/
class VolumetricLightingPushConstantData final
{

public:

	//The sky light luminance.
	Vector3<float32> _SkyLightLuminance;

	//Padding.
	Padding<4> _Padding;

	//The sky light screen space position.
	Vector2<float32> _SkyLightScreenSpacePosition;

};

/*
*	Initializes this graphics pipeline.
*/
void VolumetricLightingGraphicsPipeline::Initialize() NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("ViewportVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("VolumetricLightingFragmentShader")));

	//Add the orender targets.
	SetNumberOfOutputRenderTargets(1);
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(3);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetModelSystem()->GetModelDataRenderDataTableLayout());
	AddRenderDataTableLayout(RenderingSystem::Instance->GetLightingSystem()->GetLightingDataRenderDataTableLayout());

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::FRAGMENT, 0, sizeof(VolumetricLightingPushConstantData));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution(1));

	//Set the properties of the render pass.
	SetShouldClear(false);
	SetBlendEnabled(false);
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
void VolumetricLightingGraphicsPipeline::Execute() NOEXCEPT
{
	//Retrieve and set the command buffer.
	CommandBuffer *const RESTRICT command_buffer{ RenderingSystem::Instance->GetGlobalCommandBuffer(CommandBufferLevel::SECONDARY) };
	SetCommandBuffer(command_buffer);

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	command_buffer->BindRenderDataTable(this, 1, RenderingSystem::Instance->GetModelSystem()->GetCurrentModelDataRenderDataTable());
	command_buffer->BindRenderDataTable(this, 2, RenderingSystem::Instance->GetLightingSystem()->GetCurrentLightingDataRenderDataTable());

	//Push constants.
	VolumetricLightingPushConstantData data;

	data._SkyLightLuminance = VectorConstants::ZERO;
	data._SkyLightScreenSpacePosition = Vector2<float32>(FLOAT_MAXIMUM, FLOAT_MAXIMUM);

	const uint64 number_of_light_components{ ComponentManager::GetNumberOfLightComponents() };
	const LightComponent * RESTRICT component{ ComponentManager::GetLightLightComponents() };

	for (uint64 i{ 0 }; i < number_of_light_components; ++i, ++component)
	{
		if (component->_LightType == static_cast<uint32>(LightType::DIRECTIONAL))
		{
			data._SkyLightLuminance = component->_Color * component->_Intensity;
			data._SkyLightScreenSpacePosition = RenderingUtilities::CalculateScreenCoordinate(*Perceiver::Instance->GetViewMatrix(), Perceiver::Instance->GetPosition() - component->_Direction);

			break;
		}
	}

	command_buffer->PushConstants(this, ShaderStage::FRAGMENT, 0, sizeof(VolumetricLightingPushConstantData), &data);

	//Draw!
	command_buffer->Draw(this, 3, 1);

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}