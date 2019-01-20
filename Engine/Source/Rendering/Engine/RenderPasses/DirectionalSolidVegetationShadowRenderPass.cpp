//Header file.
#include <Rendering/Engine/RenderPasses/DirectionalSolidVegetationShadowRenderPass.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>
#include <Rendering/Engine/PhysicalVertex.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/VegetationSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DirectionalSolidVegetationShadowRenderPass);

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
DirectionalSolidVegetationShadowRenderPass::DirectionalSolidVegetationShadowRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		DirectionalSolidVegetationShadowRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the directional solid vegetation shadow render pass.
*/
void DirectionalSolidVegetationShadowRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::DirectionalShadowMapping);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::DirectionalSolidVegetationShadow);

	//Set the shaders.
	SetVertexShader(Shader::DirectionalSolidVegetationShadowVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::DirectionalSolidVegetationShadowFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::DirectionalLight);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::DirectionalShadowMap);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

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
	SetRenderResolution(RenderingSystem::Instance->GetDirectionalShadowMapResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::Back);
	SetDepthCompareOperator(CompareOperator::Less);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
	SetStencilTestEnabled(false);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Keep);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(0);
	SetTopology(Topology::TriangleList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		DirectionalSolidVegetationShadowRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the directional solid vegetation shadow.
*/
void DirectionalSolidVegetationShadowRenderPass::RenderInternal() NOEXCEPT
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

		//Pust constants.
		PushConstantData data;

		data._CutoffDistanceSquared = (information._Properties._CutoffDistance) * (information._Properties._CutoffDistance);
		data._HalfCutoffDistanceSquared = (information._Properties._CutoffDistance * 0.5f) * (information._Properties._CutoffDistance * 0.5f);
		data._InverseHalfCutoffDistanceSquared = 1.0f / data._HalfCutoffDistanceSquared;

		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(PushConstantData), &data);

		for (const SolidVegetationPatchRenderInformation &renderInformation : information._PatchRenderInformations)
		{
			//Check whether or not this should be drawn.
			if (!TEST_BIT(renderInformation._Visibilities[UNDERLYING(LevelOfDetail::High)], VisibilityFlag::Viewer)
				|| renderInformation._NumberOfTransformations[UNDERLYING(LevelOfDetail::High)] == 0)
			{
				continue;
			}

			//Bind the transformations buffer.
			commandBuffer->BindVertexBuffer(this, 0, information._Model._Buffers[UNDERLYING(LevelOfDetail::High)], &offset);
			commandBuffer->BindIndexBuffer(this, information._Model._Buffers[UNDERLYING(LevelOfDetail::High)], information._Model._IndexOffsets[UNDERLYING(LevelOfDetail::High)]);
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