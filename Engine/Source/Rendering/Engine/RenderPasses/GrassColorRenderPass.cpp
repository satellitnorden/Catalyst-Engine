//Header file.
#include <Rendering/Engine/RenderPasses/GrassColorRenderPass.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/VegetationSystem.h>

//Vegetation.
#include <Vegetation/GrassVertex.h>

//Singleton definition.
DEFINE_SINGLETON(GrassColorRenderPass);

class VertexPushConstantData final
{

public:

	float _WindModulatorFactor;

};

class FragmentPushConstantData final
{

public:

	float _Thickness;

};

/*
*	Default constructor.
*/
GrassColorRenderPass::GrassColorRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		GrassColorRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the grass color render pass.
*/
void GrassColorRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::GrassColor);

	//Set the shaders.
	SetVertexShader(Shader::GrassVegetationColorVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::GrassVegetationColorFragment);

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
	SetDepthCompareOperator(CompareOperator::Equal);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(false);
	SetStencilTestEnabled(true);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Keep);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Equal);
	SetStencilCompareMask(BIT(3));
	SetStencilWriteMask(0);
	SetStencilReferenceMask(BIT(3));
	SetTopology(Topology::TriangleList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		GrassColorRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the color of the grass.
*/
void GrassColorRenderPass::RenderInternal() NOEXCEPT
{
	//Retrieve the grass vegetion type informations.
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
		VertexPushConstantData vertexData;
		vertexData._WindModulatorFactor = information._Properties._WindModulatorFactor;
		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(VertexPushConstantData), &vertexData);

		FragmentPushConstantData fragmentData;
		fragmentData._Thickness = information._Properties._Thickness;
		commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(VertexPushConstantData), sizeof(FragmentPushConstantData), &fragmentData);

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