//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Vertex.h>
#include <Rendering/Native/RenderPasses/SceneFeaturesRenderPass.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Particle system masked color geometry push constant data definition.
*/
class ParticleSystemMaskedColorGeometryPushConstantData final
{

public:

	//The position delta.
	Vector3<float32> _PositionDelta;

	//Padding.
	Padding<4> _Padding;

};

/*
*	Particle system masked color fragment push constant data definition.
*/
class ParticleSystemMaskedColorFragmentPushConstantData final
{

public:

	uint32 _MaterialIndex;

};

/*
*	Initializes this graphics pipeline.
*/
void ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depth_buffer) NOEXCEPT
{
	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("ParticleSystemMaskedColorSceneFeaturesVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourceSystem::Instance->GetShaderResource(HashString("ParticleSystemMaskedColorSceneFeaturesGeometryShader")));
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("ParticleSystemMaskedColorSceneFeaturesFragmentShader")));

	//Set the depth buffer.
	SetDepthBuffer(depth_buffer);

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(5);
	AddOutputRenderTarget(SceneFeaturesRenderPass::Instance->GetSceneFeatures1RenderTarget());
	AddOutputRenderTarget(SceneFeaturesRenderPass::Instance->GetSceneFeatures2RenderTarget());
	AddOutputRenderTarget(SceneFeaturesRenderPass::Instance->GetSceneFeatures3RenderTarget());
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_4));
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::GEOMETRY, 0, sizeof(ParticleSystemMaskedColorGeometryPushConstantData));
	AddPushConstantRange(ShaderStage::FRAGMENT, sizeof(ParticleSystemMaskedColorGeometryPushConstantData), sizeof(ParticleSystemMaskedColorFragmentPushConstantData));

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
	SetDepthStencilAttachmentLoadOperator(AttachmentLoadOperator::LOAD);
	SetDepthStencilAttachmentStoreOperator(AttachmentStoreOperator::STORE);
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::LOAD);
	SetColorAttachmentStoreOperator(AttachmentStoreOperator::STORE);
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

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName("Particle System Masked Color Scene Features");
#endif
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

	//Retrieve and set the command buffer.
	CommandBuffer *const RESTRICT command_buffer{ RenderingSystem::Instance->GetGlobalCommandBuffer(CommandBufferLevel::SECONDARY) };
	SetCommandBuffer(command_buffer);

	//Cache data the will be used.
	const ParticleSystemComponent *RESTRICT component{ ComponentManager::GetParticleSystemParticleSystemComponents() };
	const ParticleSystemRenderComponent *RESTRICT render_component{ ComponentManager::GetParticleSystemParticleSystemRenderComponents() };

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
			ParticleSystemMaskedColorGeometryPushConstantData data;

			const Vector3<int32> position_delta{ component->_OriginalWorldGridCell - WorldSystem::Instance->GetCurrentWorldGridCell() };
			const float32 world_grid_size{ WorldSystem::Instance->GetWorldGridSize() };

			data._PositionDelta = Vector3<float32>(static_cast<float32>(position_delta._X), static_cast<float32>(position_delta._Y), static_cast<float32>(position_delta._Z)) * world_grid_size;

			command_buffer->PushConstants(this, ShaderStage::GEOMETRY, 0, sizeof(ParticleSystemMaskedColorGeometryPushConstantData), &data);
		}

		{
			ParticleSystemMaskedColorFragmentPushConstantData data;

			data._MaterialIndex = render_component->_MaterialResource->_Index;

			command_buffer->PushConstants(this, ShaderStage::FRAGMENT, sizeof(ParticleSystemMaskedColorGeometryPushConstantData), sizeof(ParticleSystemMaskedColorFragmentPushConstantData), &data);
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

/*
*	Terminates this graphics pipeline.
*/
void ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline::Terminate() NOEXCEPT
{

}