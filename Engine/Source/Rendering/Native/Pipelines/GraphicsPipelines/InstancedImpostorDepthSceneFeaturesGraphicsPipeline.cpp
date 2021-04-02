//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/InstancedImpostorDepthSceneFeaturesGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/Vertex.h>

//Systems.
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Instanced impostor depth scene features vertex push constant data definition.
*/
class InstancedImpostorDepthSceneFeaturesPushConstantData final
{

public:

	//The world grid delta.
	Vector3<float32> _WorldGridDelta;

	//Some padding.
	Padding<4> _Padding;

	//The half width.
	float32 _HalfWidth;

	//The whole width.
	float32 _WholeWidth;

	//The height.
	float32 _Height;

	//The material index.
	uint32 _MaterialIndex;

	//The start fade in distance.
	float32 _StartFadeInDistance;

	//The end fade in distance.
	float32 _EndFadeInDistance;

	//The start fade out distance.
	float32 _StartFadeOutDistance;

	//The end fade out distance.
	float32 _EndFadeOutDistance;

};

/*
*	Initializes this graphics pipeline.
*/
void InstancedImpostorDepthSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depth_buffer) NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("InstancedImpostorDepthSceneFeaturesVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("InstancedImpostorDepthSceneFeaturesFragmentShader")));

	//Set the depth buffer.
	SetDepthBuffer(depth_buffer);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(InstancedImpostorDepthSceneFeaturesPushConstantData));

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
	SetShouldClear(false);
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::Back);
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
	SetTopology(Topology::TriangleFan);
}

/*
*	Executes this graphics pipeline.
*/
void InstancedImpostorDepthSceneFeaturesGraphicsPipeline::Execute() NOEXCEPT
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
				InstancedImpostorDepthSceneFeaturesPushConstantData data;

				const Vector3<int32> delta{ component->_Cell - WorldSystem::Instance->GetCurrentWorldGridCell() };

				for (uint8 i{ 0 }; i < 3; ++i)
				{
					data._WorldGridDelta[i] = static_cast<float32>(delta[i]) * WorldSystem::Instance->GetWorldGridSize();
				}

				data._HalfWidth = component->_Dimensions._X * 0.5f;
				data._WholeWidth = component->_Dimensions._X;
				data._Height = component->_Dimensions._Y;
				data._MaterialIndex = component->_MaterialResource->_Index;
				data._StartFadeInDistance = component->_StartFadeInDistance;
				data._EndFadeInDistance = component->_EndFadeInDistance;
				data._StartFadeOutDistance = component->_StartFadeOutDistance;
				data._EndFadeOutDistance = component->_EndFadeOutDistance;

				command_buffer->PushConstants(this, ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, sizeof(InstancedImpostorDepthSceneFeaturesPushConstantData), &data);
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