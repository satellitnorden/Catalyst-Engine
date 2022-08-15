//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/InstancedImpostorColorSceneFeaturesGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Vertex.h>
#include <Rendering/Native/RenderPasses/SceneFeaturesRenderPass.h>

//Systems.
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Instanced impostor color scene features vertex push constant data definition.
*/
class InstancedImpostorColorSceneFeaturesPushConstantData final
{

public:

	//The world grid delta.
	Vector3<float32> _WorldGridDelta;

	//Padding.
	Padding<4> _Padding;

	//The half width.
	float32 _HalfWidth;

	//The whole width.
	float32 _WholeWidth;

	//The height.
	float32 _Height;

	//The material index.
	uint32 _MaterialIndex;

};

/*
*	Initializes this graphics pipeline.
*/
void InstancedImpostorColorSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depth_buffer) NOEXCEPT
{
	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("InstancedImpostorColorSceneFeaturesVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("InstancedImpostorColorSceneFeaturesFragmentShader")));

	//Set the depth buffer.
	SetDepthBuffer(depth_buffer);

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(5);
	AddOutputRenderTarget(SceneFeaturesRenderPass::Instance->GetSceneFeatures1RenderTarget());
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_2));
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_3));
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_4));
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(InstancedImpostorColorSceneFeaturesPushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(1);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										0);

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(Vector3<float32>), VertexInputBindingDescription::InputRate::Instance);

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
	SetCullMode(CullMode::Back);
	SetDepthCompareOperator(CompareOperator::Equal);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(false);
	SetStencilTestEnabled(false);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Replace);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(RenderingConstants::SCENE_BUFFER_STENCIL_BIT);
	SetStencilReferenceMask(RenderingConstants::SCENE_BUFFER_STENCIL_BIT);
	SetTopology(Topology::TriangleFan);

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName("Instanced Impostor Color Scene Features");
#endif
}

/*
*	Executes this graphics pipeline.
*/
void InstancedImpostorColorSceneFeaturesGraphicsPipeline::Execute() NOEXCEPT
{
	//Define constants.
	constexpr uint64 OFFSET{ 0 };

	//Cache the number of components.
	const uint64 number_of_components{ ComponentManager::GetNumberOfInstancedImpostorComponents() };

	//If there's no instanced impostor entities, then fudge it.
	if (number_of_components == 0)
	{
		SetIncludeInRender(false);

		return;
	}

	//Retrieve and set the command buffer.
	CommandBuffer *const RESTRICT command_buffer{ RenderingSystem::Instance->GetGlobalCommandBuffer(CommandBufferLevel::SECONDARY) };
	SetCommandBuffer(command_buffer);

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Draw instanced impostors.
	{
		//Cache relevant data.
		const InstancedImpostorComponent *RESTRICT component{ ComponentManager::GetInstancedImpostorInstancedImpostorComponents() };

		for (uint64 component_index{ 0 }; component_index < number_of_components; ++component_index, ++component)
		{
			//Bind the transformations buffer.
			command_buffer->BindVertexBuffer(this, 0, component->_TransformationsBuffer, &OFFSET);

			//Push constants.
			{
				InstancedImpostorColorSceneFeaturesPushConstantData data;

				const Vector3<int32> delta{ component->_Cell - WorldSystem::Instance->GetCurrentWorldGridCell() };

				for (uint8 i{ 0 }; i < 3; ++i)
				{
					data._WorldGridDelta[i] = static_cast<float32>(delta[i]) * WorldSystem::Instance->GetWorldGridSize();
				}

				data._HalfWidth = component->_Dimensions._X * 0.5f;
				data._WholeWidth = component->_Dimensions._X;
				data._Height = component->_Dimensions._Y;
				data._MaterialIndex = component->_MaterialResource->_Index;

				command_buffer->PushConstants(this, ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(InstancedImpostorColorSceneFeaturesPushConstantData), &data);
			}

			//Draw!
			command_buffer->Draw(this, 4, component->_NumberOfTransformations);
		}
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}

/*
*	Terminates this graphics pipeline.
*/
void InstancedImpostorColorSceneFeaturesGraphicsPipeline::Terminate() NOEXCEPT
{

}