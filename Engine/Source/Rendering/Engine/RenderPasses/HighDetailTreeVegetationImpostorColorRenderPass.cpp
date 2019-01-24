//Header file.
#include <Rendering/Engine/RenderPasses/HighDetailTreeVegetationImpostorColorRenderPass.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/VegetationSystem.h>

//Singleton definition.
DEFINE_SINGLETON(HighDetailTreeVegetationImpostorColorRenderPass);

/*
*	Geometry push constant data definition.
*/
class GeometryPushConstantData final
{

public:

	float _ImpostorHalfWidth;
	float _ImpostorHeight;

};

/*
*	Fragment push constant data definition.
*/
class FragmentPushConstantData final
{

public:

	int32 _AlbedoTextureIndex;

};

/*
*	Default constructor.
*/
HighDetailTreeVegetationImpostorColorRenderPass::HighDetailTreeVegetationImpostorColorRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		HighDetailTreeVegetationImpostorColorRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the high detail tree vegetation impostor color render pass.
*/
void HighDetailTreeVegetationImpostorColorRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::HighDetailTreeVegetationImpostorColor);

	//Set the shaders.
	SetVertexShader(Shader::HighDetailTreeVegetationImpostorColorVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::HighDetailTreeVegetationImpostorColorGeometry);
	SetFragmentShader(Shader::HighDetailTreeVegetationImpostorColorFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(3);
	AddRenderTarget(RenderTarget::SceneBufferAlbedo);
	AddRenderTarget(RenderTarget::SceneBufferNormalDepth);
	AddRenderTarget(RenderTarget::SceneBufferMaterialProperties);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::Geometry, 0, sizeof(GeometryPushConstantData));
	AddPushConstantRange(ShaderStage::Fragment, sizeof(GeometryPushConstantData), sizeof(FragmentPushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(4);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										0);
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(Vector4<float>));
	AddVertexInputAttributeDescription(	2,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(Vector4<float>) * 2);
	AddVertexInputAttributeDescription(	3,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(Vector4<float>) * 3);

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(Matrix4), VertexInputBindingDescription::InputRate::Instance);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
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
	SetStencilPassOperator(StencilOperator::Keep);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(0);
	SetTopology(Topology::PointList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		HighDetailTreeVegetationImpostorColorRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the high detail tree vegetation impostor colors.
*/
void HighDetailTreeVegetationImpostorColorRenderPass::RenderInternal() NOEXCEPT
{
	//Retrieve the tree vegetion type informations.
	const DynamicArray<TreeVegetationTypeInformation> *const RESTRICT informations{ VegetationSystem::Instance->GetTreeVegetationTypeInformations() };

	//If there's none to render - render none.
	if (informations->Empty())
	{
		//Don't include this render pass in the final render.
		SetIncludeInRender(false);

		return;
	}

	//Cache the command buffer
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Wait for the tree vegetation culling to finish.
	CullingSystem::Instance->WaitForTreeVegetationCulling();

	for (const TreeVegetationTypeInformation &information : *informations)
	{
		//Push constants.
		GeometryPushConstantData geometryData;

		geometryData._ImpostorHalfWidth = information._Properties._ImpostorWidth * 0.5f;
		geometryData._ImpostorHeight = information._Properties._ImpostorHeight;

		commandBuffer->PushConstants(this, ShaderStage::Geometry, 0, sizeof(GeometryPushConstantData), &geometryData);

		FragmentPushConstantData fragmentData;

		fragmentData._AlbedoTextureIndex = information._Material._ImpostorAlbedoTextureIndex;

		commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(GeometryPushConstantData), sizeof(FragmentPushConstantData), &fragmentData);

		for (const TreeVegetationPatchRenderInformation &renderInformation : information._PatchRenderInformations)
		{
			//Check whether or not this should be drawn.
			if (!TEST_BIT(renderInformation._Visibilities[UNDERLYING(TreeVegetationLevelOfDetail::Impostor)], VisibilityFlag::Viewer)
				|| renderInformation._NumberOfTransformations[UNDERLYING(TreeVegetationLevelOfDetail::Impostor)] == 0)
			{
				continue;
			}

			//Bind the transformations buffer.
			commandBuffer->BindVertexBuffer(this, 0, renderInformation._TransformationsBuffer, &renderInformation._TransformationsOffsets[UNDERLYING(TreeVegetationLevelOfDetail::Impostor)]);

			//Draw the instances!
			commandBuffer->Draw(this, 1, renderInformation._NumberOfTransformations[UNDERLYING(TreeVegetationLevelOfDetail::Impostor)]);
		}
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}