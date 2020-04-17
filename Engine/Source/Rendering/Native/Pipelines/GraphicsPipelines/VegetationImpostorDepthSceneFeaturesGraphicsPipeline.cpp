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

	int32 _MaterialIndex;
	float _CutoffDistanceSquared;

};

/*
*	Initializes this graphics pipeline.
*/
void VegetationImpostorDepthSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depthBuffer) NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(Shader::VegetationImpostorDepthSceneFeaturesVertex);
	SetTessellationControlShader(Shader::NONE);
	SetTessellationEvaluationShader(Shader::NONE);
	SetGeometryShader(Shader::VegetationImpostorDepthSceneFeaturesGeometry);
	SetFragmentShader(Shader::VegetationImpostorDepthSceneFeaturesFragment);

	//Set the depth buffer.
	SetDepthBuffer(depthBuffer);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::GEOMETRY, 0, sizeof(VegetationImpostorGeometryPushConstantData));
	AddPushConstantRange(ShaderStage::FRAGMENT, sizeof(VegetationImpostorGeometryPushConstantData), sizeof(VegetationImpostorFragmentPushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(4);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										offsetof(Matrix4x4, _Matrix[0]));
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										offsetof(Matrix4x4, _Matrix[1]));
	AddVertexInputAttributeDescription(	2,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										offsetof(Matrix4x4, _Matrix[2]));
	AddVertexInputAttributeDescription(	3,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										offsetof(Matrix4x4, _Matrix[3]));

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(Matrix4x4), VertexInputBindingDescription::InputRate::Instance);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution(0));

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
	CommandBuffer *const RESTRICT command_buffer{ GetCurrentCommandBuffer() };
	const VegetationComponent *RESTRICT component{ ComponentManager::GetVegetationVegetationComponents() };

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

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

		//Need a correct material index to render.
		if (component->_ImpostorMaterialIndex >= CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS)
		{
			continue;
		}

		//Push constants.
		VegetationImpostorGeometryPushConstantData geometry_data;

		geometry_data._ImpostorHalfWidth = component->_ImpostorHalfWidth;
		geometry_data._ImpostorHeight = component->_ImpostorHeight;

		command_buffer->PushConstants(this, ShaderStage::GEOMETRY, 0, sizeof(VegetationImpostorGeometryPushConstantData), &geometry_data);

		VegetationImpostorFragmentPushConstantData fragment_data;

		fragment_data._MaterialIndex = component->_ImpostorMaterialIndex;
		fragment_data._CutoffDistanceSquared = component->_CutoffDistance * component->_CutoffDistance;

		command_buffer->PushConstants(this, ShaderStage::FRAGMENT, sizeof(VegetationImpostorGeometryPushConstantData), sizeof(VegetationImpostorFragmentPushConstantData), &fragment_data);

		//Bind the transformations buffer.
		command_buffer->BindVertexBuffer(this, 0, component->_TransformationsBuffer, &OFFSET);

		//Draw!
		command_buffer->Draw(this, 1, component->_NumberOfTransformations);
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}