//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Vertex.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>

/*
*	Particle system masked depth geometry push constant data definition.
*/
class ParticleSystemMaskedDepthGeometryPushConstantData final
{

public:

	//The lifetime.
	float32 _Lifetime;

	//The fade time.
	float32 _FadeTime;

};

/*
*	Particle system masked depth fragment push constant data definition.
*/
class ParticleSystemMaskedDepthFragmentPushConstantData final
{

public:

	int32 _MaterialIndex;

};

/*
*	Initializes this graphics pipeline.
*/
void ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depth_buffer) NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(Shader::ParticleSystemMaskedDepthSceneFeaturesVertexShader);
	SetTessellationControlShader(Shader::NONE);
	SetTessellationEvaluationShader(Shader::NONE);
	SetGeometryShader(Shader::ParticleSystemMaskedDepthSceneFeaturesGeometryShader);
	SetFragmentShader(Shader::ParticleSystemMaskedDepthSceneFeaturesFragmentShader);

	//Set the depth buffer.
	SetDepthBuffer(depth_buffer);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::GEOMETRY, 0, sizeof(ParticleSystemMaskedDepthGeometryPushConstantData));
	AddPushConstantRange(ShaderStage::FRAGMENT, sizeof(ParticleSystemMaskedDepthGeometryPushConstantData), sizeof(ParticleSystemMaskedDepthFragmentPushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(4);
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
	AddVertexInputAttributeDescription(	3,
										0,
										VertexInputAttributeDescription::Format::X32SignedFloat,
										sizeof(Vector3<float>) + sizeof(Vector3<float>) + sizeof(Vector2<float>));

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(ParticleInstanceData), VertexInputBindingDescription::InputRate::Instance);

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
void ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline::Execute() NOEXCEPT
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
	CommandBuffer* const RESTRICT command_buffer{ GetCurrentCommandBuffer() };
	const ParticleSystemComponent *RESTRICT component{ ComponentManager::GetParticleSystemParticleSystemComponents() };
	const ParticleSystemRenderComponent* RESTRICT render_component{ ComponentManager::GetParticleSystemParticleSystemRenderComponents() };

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	for (uint64 i = 0; i < number_of_particle_system_components; ++i, ++component, ++render_component)
	{
		//Push constants.
		{
			ParticleSystemMaskedDepthGeometryPushConstantData data;

			data._Lifetime = component->_Lifetime;
			data._FadeTime = component->_FadeTime;

			command_buffer->PushConstants(this, ShaderStage::GEOMETRY, 0, sizeof(ParticleSystemMaskedDepthGeometryPushConstantData), &data);
		}

		{
			ParticleSystemMaskedDepthFragmentPushConstantData data;

			data._MaterialIndex = render_component->_MaterialIndex;

			command_buffer->PushConstants(this, ShaderStage::FRAGMENT, sizeof(ParticleSystemMaskedDepthGeometryPushConstantData), sizeof(ParticleSystemMaskedDepthFragmentPushConstantData), &data);
		}

		//Bind the transformations buffer.
		command_buffer->BindVertexBuffer(this, 0, render_component->_TransformationsBuffer, &OFFSET);

		//Draw!
		command_buffer->Draw(this, 1, render_component->_NumberOfInstances);
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}