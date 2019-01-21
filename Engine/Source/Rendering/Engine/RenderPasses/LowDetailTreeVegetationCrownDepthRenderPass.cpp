//Header file.
#include <Rendering/Engine/RenderPasses/LowDetailTreeVegetationCrownDepthRenderPass.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/VegetationSystem.h>

//Vegetation.
#include <Vegetation/VegetationVertex.h>

//Singleton definition.
DEFINE_SINGLETON(LowDetailTreeVegetationCrownDepthRenderPass);

/*
*	Vertex push constant data definition.
*/
class VertexPushConstantData final
{

public:

	float _FadeStartDistanceSquared;
	float _InverseFadeDistanceSquared;

};

/*
*	Fragment push constant data definition.
*/
class FragmentPushConstantData final
{

public:

	int32 _MaskTextureIndex;

};

/*
*	Default constructor.
*/
LowDetailTreeVegetationCrownDepthRenderPass::LowDetailTreeVegetationCrownDepthRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		LowDetailTreeVegetationCrownDepthRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the low detail tree vegetation crown depth render pass.
*/
void LowDetailTreeVegetationCrownDepthRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::LowDetailTreeVegetationCrownDepth);

	//Set the shaders.
	SetVertexShader(Shader::LowDetailTreeVegetationCrownDepthVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::LowDetailTreeVegetationCrownDepthFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GrassMaterial));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(VertexPushConstantData));
	AddPushConstantRange(ShaderStage::Fragment, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(9);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(VegetationVertex, _Position));
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(VegetationVertex, _Normal));
	AddVertexInputAttributeDescription(	2,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(VegetationVertex, _Tangent));
	AddVertexInputAttributeDescription(	3,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										offsetof(VegetationVertex, _TextureCoordinate));
	AddVertexInputAttributeDescription(	4,
										0,
										VertexInputAttributeDescription::Format::X32SignedFloat,
										offsetof(VegetationVertex, _ModulatorFactor));
	AddVertexInputAttributeDescription(	5,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										0);
	AddVertexInputAttributeDescription(	6,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(Vector4<float>));
	AddVertexInputAttributeDescription(	7,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(Vector4<float>) * 2);
	AddVertexInputAttributeDescription(	8,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(Vector4<float>) * 3);

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(2);
	AddVertexInputBindingDescription(0, sizeof(VegetationVertex), VertexInputBindingDescription::InputRate::Vertex);
	AddVertexInputBindingDescription(1, sizeof(Matrix4), VertexInputBindingDescription::InputRate::Instance);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
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
	SetStencilWriteMask(BIT(0) | BIT(3));
	SetStencilReferenceMask(BIT(0) | BIT(3));
	SetTopology(Topology::TriangleList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		LowDetailTreeVegetationCrownDepthRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the crown depth of the low detail tree vegetation.
*/
void LowDetailTreeVegetationCrownDepthRenderPass::RenderInternal() NOEXCEPT
{
	//Retrieve the tree vegetation type informations.
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
		//Bind the model vertex and index buffer.
		constexpr uint64 offset{ 0 };

		commandBuffer->BindVertexBuffer(this, 0, information._Model._CrownBuffers[UNDERLYING(LevelOfDetail::Low)], &offset);
		commandBuffer->BindIndexBuffer(this, information._Model._CrownBuffers[UNDERLYING(LevelOfDetail::Low)], information._Model._CrownIndexOffsets[UNDERLYING(LevelOfDetail::Low)]);

		//Push constants.
		VertexPushConstantData vertexData;

		vertexData._FadeStartDistanceSquared = information._Properties._LowDetailDistance * information._Properties._LowDetailDistance;
		vertexData._InverseFadeDistanceSquared = 1.0f / (information._Properties._CutoffDistance * information._Properties._CutoffDistance);

		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(VertexPushConstantData), &vertexData);

		FragmentPushConstantData fragmentData;

		fragmentData._MaskTextureIndex = information._Material._CrownMaskTextureIndex;

		commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData), &fragmentData);

		for (const TreeVegetationPatchRenderInformation &renderInformation : information._PatchRenderInformations)
		{
			//Check whether or not this should be drawn.
			if (!TEST_BIT(renderInformation._Visibilities[UNDERLYING(LevelOfDetail::Low)], VisibilityFlag::Viewer)
				|| renderInformation._NumberOfTransformations[UNDERLYING(LevelOfDetail::Low)] == 0)
			{
				continue;
			}

			//Bind the transformations buffer.
			commandBuffer->BindVertexBuffer(this, 1, renderInformation._TransformationsBuffers[UNDERLYING(LevelOfDetail::Low)], &offset);

			//Draw the instances!
			commandBuffer->DrawIndexed(this, information._Model._CrownIndexCounts[UNDERLYING(LevelOfDetail::Low)], renderInformation._NumberOfTransformations[UNDERLYING(LevelOfDetail::Low)]);
		}
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}