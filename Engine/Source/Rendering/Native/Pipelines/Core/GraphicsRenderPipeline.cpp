//Header file.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

/*
*	Initializes this graphics pipeline.
*/
void GraphicsRenderPipeline::Initialize() NOEXCEPT
{
	//Retrieve the render pipeline resource.
	const ResourcePointer<RenderPipelineResource> render_pipeline_resource{ ResourceSystem::Instance->GetRenderPipelineResource(_RenderPipelineIdentifier) };

	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Set if this render pipeline needs a render data table.
	_UsesRenderDataTable = !render_pipeline_resource->_InputRenderTargets.Empty();

	//If this render pipeline has input render targets, create a render data table (and its layout).
	if (_UsesRenderDataTable)
	{
		//Create the render data table layout.
		{
			constexpr uint64 MAX_BINDINGS{ 1 };

			ASSERT(render_pipeline_resource->_InputRenderTargets.Size() <= MAX_BINDINGS, "Probably need to increase MAX_BINDINGS.");

			StaticArray<RenderDataTableLayoutBinding, MAX_BINDINGS> bindings;

			for (uint64 i{ 0 }; i < render_pipeline_resource->_InputRenderTargets.Size(); ++i)
			{
				bindings[i] = RenderDataTableLayoutBinding(static_cast<uint32>(i), RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::FRAGMENT);
			}

			RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(render_pipeline_resource->_InputRenderTargets.Size()), &_RenderDataTableLayout);
		}

		//Create the render data table.
		{
			RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

			for (uint64 i{ 0 }; i < render_pipeline_resource->_InputRenderTargets.Size(); ++i)
			{
				RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable
				(
					0,
					0,
					&_RenderDataTable,
					RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(render_pipeline_resource->_InputRenderTargets[i]),
					RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge)
				);
			}
		}
	}

	//Set the shaders.
	SetVertexShader(render_pipeline_resource->_VertexShaderHandle);
	SetFragmentShader(render_pipeline_resource->_FragmentShaderHandle);

	//Set the depth buffer.
	if (render_pipeline_resource->_OutputDepthBuffer)
	{
		SetDepthBuffer(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(render_pipeline_resource->_OutputDepthBuffer));
	}

	//Add the output render targets.
	if (render_pipeline_resource->_OutputRenderTargets.Size() == 1 && render_pipeline_resource->_OutputRenderTargets[0] == HashString("Screen"))
	{
		SetIsRenderingDirectlyToScreen(true);
	}
	
	else
	{
		SetNumberOfOutputRenderTargets(render_pipeline_resource->_OutputRenderTargets.Size());

		for (const HashString output_render_target : render_pipeline_resource->_OutputRenderTargets)
		{
			AddOutputRenderTarget(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(output_render_target));
		}
	}

	//Add the render data table layouts.
	if (_UsesRenderDataTable)
	{
		SetNumberOfRenderDataTableLayouts(1);
		AddRenderDataTableLayout(_RenderDataTableLayout);
	}
	
	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetFullResolution());

	//Set the properties of the render pass.
	SetDepthStencilAttachmentLoadOperator(render_pipeline_resource->_DepthStencilLoadOperator);
	SetDepthStencilAttachmentStoreOperator(render_pipeline_resource->_DepthStencilStoreOperator);
	SetColorAttachmentLoadOperator(render_pipeline_resource->_ColorLoadOperator);
	SetColorAttachmentStoreOperator(render_pipeline_resource->_ColorStoreOperator);
	SetBlendEnabled(render_pipeline_resource->_BlendEnabled);
	SetBlendFactorSourceColor(render_pipeline_resource->_BlendColorSourceFactor);
	SetBlendFactorDestinationColor(render_pipeline_resource->_BlendColorDestinationFactor);
	SetColorBlendOperator(render_pipeline_resource->_BlendColorOperator);
	SetBlendFactorSourceAlpha(render_pipeline_resource->_BlendAlphaSourceFactor);
	SetBlendFactorDestinationAlpha(render_pipeline_resource->_BlendAlphaDestinationFactor);
	SetAlphaBlendOperator(render_pipeline_resource->_BlendAlphaOperator);
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

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName(render_pipeline_resource->_Header._ResourceName.Data());
#endif
}

/*
*	Executes this graphics pipeline.
*/
void GraphicsRenderPipeline::Execute() NOEXCEPT
{
	//Retrieve and set the command buffer.
	CommandBuffer *const RESTRICT command_buffer{ RenderingSystem::Instance->GetGlobalCommandBuffer(CommandBufferLevel::SECONDARY) };
	SetCommandBuffer(command_buffer);

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	if (_UsesRenderDataTable)
	{
		command_buffer->BindRenderDataTable(this, 0, _RenderDataTable);
	}

	//Draw!
	command_buffer->Draw(this, 3, 1);

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}

/*
*	Terminates this graphics pipeline.
*/
void GraphicsRenderPipeline::Terminate() NOEXCEPT
{
	if (_UsesRenderDataTable)
	{
		//Destroy the render data table.
		RenderingSystem::Instance->DestroyRenderDataTable(&_RenderDataTable);

		//Destroy the render data table layout.
		RenderingSystem::Instance->DestroyRenderDataTableLayout(&_RenderDataTableLayout);
	}
}
