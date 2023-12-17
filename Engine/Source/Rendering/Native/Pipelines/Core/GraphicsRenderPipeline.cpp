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
void GraphicsRenderPipeline::Initialize(const GraphicsRenderPipelineParameters &parameters) NOEXCEPT
{
	//Retrieve the render pipeline resource.
	_RenderPipelineResource = ResourceSystem::Instance->GetRenderPipelineResource(_RenderPipelineIdentifier);

	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Set if this render pipeline needs a render data table.
	_UsesRenderDataTable =	!_RenderPipelineResource->_IncludedUniformBuffers.Empty()
							|| !_RenderPipelineResource->_IncludedStorageBuffers.Empty()
							|| !_RenderPipelineResource->_SamplerProperties.Empty()
							|| !_RenderPipelineResource->_InputRenderTargets.Empty();

	//If this render pipeline has input render targets, create a render data table (and its layout).
	if (_UsesRenderDataTable)
	{
		//Create the render data table layout.
		{
			constexpr uint64 MAX_BINDINGS{ 6 };

			StaticArray<RenderDataTableLayoutBinding, MAX_BINDINGS> bindings;
			uint32 current_binding_index{ 0 };

			for (const HashString included_uniform_buffer : _RenderPipelineResource->_IncludedUniformBuffers)
			{
				bindings[current_binding_index] = RenderDataTableLayoutBinding(current_binding_index, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::VERTEX | ShaderStage::FRAGMENT);
			
				++current_binding_index;

				ASSERT(current_binding_index <= MAX_BINDINGS, "Probably need to increase MAX_BINDINGS");
			}

			for (const HashString included_storage_buffer : _RenderPipelineResource->_IncludedStorageBuffers)
			{
				bindings[current_binding_index] = RenderDataTableLayoutBinding(current_binding_index, RenderDataTableLayoutBinding::Type::StorageBuffer, 1, ShaderStage::VERTEX | ShaderStage::FRAGMENT);

				++current_binding_index;

				ASSERT(current_binding_index <= MAX_BINDINGS, "Probably need to increase MAX_BINDINGS");
			}

			for (const SamplerProperties &sampler_properties : _RenderPipelineResource->_SamplerProperties)
			{
				bindings[current_binding_index] = RenderDataTableLayoutBinding(current_binding_index, RenderDataTableLayoutBinding::Type::Sampler, 1, ShaderStage::VERTEX | ShaderStage::FRAGMENT);

				++current_binding_index;

				ASSERT(current_binding_index <= MAX_BINDINGS, "Probably need to increase MAX_BINDINGS");
			}

			for (const HashString input_render_target : _RenderPipelineResource->_InputRenderTargets)
			{
				bindings[current_binding_index] = RenderDataTableLayoutBinding(current_binding_index, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::VERTEX | ShaderStage::FRAGMENT);
			
				++current_binding_index;

				ASSERT(current_binding_index <= MAX_BINDINGS, "Probably need to increase MAX_BINDINGS");
			}

			RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), current_binding_index, &_RenderDataTableLayout);
		}

		//Create the render data tables.
		_RenderDataTables.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

		for (uint64 i{ 0 }; i < _RenderDataTables.Size(); ++i)
		{
			RenderDataTableHandle &render_data_table{ _RenderDataTables[i] };

			RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &render_data_table);

			uint32 current_binding_index{ 0 };

			for (const HashString included_uniform_buffer : _RenderPipelineResource->_IncludedUniformBuffers)
			{
				RenderingSystem::Instance->BindUniformBufferToRenderDataTable
				(
					current_binding_index++,
					0,
					&render_data_table,
					RenderingSystem::Instance->GetBufferManager()->GetUniformBuffer(included_uniform_buffer, static_cast<uint8>(i))
				);
			}

			for (const HashString included_storage_buffer : _RenderPipelineResource->_IncludedStorageBuffers)
			{
				RenderingSystem::Instance->BindStorageBufferToRenderDataTable
				(
					current_binding_index++,
					0,
					&render_data_table,
					RenderingSystem::Instance->GetBufferManager()->GetStorageBuffer(included_storage_buffer, static_cast<uint8>(i))
				);
			}

			for (const SamplerProperties &sampler_properties : _RenderPipelineResource->_SamplerProperties)
			{
				RenderingSystem::Instance->BindSamplerToRenderDataTable
				(
					current_binding_index++,
					0,
					&render_data_table,
					RenderingSystem::Instance->GetSampler(sampler_properties)
				);
			}

			for (const HashString input_render_target : _RenderPipelineResource->_InputRenderTargets)
			{
				RenderTargetHandle render_target;

				//First check if we received an input render target in the parameters.
				bool found{ false };

				for (const Pair<HashString, RenderTargetHandle>& _input_render_target : parameters._InputRenderTargets)
				{
					if (_input_render_target._First == input_render_target)
					{
						render_target = _input_render_target._Second;
						found = true;

						break;
					}
				}

				//Otherwise, try the shared render target manager.
				if (!found)
				{
					render_target = RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(input_render_target);
				}

				RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable
				(
					current_binding_index++,
					0,
					&render_data_table,
					render_target,
					RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge)
				);
			}
		}
	}

	//Set the shaders.
	SetVertexShader(_RenderPipelineResource->_VertexShaderHandle);
	SetFragmentShader(_RenderPipelineResource->_FragmentShaderHandle);

	//Set the depth buffer.
	if (_RenderPipelineResource->_OutputDepthBuffer)
	{
		SetDepthBuffer(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(_RenderPipelineResource->_OutputDepthBuffer));
	}

	//Add the output render targets.
	if (_RenderPipelineResource->_OutputRenderTargets.Size() == 1 && _RenderPipelineResource->_OutputRenderTargets[0] == HashString("Screen"))
	{
		SetIsRenderingDirectlyToScreen(true);
	}
	
	else
	{
		SetNumberOfOutputRenderTargets(_RenderPipelineResource->_OutputRenderTargets.Size());

		for (const HashString output_render_target : _RenderPipelineResource->_OutputRenderTargets)
		{
			//First check if we received an output render target in the parameters.
			bool found{ false };

			for (const Pair<HashString, RenderTargetHandle> &_output_render_target : parameters._OutputRenderTargets)
			{
				if (_output_render_target._First == output_render_target)
				{
					AddOutputRenderTarget(_output_render_target._Second);
					found = true;

					break;
				}
			}
			
			//Otherwise, try the shared render target manager.
			if (!found)
			{
				AddOutputRenderTarget(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(output_render_target));
			}
		}
	}

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(_UsesRenderDataTable ? 2 : 1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL_2));

	if (_UsesRenderDataTable)
	{
		AddRenderDataTableLayout(_RenderDataTableLayout);
	}
	
	//Set the render resolution.
	if (_RenderPipelineResource->_RenderResolution == HashString("MAIN_FULL"))
	{
		SetRenderResolution(RenderingSystem::Instance->GetScaledResolution(0));
	}

	else if (_RenderPipelineResource->_RenderResolution == HashString("MAIN_HALF"))
	{
		SetRenderResolution(RenderingSystem::Instance->GetScaledResolution(1));
	}
	
	else
	{
		ASSERT(false, "Unknown render resolution!");
	}

	//Retrieve the input stream.
	ASSERT(!_RenderPipelineResource->_InputStreamSubscriptions.Empty(), "Need at least one input stream subscription!");
	const RenderInputStream &input_stream{ RenderingSystem::Instance->GetRenderInputManager()->GetInputStream(_RenderPipelineResource->_InputStreamSubscriptions[0]) };

	//Assume that all input streams match in required vertex input attribute/binding descriptions.
	if (!input_stream._RequiredVertexInputAttributeDescriptions.Empty())
	{
		SetNumberOfVertexInputAttributeDescriptions(input_stream._RequiredVertexInputAttributeDescriptions.Size());

		for (const VertexInputAttributeDescription &item : input_stream._RequiredVertexInputAttributeDescriptions)
		{
			AddVertexInputAttributeDescription(item._Location, item._Binding, item._Format, item._Offset);
		}
	}

	if (!input_stream._RequiredVertexInputBindingDescriptions.Empty())
	{
		SetNumberOfVertexInputBindingDescriptions(input_stream._RequiredVertexInputBindingDescriptions.Size());

		for (const VertexInputBindingDescription &item : input_stream._RequiredVertexInputBindingDescriptions)
		{
			AddVertexInputBindingDescription(item._Binding, item._Stride, item._InputRate);
		}
	}

	//Set the push constant ranges.
	if (input_stream._RequiredPushConstantDataSize > 0)
	{
		SetNumberOfPushConstantRanges(1);
		AddPushConstantRange(ShaderStage::VERTEX | ShaderStage::FRAGMENT, 0, static_cast<uint32>(input_stream._RequiredPushConstantDataSize));
	}

	//Set the properties of the render pass.
	SetDepthStencilAttachmentLoadOperator(_RenderPipelineResource->_DepthStencilLoadOperator);
	SetDepthStencilAttachmentStoreOperator(_RenderPipelineResource->_DepthStencilStoreOperator);
	SetColorAttachmentLoadOperator(_RenderPipelineResource->_ColorLoadOperator);
	SetColorAttachmentStoreOperator(_RenderPipelineResource->_ColorStoreOperator);
	SetBlendEnabled(_RenderPipelineResource->_BlendEnabled);
	SetBlendFactorSourceColor(_RenderPipelineResource->_BlendColorSourceFactor);
	SetBlendFactorDestinationColor(_RenderPipelineResource->_BlendColorDestinationFactor);
	SetColorBlendOperator(_RenderPipelineResource->_BlendColorOperator);
	SetBlendFactorSourceAlpha(_RenderPipelineResource->_BlendAlphaSourceFactor);
	SetBlendFactorDestinationAlpha(_RenderPipelineResource->_BlendAlphaDestinationFactor);
	SetAlphaBlendOperator(_RenderPipelineResource->_BlendAlphaOperator);
	SetCullMode(_RenderPipelineResource->_CullMode);
	SetDepthTestEnabled(_RenderPipelineResource->_DepthTestEnabled);
	SetDepthWriteEnabled(_RenderPipelineResource->_DepthWriteEnabled);
	SetDepthCompareOperator(_RenderPipelineResource->_DepthCompareOperator);
	SetStencilTestEnabled(_RenderPipelineResource->_StencilTestEnabled);
	SetStencilFailOperator(_RenderPipelineResource->_StencilFailOperator);
	SetStencilPassOperator(_RenderPipelineResource->_StencilPassOperator);
	SetStencilDepthFailOperator(_RenderPipelineResource->_StencilDepthFailOperator);
	SetStencilCompareOperator(_RenderPipelineResource->_StencilCompareOperator);
	SetStencilCompareMask(_RenderPipelineResource->_StencilCompareMask);
	SetStencilWriteMask(_RenderPipelineResource->_StencilWriteMask);
	SetStencilReferenceMask(_RenderPipelineResource->_StencilReferenceMask);
	SetTopology(_RenderPipelineResource->_Topology);

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName(_RenderPipelineResource->_Header._ResourceName.Data());
#endif
}

/*
*	Executes this graphics pipeline.
*/
void GraphicsRenderPipeline::Execute() NOEXCEPT
{
	//Check if this graphics pipeline should be included in the render.
	{
		bool all_input_streams_empty{ true };

		for (const HashString input_stream_subscription : _RenderPipelineResource->_InputStreamSubscriptions)
		{
			const RenderInputStream &input_stream{ RenderingSystem::Instance->GetRenderInputManager()->GetInputStream(input_stream_subscription) };

			all_input_streams_empty &= input_stream._Entries.Empty();
		}

		if (all_input_streams_empty)
		{
			SetIncludeInRender(false);

			return;
		}
	}

	//Retrieve and set the command buffer.
	CommandBuffer *const RESTRICT command_buffer{ RenderingSystem::Instance->GetGlobalCommandBuffer(CommandBufferLevel::SECONDARY) };
	SetCommandBuffer(command_buffer);

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data table.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable2());

	if (_UsesRenderDataTable)
	{
		RenderDataTableHandle &current_render_data_table{ _RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

		command_buffer->BindRenderDataTable(this, 1, current_render_data_table);
	}

	//Go over the input streams.
	for (const HashString input_stream_subscription : _RenderPipelineResource->_InputStreamSubscriptions)
	{
		const RenderInputStream &input_stream{ RenderingSystem::Instance->GetRenderInputManager()->GetInputStream(input_stream_subscription) };

		ProcessInputStream(input_stream, command_buffer);
	}
	
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
