//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/VegetationImpostorDepthSceneFeaturesGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Vertex.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/RenderingSystem.h>

/*
*	Vegetation impostor geometry push constant data definition.
*/
class VegetationImpostorGeometryPushConstantData final
{

public:

	float _ImpostorHalfWidth;
	float _ImpostorHeight;

};

/*
*	Vegetation impostor fragment push constant data definition.
*/
class VegetationImpostorFragmentPushConstantData final
{

public:

	int32 _MaskTextureIndex;
	float _CutoffDistanceSquared;

};

/*
*	Initializes this graphics pipeline.
*/
void VegetationImpostorDepthSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depthBuffer) NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(Shader::VegetationImpostorDepthSceneFeaturesVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::VegetationImpostorDepthSceneFeaturesGeometry);
	SetFragmentShader(Shader::VegetationImpostorDepthSceneFeaturesFragment);

	//Set the depth buffer.
	SetDepthBuffer(depthBuffer);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::Geometry, 0, sizeof(VegetationImpostorGeometryPushConstantData));
	AddPushConstantRange(ShaderStage::Fragment, sizeof(VegetationImpostorGeometryPushConstantData), sizeof(VegetationImpostorFragmentPushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(4);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										offsetof(Matrix4, _Matrix[0]));
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										offsetof(Matrix4, _Matrix[1]));
	AddVertexInputAttributeDescription(	2,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										offsetof(Matrix4, _Matrix[2]));
	AddVertexInputAttributeDescription(	3,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										offsetof(Matrix4, _Matrix[3]));

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(Matrix4), VertexInputBindingDescription::InputRate::Instance);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
	SetShouldClear(false);
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::None);
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
	SetTopology(Topology::PointList);
}

/*
*	Executes this graphics pipeline.
*/
void VegetationImpostorDepthSceneFeaturesGraphicsPipeline::Execute() NOEXCEPT
{
	//Define constants.
	constexpr uint64 OFFSET{ 0 };

	//Iterate over all vegetation components and draw them all.
	const uint64 numberOfVegetationComponents{ ComponentManager::GetNumberOfVegetationComponents() };

	//If there's none to render - render none.
	if (numberOfVegetationComponents == 0)
	{
		//Don't include this render pass in the final render.
		SetIncludeInRender(false);

		return;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };
	const VegetationComponent *RESTRICT component{ ComponentManager::GetVegetationVegetationComponents() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Wait for vegetation culling and level of detail to finish.
	CullingSystem::Instance->WaitForVegetationCulling();
	LevelOfDetailSystem::Instance->WaitForVegetationLevelOfDetail();

	for (uint64 i = 0; i < numberOfVegetationComponents; ++i, ++component)
	{
		//Don't draw if it's not visible.
		if (!component->_Visibility)
		{
			continue;
		}

		//Don't draw if it's not the correct level of detail.
		if (component->_LevelOfDetail != VegetationComponent::LevelOfDetail::Impostor)
		{
			continue;
		}

		//Need a correct texture index to render.
		if (component->_ImpostorMaskTextureIndex >= RenderingConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES)
		{
			continue;
		}

		//Push constants.
		VegetationImpostorGeometryPushConstantData geometry_data;

		geometry_data._ImpostorHalfWidth = component->_ImpostorHalfWidth;
		geometry_data._ImpostorHeight = component->_ImpostorHeight;

		commandBuffer->PushConstants(this, ShaderStage::Geometry, 0, sizeof(VegetationImpostorGeometryPushConstantData), &geometry_data);

		VegetationImpostorFragmentPushConstantData fragment_data;

		fragment_data._MaskTextureIndex = component->_ImpostorMaskTextureIndex;
		fragment_data._CutoffDistanceSquared = component->_CutoffDistance * component->_CutoffDistance;

		commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(VegetationImpostorGeometryPushConstantData), sizeof(VegetationImpostorFragmentPushConstantData), &fragment_data);

		//Bind the transformations buffer.
		commandBuffer->BindVertexBuffer(this, 0, component->_TransformationsBuffer, &OFFSET);

		commandBuffer->Draw(this, 1, component->_NumberOfTransformations);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}