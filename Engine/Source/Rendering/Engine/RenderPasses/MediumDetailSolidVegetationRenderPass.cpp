//Header file.
#include <Rendering/Engine/RenderPasses/MediumDetailSolidVegetationRenderPass.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>
#include <Rendering/Engine/PhysicalVertex.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/VegetationSystem.h>

//Singleton definition.
DEFINE_SINGLETON(MediumDetailSolidVegetationRenderPass);

class PushConstantData final
{

public:

	float _CutoffDistanceSquared;
	float _HalfCutoffDistanceSquared;
	float _InverseHalfCutoffDistanceSquared;

};

/*
*	Default constructor.
*/
MediumDetailSolidVegetationRenderPass::MediumDetailSolidVegetationRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		MediumDetailSolidVegetationRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the medium detail solid vegetation render pass.
*/
void MediumDetailSolidVegetationRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::MediumDetailSolidVegetation);

	//Set the shaders.
	SetVertexShader(Shader::SolidVegetationVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::MediumDetailSolidVegetationFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(3);
	AddRenderTarget(RenderTarget::SceneBufferAlbedo);
	AddRenderTarget(RenderTarget::SceneBufferNormalDepth);
	AddRenderTarget(RenderTarget::SceneBufferMaterialProperties);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::DynamicUniformData));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::PhysicalMaterial));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(PushConstantData));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(8);
	AddVertexInputAttributeDescription(	0,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(PhysicalVertex, _Position));
	AddVertexInputAttributeDescription(	1,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(PhysicalVertex, _Normal));
	AddVertexInputAttributeDescription(	2,
										0,
										VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
										offsetof(PhysicalVertex, _Tangent));
	AddVertexInputAttributeDescription(	3,
										0,
										VertexInputAttributeDescription::Format::X32Y32SignedFloat,
										offsetof(PhysicalVertex, _TextureCoordinate));
	AddVertexInputAttributeDescription(	4,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										0);
	AddVertexInputAttributeDescription(	5,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(Vector4));
	AddVertexInputAttributeDescription(	6,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(Vector4) * 2);
	AddVertexInputAttributeDescription(	7,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(Vector4) * 3);

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(2);
	AddVertexInputBindingDescription(0, sizeof(PhysicalVertex), VertexInputBindingDescription::InputRate::Vertex);
	AddVertexInputBindingDescription(1, sizeof(Matrix4), VertexInputBindingDescription::InputRate::Instance);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(false);
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
	SetStencilWriteMask(BIT(0));
	SetStencilReferenceMask(BIT(0));
	SetTopology(Topology::TriangleList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		MediumDetailSolidVegetationRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the medium detail solid vegetation.
*/
void MediumDetailSolidVegetationRenderPass::RenderInternal() NOEXCEPT
{
	//Retrieve the solid vegetion type informations.
	const DynamicArray<SolidVegetationTypeInformation> *const RESTRICT informations{ VegetationSystem::Instance->GetSolidVegetationTypeInformations() };

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

	//Wait for the solid vegetation culling to finish.
	CullingSystem::Instance->WaitForSolidVegetationCulling();

	//Wait for the solid vegetation level of detail to finish.
	LevelOfDetailSystem::Instance->WaitForSolidVegetationLevelOfDetail();

	for (const SolidVegetationTypeInformation &information : *informations)
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

		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(PushConstantData), &data);

		for (const VegetationPatchRenderInformation &renderInformation : information._PatchRenderInformations)
		{
			//Check whether or not this should be drawn.
			if (!renderInformation._Draw
				|| renderInformation._NumberOfTransformations == 0
				|| renderInformation._LevelOfDetail != VegetationLevelOfDetail::Medium)
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