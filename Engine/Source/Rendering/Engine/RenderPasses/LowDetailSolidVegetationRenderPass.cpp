//Header file.
#include <Rendering/Engine/RenderPasses/LowDetailSolidVegetationRenderPass.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>
#include <Rendering/Engine/PhysicalVertex.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/VegetationSystem.h>

//Singleton definition.
DEFINE_SINGLETON(LowDetailSolidVegetationRenderPass);

/*
*	Vertex push constant data definition.
*/
class VertexPushConstantData final
{

public:

	float _CutoffDistanceSquared;
	float _HalfCutoffDistanceSquared;
	float _InverseHalfCutoffDistanceSquared;

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

};

/*
*	Default constructor.
*/
LowDetailSolidVegetationRenderPass::LowDetailSolidVegetationRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		LowDetailSolidVegetationRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the low detail solid vegetation render pass.
*/
void LowDetailSolidVegetationRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::LowDetailSolidVegetation);

	//Set the shaders.
	SetVertexShader(Shader::LowDetailSolidVegetationVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::LowDetailSolidVegetationFragment);

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
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(VertexPushConstantData));
	AddPushConstantRange(ShaderStage::Fragment, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData));

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
										sizeof(Vector4<float>));
	AddVertexInputAttributeDescription(	6,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(Vector4<float>) * 2);
	AddVertexInputAttributeDescription(	7,
										1,
										VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
										sizeof(Vector4<float>) * 3);

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(2);
	AddVertexInputBindingDescription(0, sizeof(PhysicalVertex), VertexInputBindingDescription::InputRate::Vertex);
	AddVertexInputBindingDescription(1, sizeof(Matrix4), VertexInputBindingDescription::InputRate::Instance);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
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
	SetStencilWriteMask(BIT(0));
	SetStencilReferenceMask(BIT(0));
	SetTopology(Topology::TriangleList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		LowDetailSolidVegetationRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the low detail solid vegetation.
*/
void LowDetailSolidVegetationRenderPass::RenderInternal() NOEXCEPT
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

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Wait for the solid vegetation culling to finish.
	CullingSystem::Instance->WaitForSolidVegetationCulling();

	for (const SolidVegetationTypeInformation &information : *informations)
	{
		//Bind the model vertex and index buffer.
		const uint64 offset{ 0 };

		commandBuffer->BindVertexBuffer(this, 0, information._Model._Buffer, &offset);
		commandBuffer->BindIndexBuffer(this, information._Model._Buffer, information._Model._IndexOffset);

		//Push constants.
		VertexPushConstantData vertexData;

		vertexData._CutoffDistanceSquared = (information._Properties._CutoffDistance) * (information._Properties._CutoffDistance);
		vertexData._HalfCutoffDistanceSquared = (information._Properties._CutoffDistance * 0.5f) * (information._Properties._CutoffDistance * 0.5f);
		vertexData._InverseHalfCutoffDistanceSquared = 1.0f / vertexData._HalfCutoffDistanceSquared;

		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(VertexPushConstantData), &vertexData);

		FragmentPushConstantData fragmentData;

		fragmentData._AlbedoTextureIndex = information._Material._AlbedoTextureIndex;
		fragmentData._NormalMapTextureIndex = information._Material._NormalMapTextureIndex;
		fragmentData._MaterialPropertiesTextureIndex = information._Material._MaterialPropertiesTextureIndex;

		commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData), &fragmentData);

		for (const SolidVegetationPatchRenderInformation &renderInformation : information._PatchRenderInformations)
		{
			//Check whether or not this should be drawn.
			if (!TEST_BIT(renderInformation._Visibilities[UNDERLYING(VegetationLevelOfDetail::Low)], VisibilityFlag::Viewer)
				|| renderInformation._NumberOfTransformations[UNDERLYING(VegetationLevelOfDetail::Low)] == 0)
			{
				continue;
			}

			//Bind the transformations buffer.
			commandBuffer->BindVertexBuffer(this, 1, renderInformation._TransformationsBuffers[UNDERLYING(VegetationLevelOfDetail::Low)], &offset);

			//Draw the instances!
			commandBuffer->DrawIndexed(this, information._Model._IndexCount, renderInformation._NumberOfTransformations[UNDERLYING(VegetationLevelOfDetail::Low)]);
		}
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}