#if !defined(CATALYST_FINAL)
//Header file.
#include <Rendering/Engine/RenderPasses/DebugAxisAlignedBoundingBoxRenderPass.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>
#include <Rendering/Engine/PhysicalVertex.h>

//Systems.
#include <Systems/DebugRenderingSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DebugAxisAlignedBoundingBoxRenderPass);

/*
*	Default constructor.
*/
DebugAxisAlignedBoundingBoxRenderPass::DebugAxisAlignedBoundingBoxRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		DebugAxisAlignedBoundingBoxRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the debug axis-aligned bounding box render pass.
*/
void DebugAxisAlignedBoundingBoxRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::Debug);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::DebugAxisAlignedBoundingBox);

	//Set the shaders.
	SetVertexShader(Shader::DebugAxisAlignedBoundingBoxVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::DebugAxisAlignedBoundingBoxFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::Scene);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::DynamicUniformData));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::Vertex, 0, sizeof(Matrix4));
	AddPushConstantRange(ShaderStage::Fragment, sizeof(Matrix4), sizeof(Vector4));

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(4);
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

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(1);
	AddVertexInputBindingDescription(0, sizeof(PhysicalVertex), VertexInputBindingDescription::InputRate::Vertex);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(true);
	SetCullMode(CullMode::None);
	SetDepthCompareOperator(CompareOperator::Less);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(false);
	SetTopology(Topology::TriangleList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		DebugAxisAlignedBoundingBoxRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the debug axis-aligned bounding boxes.
*/
void DebugAxisAlignedBoundingBoxRenderPass::RenderInternal() NOEXCEPT
{
	//Iterate over all axis-aligned bounding box debug render data and render it all.
	const DynamicArray<DebugRenderingSystem::AxisAlignedBoundingBoxDebugRenderData> *const RESTRICT data{ DebugRenderingSystem::Instance->GetAxisAlignedBoundingBoxDebugRenderData() };

	//If there's none to render - render none.
	if (data->Empty())
	{
		//Don't include this render pass in the final render.
		SetIncludeInRender(false);

		return;
	}

	//Cache the command buffer.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the commandbuffer.
	commandBuffer->Begin(this);

	//Bind the render data table.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetCurrentDynamicUniformDataRenderDataTable());

	//Bind the cube model.
	const PhysicalModel cubeModel{ RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Cube) };
	constexpr uint64 offset{ 0 };
	commandBuffer->BindVertexBuffers(this, 1, &cubeModel._Buffer, &offset);
	commandBuffer->BindIndexBuffer(this, cubeModel._Buffer, cubeModel._IndexOffset);

	for (const DebugRenderingSystem::AxisAlignedBoundingBoxDebugRenderData &renderData : *data)
	{
		//Calculate the model matrix.
		const Vector3 position{ AxisAlignedBoundingBox::CalculateCenter(renderData._Box) };
		const Vector3 scale{ renderData._Box._Maximum._X - renderData._Box._Minimum._X, renderData._Box._Maximum._Y - renderData._Box._Minimum._Y, renderData._Box._Maximum._Z - renderData._Box._Minimum._Z };

		const Matrix4 modelMatrix{ position, Vector3(0.0f, 0.0f, 0.0f), scale };

		commandBuffer->PushConstants(this, ShaderStage::Vertex, 0, sizeof(Matrix4), &modelMatrix);
		commandBuffer->PushConstants(this, ShaderStage::Fragment, sizeof(Matrix4), sizeof(Vector4), &renderData._Color);

		commandBuffer->DrawIndexed(this, cubeModel._IndexCount, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}
#endif