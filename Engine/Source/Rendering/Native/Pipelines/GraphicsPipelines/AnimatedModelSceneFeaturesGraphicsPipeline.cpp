//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/AnimatedModelSceneFeaturesGraphicsPipeline.h>

//Animation.
#include <Animation/AnimatedVertex.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/AnimationSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

/*
*	Vertex push constant data definition.
*/
class VertexPushConstantData final
{

public:

	Matrix4x4 _PreviousModelMatrix;
	Matrix4x4 _CurrentModelMatrix;

};

/*
*	Fragment push constant data definition.
*/
class FragmentPushConstantData final
{

public:

	int32 _AlbedoTextureIndex;
	int32 _NormalMapTextureIndex;
	int32 _MaterialPropertiesTextureIndex;
	int32 _MaterialProperties;
	float _LuminanceMultiplier;

};

/*
*	Initializes this graphics pipeline.
*/
void AnimatedModelSceneFeaturesGraphicsPipeline::Initialize(const DepthBufferHandle depthBuffer) NOEXCEPT
{
	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("AnimatedModelSceneFeaturesVertexShader"))->_Handle);
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("AnimatedModelSceneFeaturesFragmentShader"))->_Handle);

	//Set the depth buffer.
	SetDepthBuffer(depthBuffer);

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(4);
	AddOutputRenderTarget(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_1));
	AddOutputRenderTarget(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_2));
	AddOutputRenderTarget(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_3));
	AddOutputRenderTarget(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_4));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));
	AddRenderDataTableLayout(AnimationSystem::Instance->GetAnimationDataRenderDataTableLayout());

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::VERTEX, 0, sizeof(VertexPushConstantData));
	AddPushConstantRange(ShaderStage::FRAGMENT, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(6);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(AnimatedVertex, _Position));
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(AnimatedVertex, _Normal));
	AddVertexInputAttributeDescription(	2,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(AnimatedVertex, _Tangent));
	AddVertexInputAttributeDescription(	3,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedInt,
										offsetof(AnimatedVertex, _BoneIndices));
	AddVertexInputAttributeDescription(	4,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(AnimatedVertex, _BoneWeights));
	AddVertexInputAttributeDescription(	5,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										offsetof(AnimatedVertex, _TextureCoordinate));

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(AnimatedVertex), VertexInputBindingDescription::InputRate::Vertex);

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
	SetTopology(Topology::TriangleList);

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName("Animated Model Scene Features");
#endif
}

/*
*	Executes this graphics pipeline.
*/
void AnimatedModelSceneFeaturesGraphicsPipeline::Execute() NOEXCEPT
{
	//Define constants.
	constexpr uint64 OFFSET{ 0 };

	//Iterate over all model components and draw them all.
	const uint64 numberOfAnimatedModelComponents{ ComponentManager::GetNumberOfAnimatedModelComponents() };

	//If there's none to render - render none.
	if (numberOfAnimatedModelComponents == 0)
	{
		//Don't include this render pass in the final render.
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

	//Draw all animated models
	const AnimatedModelComponent *RESTRICT component{ ComponentManager::GetAnimatedModelAnimatedModelComponents() };

	for (uint64 i = 0; i < numberOfAnimatedModelComponents; ++i, ++component)
	{
		//Bind the vertex/inder buffer.
		command_buffer->BindVertexBuffer(this, 0, component->_AnimatedModelResource->_VertexBuffer, &OFFSET);
		command_buffer->BindIndexBuffer(this, component->_AnimatedModelResource->_IndexBuffer, OFFSET);

		//Push constants.
		VertexPushConstantData vertexData;

		vertexData._PreviousModelMatrix = component->_PreviousWorldTransform;
		vertexData._CurrentModelMatrix = component->_CurrentWorldTransform;

		command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(VertexPushConstantData), &vertexData);

		FragmentPushConstantData fragmentData;

		fragmentData._AlbedoTextureIndex = 0;
		fragmentData._NormalMapTextureIndex = 0;
		fragmentData._MaterialPropertiesTextureIndex = 0;
		fragmentData._MaterialProperties = 0;
		fragmentData._LuminanceMultiplier = 0.0f;

		command_buffer->PushConstants(this, ShaderStage::FRAGMENT, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData), &fragmentData);

		//Bind the aimation data render data table.
		command_buffer->BindRenderDataTable(this, 1, component->_AnimationDataRenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()]);

		command_buffer->DrawIndexed(this, component->_AnimatedModelResource->_IndexCount, 1);
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}

/*
*	Terminates this graphics pipeline.
*/
void AnimatedModelSceneFeaturesGraphicsPipeline::Terminate() NOEXCEPT
{

}