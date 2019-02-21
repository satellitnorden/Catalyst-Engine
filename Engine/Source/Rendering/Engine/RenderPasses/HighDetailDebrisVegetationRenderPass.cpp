//Header file.
#include <Rendering/Engine/RenderPasses/HighDetailDebrisVegetationRenderPass.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>
#include <Rendering/Engine/PhysicalVertex.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/VegetationSystem.h>

//Singleton definition.
DEFINE_SINGLETON(HighDetailDebrisVegetationRenderPass);

/*
*	Push constant data definition.
*/
class PushConstantData final
{

public:

	int32 _AlbedoTextureIndex;
	int32 _NormalMapTextureIndex;
	int32 _MaterialPropertiesTextureIndex;

};

/*
*	Default constructor.
*/
HighDetailDebrisVegetationRenderPass::HighDetailDebrisVegetationRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		HighDetailDebrisVegetationRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the high detail debris vegetation render pass.
*/
void HighDetailDebrisVegetationRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::HighDetailDebrisVegetation);

	//Set the shaders.
	SetVertexShader(Shader::HighDetailDebrisVegetationVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::HighDetailDebrisVegetationFragment);

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
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Fragment, 0, sizeof(PushConstantData));

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
	SetStencilWriteMask(RenderingConstants::SCENE_BUFFER_STENCIL_BIT | RenderingConstants::SCENE_BUFFER_GENERAL_STENCIL_BIT);
	SetStencilReferenceMask(RenderingConstants::SCENE_BUFFER_STENCIL_BIT | RenderingConstants::SCENE_BUFFER_GENERAL_STENCIL_BIT);
	SetTopology(Topology::TriangleList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		HighDetailDebrisVegetationRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the high detail vegetation.
*/
void HighDetailDebrisVegetationRenderPass::RenderInternal() NOEXCEPT
{
	//Retrieve the debris vegetion type informations.
	const DynamicArray<DebrisVegetationTypeInformation> *const RESTRICT informations{ VegetationSystem::Instance->GetDebrisVegetationTypeInformations() };

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

	//Wait for the debris vegetation culling to finish.
	CullingSystem::Instance->WaitForDebrisVegetationCulling();

	for (const DebrisVegetationTypeInformation &information : *informations)
	{
		//Bind the model vertex and index buffer.
		const uint64 offset{ 0 };

		commandBuffer->BindVertexBuffer(this, 0, information._Model._Buffers[UNDERLYING(LevelOfDetail::High)], &offset);
		commandBuffer->BindIndexBuffer(this, information._Model._Buffers[UNDERLYING(LevelOfDetail::High)], information._Model._IndexOffsets[UNDERLYING(LevelOfDetail::High)]);

		//Push constants.
		PushConstantData data;

		data._AlbedoTextureIndex = information._Material._AlbedoTextureIndex;
		data._NormalMapTextureIndex = information._Material._NormalMapTextureIndex;
		data._MaterialPropertiesTextureIndex = information._Material._MaterialPropertiesTextureIndex;

		commandBuffer->PushConstants(this, ShaderStage::Fragment, 0, sizeof(PushConstantData), &data);

		for (const DebrisVegetationPatchRenderInformation &renderInformation : information._PatchRenderInformations)
		{
			//Check whether or not this should be drawn.
			if (!TEST_BIT(renderInformation._Visibilities[UNDERLYING(LevelOfDetail::High)], VisibilityFlag::Perceiver)
				|| renderInformation._NumberOfTransformations[UNDERLYING(LevelOfDetail::High)] == 0)
			{
				continue;
			}

			//Bind the transformations buffer.
			commandBuffer->BindVertexBuffer(this, 1, renderInformation._TransformationsBuffers[UNDERLYING(LevelOfDetail::High)], &offset);

			//Draw the instances!
			commandBuffer->DrawIndexed(this, information._Model._IndexCounts[UNDERLYING(LevelOfDetail::High)], renderInformation._NumberOfTransformations[UNDERLYING(LevelOfDetail::High)]);
		}
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}