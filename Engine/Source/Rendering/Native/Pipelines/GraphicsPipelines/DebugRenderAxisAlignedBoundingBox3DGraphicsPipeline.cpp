#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/DebugRenderAxisAlignedBoundingBox3DGraphicsPipeline.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

/*
*	Debug render axis aligned bounding box 3D vertex push constant data definition.
*/
class DebugRenderAxisAlignedBoundingBox3DVertexPushConstantData final
{

public:

	//The minimum.
	Vector3<float32> _Minimum;

	//Padding.
	Padding<4> _Padding1;

	//The maximum.
	Vector3<float32> _Maximum;

	//Padding.
	Padding<4> _Padding2;

};

/*
*	Debug render axis aligned bounding box 3D fragment push constant data definition.
*/
class DebugRenderAxisAlignedBoundingBox3DFragmentPushConstantData final
{

public:

	//The color.
	Vector4<float32> _Color;

};

/*
*	Initializes this graphics pipeline.
*/
void DebugRenderAxisAlignedBoundingBox3DGraphicsPipeline::Initialize(const DepthBufferHandle depth_buffer, const bool depth_test, const bool wireframe) NOEXCEPT
{
	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Set whether or not to perform depth test.
	_DepthTest = depth_test;

	//Set whether or not to draw as wireframe.
	_Wireframe = wireframe;

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("AxisAlignedBoundingBox3DVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("DebugRenderFragmentShader")));

	//Set the depth buffer.
	if (_DepthTest)
	{
		SetDepthBuffer(depth_buffer);
	}

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(1);
	AddOutputRenderTarget(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::VERTEX, 0, sizeof(DebugRenderAxisAlignedBoundingBox3DVertexPushConstantData));
	AddPushConstantRange(ShaderStage::FRAGMENT, sizeof(DebugRenderAxisAlignedBoundingBox3DVertexPushConstantData), sizeof(DebugRenderAxisAlignedBoundingBox3DFragmentPushConstantData));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution(0));

	//Set the properties of the render pass.
	if (_DepthTest)
	{
		SetDepthStencilAttachmentLoadOperator(AttachmentLoadOperator::LOAD);
		SetDepthStencilAttachmentStoreOperator(AttachmentStoreOperator::STORE);
	}

	else
	{
		SetDepthStencilAttachmentLoadOperator(AttachmentLoadOperator::DONT_CARE);
		SetDepthStencilAttachmentStoreOperator(AttachmentStoreOperator::DONT_CARE);
	}
	
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::LOAD);
	SetColorAttachmentStoreOperator(AttachmentStoreOperator::STORE);
	SetBlendEnabled(true);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::None);

	if (_DepthTest)
	{
		SetDepthCompareOperator(CompareOperator::Greater);
		SetDepthTestEnabled(true);
		SetDepthWriteEnabled(true);
	}
	
	else
	{
		SetDepthCompareOperator(CompareOperator::Always);
		SetDepthTestEnabled(false);
		SetDepthWriteEnabled(false);
	}

	SetStencilTestEnabled(false);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Keep);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(0);

	if (_Wireframe)
	{
		SetTopology(Topology::LineList);
	}
	
	else
	{
		SetTopology(Topology::TriangleList);
	}

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName("Debug Render Axis Aligned Bounding Box 3D");
#endif
}

/*
*	Executes this graphics pipeline.
*/
void DebugRenderAxisAlignedBoundingBox3DGraphicsPipeline::Execute() NOEXCEPT
{
	//Lock the debug axis aligned bounding box 3D renders.
	SCOPED_LOCK(*RenderingSystem::Instance->GetDebugRenderingSystem()->GetDebugAxisAlignedBoundingBox3DRendersLock());

	//Cache the debug axis aligned bounding box 3D renders.
	const DynamicArray<DebugRenderingSystem::DebugAxisAlignedBoundingBox3DRender> &debug_axis_aligned_bounding_box_3D_renders{ RenderingSystem::Instance->GetDebugRenderingSystem()->GetDebugAxisAlignedBoundingBox3DRenders() };

	//Just return if there's none to draw.
	if (debug_axis_aligned_bounding_box_3D_renders.Empty())
	{
		SetIncludeInRender(false);

		return;
	}

	//Retrieve and set the command buffer.
	CommandBuffer *const RESTRICT command_buffer{ RenderingSystem::Instance->GetGlobalCommandBuffer(CommandBufferLevel::SECONDARY) };
	SetCommandBuffer(command_buffer);

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Render all debug sphere renders.
	for (const DebugRenderingSystem::DebugAxisAlignedBoundingBox3DRender &axis_aligned_bounding_box_3D_render : debug_axis_aligned_bounding_box_3D_renders)
	{
		//Skip this entry if it doesn't match the debug render state.
		if (_DepthTest != axis_aligned_bounding_box_3D_render._DebugRender._DepthTest
			|| _Wireframe != axis_aligned_bounding_box_3D_render._DebugRender._Wireframe)
		{
			continue;
		}
		
		//Push constants.
		{
			DebugRenderAxisAlignedBoundingBox3DVertexPushConstantData data;

			data._Minimum = axis_aligned_bounding_box_3D_render._AxisAlignedBoundingBox3D._Minimum;
			data._Maximum = axis_aligned_bounding_box_3D_render._AxisAlignedBoundingBox3D._Maximum;

			command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(DebugRenderAxisAlignedBoundingBox3DVertexPushConstantData), &data);
		}

		{
			DebugRenderAxisAlignedBoundingBox3DFragmentPushConstantData data;

			data._Color = axis_aligned_bounding_box_3D_render._DebugRender._Color;

			command_buffer->PushConstants(this, ShaderStage::FRAGMENT, sizeof(DebugRenderAxisAlignedBoundingBox3DVertexPushConstantData), sizeof(DebugRenderAxisAlignedBoundingBox3DFragmentPushConstantData), &data);
		}

		//Draw!
		command_buffer->Draw(this, 36, 1);
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}

/*
*	Terminates this graphics pipeline.
*/
void DebugRenderAxisAlignedBoundingBox3DGraphicsPipeline::Terminate() NOEXCEPT
{

}
#endif