//Header file.
#include <Rendering/Engine/RenderPasses/LowDetailTreeVegetationCrownColorRenderPass.h>

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
DEFINE_SINGLETON(LowDetailTreeVegetationCrownColorRenderPass);

/*
*	Push constant data definition.
*/
class PushConstantData final
{

public:

	int32 _AlbedoTextureIndex;
	int32 _NormalMapTextureIndex;

};

/*
*	Default constructor.
*/
LowDetailTreeVegetationCrownColorRenderPass::LowDetailTreeVegetationCrownColorRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		LowDetailTreeVegetationCrownColorRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the low detail grass vegetation color render pass.
*/
void LowDetailTreeVegetationCrownColorRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::LowDetailTreeVegetationCrownColor);

	//Set the shaders.
	SetVertexShader(Shader::LowDetailTreeVegetationCrownColorVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::LowDetailTreeVegetationCrownColorFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(3);
	AddRenderTarget(RenderTarget::SceneBufferAlbedo);
	AddRenderTarget(RenderTarget::SceneBufferNormalDepth);
	AddRenderTarget(RenderTarget::SceneBufferMaterialProperties);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GrassMaterial));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Fragment, 0, sizeof(PushConstantData));

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
	SetDepthCompareOperator(CompareOperator::Equal);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(false);
	SetStencilTestEnabled(false);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Keep);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Equal);
	SetStencilCompareMask(0);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(0);
	SetTopology(Topology::TriangleList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		LowDetailTreeVegetationCrownColorRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the crown color of the low detail tree vegetation.
*/
void LowDetailTreeVegetationCrownColorRenderPass::RenderInternal() NOEXCEPT
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

	//Bind the data render data tables.
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
		PushConstantData data;

		data._AlbedoTextureIndex = information._Material._CrownAlbedoTextureIndex;
		data._NormalMapTextureIndex = information._Material._CrownNormalMapTextureIndex;

		commandBuffer->PushConstants(this, ShaderStage::Fragment, 0, sizeof(PushConstantData), &data);

		for (const TreeVegetationPatchRenderInformation &renderInformation : information._PatchRenderInformations)
		{
			//Check whether or not this should be drawn.
			if (!TEST_BIT(renderInformation._Visibilities[UNDERLYING(TreeVegetationLevelOfDetail::Low)], VisibilityFlag::Perceiver)
				|| renderInformation._NumberOfTransformations[UNDERLYING(TreeVegetationLevelOfDetail::Low)] == 0)
			{
				continue;
			}

			//Bind the transformations buffer.
			commandBuffer->BindVertexBuffer(this, 1, renderInformation._TransformationsBuffer, &renderInformation._TransformationsOffsets[UNDERLYING(TreeVegetationLevelOfDetail::Low)]);

			//Draw the instances!
			commandBuffer->DrawIndexed(this, information._Model._CrownIndexCounts[UNDERLYING(LevelOfDetail::Low)], renderInformation._NumberOfTransformations[UNDERLYING(TreeVegetationLevelOfDetail::Low)]);
		}
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}