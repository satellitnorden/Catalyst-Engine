//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Vertex.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>

/*
*	Particle system masked depth push constant data definition.
*/
class ParticleSystemMaskedDepthFragmentPushConstantData final
{

public:

	int32 _MaterialIndex;

};

/*
*	Initializes this graphics pipeline.
*/
void ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depth_buffer) NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(Shader::ParticleSystemMaskedDepthSceneFeaturesVertexShader);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::ParticleSystemMaskedDepthSceneFeaturesGeometryShader);
	SetFragmentShader(Shader::ParticleSystemMaskedDepthSceneFeaturesFragmentShader);

	//Set the depth buffer.
	SetDepthBuffer(depth_buffer);

	//Add the render targets.
	SetNumberOfRenderTargets(3);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures1));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures2));
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures3));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Fragment, 0, sizeof(ParticleSystemMaskedDepthFragmentPushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(3);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										0);
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										sizeof(Vector3<float>));
	AddVertexInputAttributeDescription(	2,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										sizeof(Vector3<float>) + sizeof(Vector3<float>));

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
	SetTopology(Topology::PointList);
}

/*
*	Executes this graphics pipeline.
*/
void ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline::Execute() NOEXCEPT
{
	//Define constants.
	constexpr uint64 OFFSET{ 0 };

	//Iterate over all particle system components and draw them all.
	const uint64 number_of_particle_system_components{ ComponentManager::GetNumberOfParticleSystemComponents() };

	//If there's none to render - render none.
	if (number_of_particle_system_components == 0)
	{
		//Don't include this render pass in the final render.
		SetIncludeInRender(false);

		return;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT command_buffer{ GetCurrentCommandBuffer() };
	const ParticleSystemComponent *RESTRICT component{ ComponentManager::GetParticleSystemParticleSystemComponents() };

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	for (uint64 i = 0; i < number_of_particle_system_components; ++i, ++component)
	{
		/*
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
		if (component->_ImpostorMaterialIndex >= RenderingConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS)
		{
			continue;
		}

		//Push constants.
		VegetationImpostorGeometryPushConstantData geometry_data;

		geometry_data._ImpostorHalfWidth = component->_ImpostorHalfWidth;
		geometry_data._ImpostorHeight = component->_ImpostorHeight;

		commandBuffer->PushConstants(this, ShaderStage::Geometry, 0, sizeof(VegetationImpostorGeometryPushConstantData), &geometry_data);

		//Push constants.
		VegetationImpostorFragmentPushConstantData fragment_data;

		fragment_data._MaterialIndex = component->_ImpostorMaterialIndex;

		commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(VegetationImpostorGeometryPushConstantData), sizeof(VegetationImpostorFragmentPushConstantData), &fragment_data);

		//Bind the transformations buffer.
		commandBuffer->BindVertexBuffer(this, 0, component->_TransformationsBuffer, &OFFSET);

		commandBuffer->Draw(this, 1, component->_NumberOfTransformations);
		*/
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}