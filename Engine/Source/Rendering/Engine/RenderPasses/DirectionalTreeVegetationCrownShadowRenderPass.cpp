//Header file.
#include <Rendering/Engine/RenderPasses/DirectionalTreeVegetationCrownShadowRenderPass.h>

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
DEFINE_SINGLETON(DirectionalTreeVegetationCrownShadowRenderPass);

class PushConstantData final
{

public:

	int32 _MaskTextureIndex;

};

/*
*	Default constructor.
*/
DirectionalTreeVegetationCrownShadowRenderPass::DirectionalTreeVegetationCrownShadowRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		DirectionalTreeVegetationCrownShadowRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the directional vegetation crown shadow render pass.
*/
void DirectionalTreeVegetationCrownShadowRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::HighDetailTreeVegetationCrownDepth);

	//Set the shaders.
	SetVertexShader(Shader::DirectionalTreeVegetationCrownShadowVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::DirectionalTreeVegetationCrownShadowFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::DirectionalLight);

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
	SetRenderResolution(RenderingSystem::Instance->GetDirectionalShadowMapResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::None);
	SetDepthCompareOperator(CompareOperator::Less);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
	SetStencilTestEnabled(false);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Replace);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(0);
	SetTopology(Topology::TriangleList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		DirectionalTreeVegetationCrownShadowRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the crown shadows of the directional tree vegetation.
*/
void DirectionalTreeVegetationCrownShadowRenderPass::RenderInternal() NOEXCEPT
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

		commandBuffer->BindVertexBuffer(this, 0, information._Model._CrownBuffers[UNDERLYING(LevelOfDetail::High)], &offset);
		commandBuffer->BindIndexBuffer(this, information._Model._CrownBuffers[UNDERLYING(LevelOfDetail::High)], information._Model._CrownIndexOffsets[UNDERLYING(LevelOfDetail::High)]);

		//Push constants.
		PushConstantData data;

		data._MaskTextureIndex = information._Material._CrownMaskTextureIndex;

		commandBuffer->PushConstants(this, ShaderStage::Fragment, 0, sizeof(PushConstantData), &data);

		for (const TreeVegetationPatchRenderInformation &renderInformation : information._PatchRenderInformations)
		{
			//Check whether or not this should be drawn.
			if (!TEST_BIT(renderInformation._Visibilities[UNDERLYING(TreeVegetationLevelOfDetail::High)], VisibilityFlag::Perceiver)
				|| renderInformation._NumberOfTransformations[UNDERLYING(TreeVegetationLevelOfDetail::High)] == 0)
			{
				continue;
			}

			//Bind the transformations buffer.
			commandBuffer->BindVertexBuffer(this, 1, renderInformation._TransformationsBuffer, &renderInformation._TransformationsOffsets[UNDERLYING(TreeVegetationLevelOfDetail::High)]);

			//Draw the instances!
			commandBuffer->DrawIndexed(this, information._Model._CrownIndexCounts[UNDERLYING(LevelOfDetail::High)], renderInformation._NumberOfTransformations[UNDERLYING(TreeVegetationLevelOfDetail::High)]);
		}
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}