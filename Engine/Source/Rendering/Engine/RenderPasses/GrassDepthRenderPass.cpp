//Header file.
#include <Rendering/Engine/RenderPasses/GrassDepthRenderPass.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/VegetationSystem.h>

//Vegetation.
#include <Vegetation/GrassVertex.h>

//Singleton definition.
DEFINE_SINGLETON(GrassDepthRenderPass);

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
GrassDepthRenderPass::GrassDepthRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		GrassDepthRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the grass depth render pass.
*/
void GrassDepthRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::GrassDepth);

	//Set the shaders.
	SetVertexShader(Shader::GrassVegetationDepthVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::GrassVegetationDepthFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::DynamicUniformData));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GrassMaterial));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(PushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(9);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(GrassVertex, _Position));
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(GrassVertex, _Normal));
	AddVertexInputAttributeDescription(	2,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(GrassVertex, _Tangent));
	AddVertexInputAttributeDescription(	3,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										offsetof(GrassVertex, _TextureCoordinate));
	AddVertexInputAttributeDescription(	4,
										0,
										VertexInputAttributeDescription::Format::X32SignedFloat,
										offsetof(GrassVertex, _ModulatorFactor));
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
	AddVertexInputBindingDescription(0, sizeof(GrassVertex), VertexInputBindingDescription::InputRate::Vertex);
	AddVertexInputBindingDescription(1, sizeof(Matrix4), VertexInputBindingDescription::InputRate::Instance);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(false);
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
		GrassDepthRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the depth of the grass.
*/
void GrassDepthRenderPass::RenderInternal() NOEXCEPT
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

	for (const GrassVegetationTypeInformation &information : *informations)
	{
		//Bind the model vertex and index buffer.
		const uint64 offset{ 0 };

		commandBuffer->BindVertexBuffer(this, 0, information._Model._Buffer, &offset);
		commandBuffer->BindIndexBuffer(this, information._Model._Buffer, information._Model._IndexOffset);

		//Bind the render data table.
		commandBuffer->BindRenderDataTable(this, 1, information._Material._RenderDataTable);

		//Pust constants.
		PushConstantData data;

		data._CutoffDistanceSquared = (information._Properties._CutoffDistance) * (information._Properties._CutoffDistance);
		data._HalfCutoffDistanceSquared = (information._Properties._CutoffDistance * 0.5f) * (information._Properties._CutoffDistance * 0.5f);
		data._InverseHalfCutoffDistanceSquared = 1.0f / data._HalfCutoffDistanceSquared;
		data._WindModulatorFactor = information._Properties._WindModulatorFactor;

		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(PushConstantData), &data);

		for (const VegetationPatchRenderInformation &renderInformation : information._PatchRenderInformations)
		{
			//Check whether or not this should be drawn.
			if (!renderInformation._Draw || renderInformation._NumberOfTransformations == 0)
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