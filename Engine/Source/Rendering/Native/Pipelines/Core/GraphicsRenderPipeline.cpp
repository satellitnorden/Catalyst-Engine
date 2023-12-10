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
	_UsesRenderDataTable = !render_pipeline_resource->_IncludedUniformBuffers.Empty() || !render_pipeline_resource->_InputRenderTargets.Empty();

	//If this render pipeline has input render targets, create a render data table (and its layout).
	if (_UsesRenderDataTable)
	{
		//Create the render data table layout.
		{
			constexpr uint64 MAX_BINDINGS{ 2 };

			StaticArray<RenderDataTableLayoutBinding, MAX_BINDINGS> bindings;
			uint32 current_binding_index{ 0 };

			for (const HashString included_uniform_buffer : render_pipeline_resource->_IncludedUniformBuffers)
			{
				bindings[current_binding_index] = RenderDataTableLayoutBinding(current_binding_index, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::VERTEX | ShaderStage::FRAGMENT);
			
				++current_binding_index;

				ASSERT(current_binding_index <= MAX_BINDINGS, "Probably need to increase MAX_BINDINGS");
			}

			for (const HashString input_render_target : render_pipeline_resource->_InputRenderTargets)
			{
				bindings[current_binding_index] = RenderDataTableLayoutBinding(current_binding_index, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::VERTEX | ShaderStage::FRAGMENT);
			
				++current_binding_index;

				ASSERT(current_binding_index <= MAX_BINDINGS, "Probably need to increase MAX_BINDINGS");
			}

			RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), current_binding_index, &_RenderDataTableLayout);
		}

		//Create the render data tables.
		_RenderDataTables.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

		for (RenderDataTableHandle &render_data_table : _RenderDataTables)
		{
			RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &render_data_table);

			uint32 current_binding_index{ 0 };

			{
				uint8 framebuffer_index{ 0 };

				for (const HashString included_uniform_buffer : render_pipeline_resource->_IncludedUniformBuffers)
				{
					RenderingSystem::Instance->BindUniformBufferToRenderDataTable
					(
						current_binding_index++,
						0,
						&render_data_table,
						RenderingSystem::Instance->GetUniformBufferManager()->GetUniformBuffer(included_uniform_buffer, framebuffer_index++)
					);
				}
			}

			for (const HashString input_render_target : render_pipeline_resource->_InputRenderTargets)
			{
				RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable
				(
					current_binding_index++,
					0,
					&render_data_table,
					RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(input_render_target),
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
	SetDepthTestEnabled(render_pipeline_resource->_DepthTestEnabled);
	SetDepthWriteEnabled(render_pipeline_resource->_DepthWriteEnabled);
	SetDepthCompareOperator(render_pipeline_resource->_DepthCompareOperator);
	SetStencilTestEnabled(render_pipeline_resource->_StencilTestEnabled);
	SetStencilFailOperator(render_pipeline_resource->_StencilFailOperator);
	SetStencilPassOperator(render_pipeline_resource->_StencilPassOperator);
	SetStencilDepthFailOperator(render_pipeline_resource->_StencilDepthFailOperator);
	SetStencilCompareOperator(render_pipeline_resource->_StencilCompareOperator);
	SetStencilCompareMask(render_pipeline_resource->_StencilCompareMask);
	SetStencilWriteMask(render_pipeline_resource->_StencilWriteMask);
	SetStencilReferenceMask(render_pipeline_resource->_StencilReferenceMask);
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

	//Bind the render data table.
	if (_UsesRenderDataTable)
	{
		RenderDataTableHandle &current_render_data_table{ _RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

		command_buffer->BindRenderDataTable(this, 0, current_render_data_table);
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
		for (RenderDataTableHandle &render_data_table : _RenderDataTables)
		{
			RenderingSystem::Instance->DestroyRenderDataTable(&render_data_table);
		}

		//Destroy the render data table layout.
		RenderingSystem::Instance->DestroyRenderDataTableLayout(&_RenderDataTableLayout);
	}
}
