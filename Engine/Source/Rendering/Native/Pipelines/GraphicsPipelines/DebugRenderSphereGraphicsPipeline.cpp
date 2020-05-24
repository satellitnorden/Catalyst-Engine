#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/DebugRenderSphereGraphicsPipeline.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

/*
*	Debug render sphere vertex push constant data definition.
*/
class DebugRenderSphereVertexPushConstantData final
{

public:

	//The position.
	Vector3<float32> _Position;

	//Padding.
	Padding<4> _Padding1;

	//The radius.
	float32 _Radius;

	//The number of segments.
	uint32 _NumberOfSegments;

	//Padding.
	Padding<8> _Padding2;

};

/*
*	Debug render sphere fragment push constant data definition.
*/
class DebugRenderSphereFragmentPushConstantData final
{

public:

	//The color.
	Vector4<float32> _Color;

};

/*
*	Initializes this graphics pipeline.
*/
void DebugRenderSphereGraphicsPipeline::Initialize() NOEXCEPT
{
	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("SphereVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("DebugRenderFragmentShader")));

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(1);
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(2);
	AddPushConstantRange(ShaderStage::VERTEX, 0, sizeof(DebugRenderSphereVertexPushConstantData));
	AddPushConstantRange(ShaderStage::FRAGMENT, sizeof(DebugRenderSphereVertexPushConstantData), sizeof(DebugRenderSphereFragmentPushConstantData));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution(0));

	//Set the properties of the render pass.
	SetShouldClear(false);
	SetBlendEnabled(true);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::None);
	SetDepthCompareOperator(CompareOperator::Always);
	SetDepthTestEnabled(false);
	SetDepthWriteEnabled(false);
	SetStencilTestEnabled(false);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Keep);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(0);
	SetTopology(Topology::TriangleFan);
}

/*
*	Executes this graphics pipeline.
*/
void DebugRenderSphereGraphicsPipeline::Execute() NOEXCEPT
{
	//Lock the debug sphere renders.
	SCOPED_LOCK(*RenderingSystem::Instance->GetDebugRenderingSystem()->GetDebugSphereRendersLock());

	//Cache the debug sphere renders.
	const DynamicArray<DebugRenderingSystem::DebugSphereRender> &debug_sphere_renders{ RenderingSystem::Instance->GetDebugRenderingSystem()->GetDebugSphereRenders() };

	//Just return if there's none to draw.
	if (debug_sphere_renders.Empty())
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
	for (const DebugRenderingSystem::DebugSphereRender& sphere : debug_sphere_renders)
	{
		//Push constants.
		{
			DebugRenderSphereVertexPushConstantData data;

			data._Position = sphere._Sphere._Position;
			data._Radius = sphere._Sphere._Radius;
			data._NumberOfSegments = sphere._NumberOfSegments;

			command_buffer->PushConstants(this, ShaderStage::VERTEX, 0, sizeof(DebugRenderSphereVertexPushConstantData), &data);
		}

		{
			DebugRenderSphereFragmentPushConstantData data;

			data._Color = sphere._DebugRender._Color;

			command_buffer->PushConstants(this, ShaderStage::FRAGMENT, sizeof(DebugRenderSphereVertexPushConstantData), sizeof(DebugRenderSphereFragmentPushConstantData), &data);
		}

		//Draw!
		command_buffer->Draw(this, sphere._NumberOfSegments * sphere._NumberOfSegments, 1);
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}
#endif