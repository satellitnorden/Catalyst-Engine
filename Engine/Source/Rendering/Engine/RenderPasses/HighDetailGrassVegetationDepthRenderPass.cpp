//Header file.
#include <Rendering/Engine/RenderPasses/HighDetailGrassVegetationDepthRenderPass.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/VegetationSystem.h>

//Vegetation.
#include <Vegetation/GrassVegetationVertex.h>

//Singleton definition.
DEFINE_SINGLETON(HighDetailGrassVegetationDepthRenderPass);

class PushConstantData final
{

public:

	float _CutoffDistanceSquared;
	float _HalfCutoffDistanceSquared;
	float _InverseHalfCutoffDistanceSquared;
	float _WindModulatorFactor;

};

/*
*	Default constructor.
*/
HighDetailGrassVegetationDepthRenderPass::HighDetailGrassVegetationDepthRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		HighDetailGrassVegetationDepthRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the high detail grass vegetation depth render pass.
*/
void HighDetailGrassVegetationDepthRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::HighDetailGrassVegetationDepth);

	//Set the shaders.
	SetVertexShader(Shader::HighDetailGrassVegetationDepthVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::HighDetailGrassVegetationDepthFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GrassMaterial));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(PushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(9);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(GrassVegetationVertex, _Position));
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(GrassVegetationVertex, _Normal));
	AddVertexInputAttributeDescription(	2,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(GrassVegetationVertex, _Tangent));
	AddVertexInputAttributeDescription(	3,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										offsetof(GrassVegetationVertex, _TextureCoordinate));
	AddVertexInputAttributeDescription(	4,
										0,
										VertexInputAttributeDescription::Format::X32SignedFloat,
										offsetof(GrassVegetationVertex, _ModulatorFactor));
	AddVertexInputAttributeDescription(	5,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										0);
	AddVertexInputAttributeDescription(	6,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(Vector4));
	AddVertexInputAttributeDescription(	7,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(Vector4) * 2);
	AddVertexInputAttributeDescription(	8,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(Vector4) * 3);

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(2);
	AddVertexInputBindingDescription(0, sizeof(GrassVegetationVertex), VertexInputBindingDescription::InputRate::Vertex);
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
		HighDetailGrassVegetationDepthRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the high detail depth of the grass vegetation.
*/
void HighDetailGrassVegetationDepthRenderPass::RenderInternal() NOEXCEPT
{
	//Retrieve the grass vegetation type informations.
	const DynamicArray<GrassVegetationTypeInformation> *const RESTRICT informations{ VegetationSystem::Instance->GetGrassVegetationTypeInformations() };

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

	//Bind the current dynamic uniform data render data table.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetCurrentDynamicUniformDataRenderDataTable());

	//Wait for the grass vegetation culling to finish.
	CullingSystem::Instance->WaitForGrassVegetationCulling();

	//Wait for the grass vegetation level of detail to finish.
	LevelOfDetailSystem::Instance->WaitForGrassVegetationLevelOfDetail();

	for (const GrassVegetationTypeInformation &information : *informations)
	{
		//Bind the model vertex and index buffer.
		const uint64 offset{ 0 };

		commandBuffer->BindVertexBuffer(this, 0, information._Model._Buffer, &offset);
		commandBuffer->BindIndexBuffer(this, information._Model._Buffer, information._Model._IndexOffset);

		//Bind the render data table.
		commandBuffer->BindRenderDataTable(this, 1, information._Material._RenderDataTable);

		//Push constants.
		PushConstantData data;

		data._CutoffDistanceSquared = (information._Properties._CutoffDistance) * (information._Properties._CutoffDistance);
		data._HalfCutoffDistanceSquared = (information._Properties._CutoffDistance * 0.5f) * (information._Properties._CutoffDistance * 0.5f);
		data._InverseHalfCutoffDistanceSquared = 1.0f / data._HalfCutoffDistanceSquared;
		data._WindModulatorFactor = information._Properties._WindModulatorFactor;

		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(PushConstantData), &data);

		for (const VegetationPatchRenderInformation &renderInformation : information._PatchRenderInformations)
		{
			//Check whether or not this should be drawn.
			if (!TEST_BIT(renderInformation._Visibility, VisibilityFlag::Viewer)
				|| renderInformation._NumberOfTransformations == 0
				|| renderInformation._LevelOfDetail != VegetationLevelOfDetail::High)
			{
				continue;
			}

			//Bind the transformations buffer.
			commandBuffer->BindVertexBuffer(this, 1, renderInformation._TransformationsBuffer, &offset);

			//Draw the instances!
			commandBuffer->DrawIndexed(this, information._Model._IndexCount, renderInformation._NumberOfTransformations);
		}
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}