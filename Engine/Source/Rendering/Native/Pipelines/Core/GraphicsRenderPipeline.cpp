//Header file.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/Utilities/RenderPipelineUtilities.h>
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/ContentSystem.h>
#include <Systems/RenderingSystem.h>

/*
*	Initializes this graphics pipeline.
*/
void GraphicsRenderPipeline::Initialize(const GraphicsRenderPipelineInitializeParameters &parameters) NOEXCEPT
{
	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Retrieve the render pipeline ASSET.
	_RenderPipelineAsset = ContentSystem::Instance->GetAsset<RenderPipelineAsset>(_RenderPipelineIdentifier);

	//Set if this render pipeline needs a render data table.
	_UsesInternalRenderDataTable = !_RenderPipelineAsset->_CommonData._Bindings.Empty();
	_UsesExternalRenderDataTable = _RenderPipelineAsset->_CommonData._NumberOfExternalTextures > 0;

	ASSERT((!_UsesInternalRenderDataTable && !_UsesExternalRenderDataTable) || _UsesInternalRenderDataTable != _UsesExternalRenderDataTable, "Can't use both internal and external render data tables!");

	//Set up the internal render data tables, if this render pipeline uses one.
	if (_UsesInternalRenderDataTable)
	{
		RenderPipelineUtilities::CreateRenderDataTableLayoutFromBindings
		(
			_RenderPipelineAsset->_CommonData._Bindings,
			ShaderStage::VERTEX | ShaderStage::FRAGMENT,
			&_RenderDataTableLayout,
			&_StorageBufferIncludes
		);

		//Create the render data tables.
		_RenderDataTables.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

		for (uint64 i{ 0 }; i < _RenderDataTables.Size(); ++i)
		{
			//Cache the render data table.
			RenderDataTableHandle &render_data_table{ _RenderDataTables[i] };

			//Create the render data table.
			RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &render_data_table);

			//Bind all bindings.
			for (uint64 binding_index{ 0 }; binding_index < _RenderPipelineAsset->_CommonData._Bindings.Size(); ++binding_index)
			{
				//Cache the binding.
				const RenderPipelineAsset::Binding &binding{ _RenderPipelineAsset->_CommonData._Bindings[binding_index] };

				//Bind depending on type.
				switch (binding._Type)
				{
					case RenderPipelineAsset::Binding::Type::COMPUTE_RENDER_TARGET:
					{
						//Bind the compute render target!
						RenderingSystem::Instance->BindStorageImageToRenderDataTable
						(
							static_cast<uint32>(binding_index),
							0,
							&render_data_table,
							RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(binding._ComputeRenderTargetData._Identifier)
						);

						break;
					}

					case RenderPipelineAsset::Binding::Type::INPUT_RENDER_TARGET:
					{
						//Retrieve the render target.
						RenderTargetHandle render_target;

						//First check if we received an input render target in the parameters.
						bool found{ false };

						for (const Pair<HashString, RenderTargetHandle> &_input_render_target : parameters._InputRenderTargets)
						{
							if (_input_render_target._First == binding._InputRenderTargetData._Identifier)
							{
								render_target = _input_render_target._Second;
								found = true;

								break;
							}
						}

						//Otherwise, try the shared render target manager.
						if (!found)
						{
							render_target = RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(binding._InputRenderTargetData._Identifier);
						}

						//Bind the input render target!
						RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable
						(
							static_cast<uint32>(binding_index),
							0,
							&render_data_table,
							render_target,
							RenderingSystem::Instance->GetSampler(binding._InputRenderTargetData._SamplerProperties)
						);

						break;
					}

					case RenderPipelineAsset::Binding::Type::SAMPLER:
					{
						//Bind the sampler!
						RenderingSystem::Instance->BindSamplerToRenderDataTable
						(
							static_cast<uint32>(binding_index),
							0,
							&render_data_table,
							RenderingSystem::Instance->GetSampler(binding._SamplerData._SamplerProperties)
						);

						break;
					}

					case RenderPipelineAsset::Binding::Type::STORAGE_BUFFER:
					{
						//Bind the storage buffer!
						RenderingSystem::Instance->BindStorageBufferToRenderDataTable
						(
							static_cast<uint32>(binding_index),
							0,
							&render_data_table,
							RenderingSystem::Instance->GetBufferManager()->GetStorageBuffer(binding._StorageBufferData._Identifier, static_cast<uint8>(i))
						);

						break;
					}

					case RenderPipelineAsset::Binding::Type::UNIFORM_BUFFER:
					{
						//Bind the uniform buffer!
						RenderingSystem::Instance->BindUniformBufferToRenderDataTable
						(
							static_cast<uint32>(binding_index),
							0,
							&render_data_table,
							RenderingSystem::Instance->GetBufferManager()->GetUniformBuffer(binding._UniformBufferData._Identifier, static_cast<uint8>(i))
						);

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}
			}
		}
	}

	//Set up the external render data tables, if this render pipeline uses one.
	if (_UsesExternalRenderDataTable)
	{
		constexpr uint64 MAX_BINDINGS{ 16 };

		StaticArray<RenderDataTableLayoutBinding, MAX_BINDINGS> bindings;
		uint32 current_binding_index{ 0 };

		for (uint32 texture_index{ 0 }; texture_index < _RenderPipelineAsset->_CommonData._NumberOfExternalTextures; ++texture_index)
		{
			bindings[current_binding_index] = RenderDataTableLayoutBinding(current_binding_index, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::VERTEX | ShaderStage::FRAGMENT);

			++current_binding_index;

			ASSERT(current_binding_index <= MAX_BINDINGS, "Probably need to increase MAX_BINDINGS");
		}

		RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), current_binding_index, &_RenderDataTableLayout);
	}

	//Set the shaders.
	SetVertexShader(_RenderPipelineAsset->_GraphicsData._VertexShader);
	SetFragmentShader(_RenderPipelineAsset->_GraphicsData._FragmentShader);

	//Set the depth buffer.
	if (_RenderPipelineAsset->_GraphicsData._DepthBuffer)
	{
		DepthBufferHandle depth_buffer{ EMPTY_HANDLE };

		if (parameters._DepthBuffer.Valid() && parameters._DepthBuffer.Get()._First == _RenderPipelineAsset->_GraphicsData._DepthBuffer)
		{
			depth_buffer = parameters._DepthBuffer.Get()._Second;
		}

		else
		{
			depth_buffer = RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(_RenderPipelineAsset->_GraphicsData._DepthBuffer);
		}

		SetDepthBuffer(depth_buffer);
	}

	else if (parameters._DepthBuffer.Valid())
	{
		SetDepthBuffer(parameters._DepthBuffer.Get()._Second);
	}

	//Add the output render targets.
	if ((parameters._RenderDirectlyToScreen.Valid() && parameters._RenderDirectlyToScreen.Get()) || (_RenderPipelineAsset->_GraphicsData._OutputRenderTargets.Size() == 1 && _RenderPipelineAsset->_GraphicsData._OutputRenderTargets[0] == HashString("Screen")))
	{
		SetIsRenderingDirectlyToScreen(true);
	}

	else
	{
		SetNumberOfOutputRenderTargets(_RenderPipelineAsset->_GraphicsData._OutputRenderTargets.Size());

		for (const HashString output_render_target : _RenderPipelineAsset->_GraphicsData._OutputRenderTargets)
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
	SetNumberOfRenderDataTableLayouts((_UsesInternalRenderDataTable || _UsesExternalRenderDataTable) ? 2 : 1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL_2));

	if (_UsesInternalRenderDataTable || _UsesExternalRenderDataTable)
	{
		AddRenderDataTableLayout(_RenderDataTableLayout);
	}

	//Set the render resolution.
	if (parameters._RenderResolution.Valid())
	{
		SetRenderResolution(parameters._RenderResolution.Get());
	}

	else if (_RenderPipelineAsset->_GraphicsData._RenderResolution == HashString("MAIN_FULL"))
	{
		SetRenderResolution(RenderingSystem::Instance->GetScaledResolution(0));
	}

	else if (_RenderPipelineAsset->_GraphicsData._RenderResolution == HashString("MAIN_HALF"))
	{
		SetRenderResolution(RenderingSystem::Instance->GetScaledResolution(1));
	}

	else if (_RenderPipelineAsset->_GraphicsData._RenderResolution == HashString("SHADOW_MAP"))
	{
		SetRenderResolution(Resolution(CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution, CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution));
	}

	else
	{
		ASSERT(false, "Unknown render resolution!");
	}

	//Add together the input stream subscriptions from the resource and the parameters.
	for (const HashString input_stream_subscription : _RenderPipelineAsset->_CommonData._InputStreamSubscriptions)
	{
		_InputStreamSubscriptions.Emplace(input_stream_subscription);
	}

	for (const HashString input_stream_subscription : parameters._InputStreamSubscriptions)
	{
		_InputStreamSubscriptions.Emplace(input_stream_subscription);
	}

	//Retrieve the input stream.
	ASSERT(!_InputStreamSubscriptions.Empty(), "Need at least one input stream subscription!");
	const RenderInputStream &input_stream{ RenderingSystem::Instance->GetRenderInputManager()->GetInputStream(_InputStreamSubscriptions[0]) };

	//Assume that all input streams match in required vertex input attribute/binding descriptions.
	if (!input_stream._RequiredVertexInputAttributeDescriptions.Empty())
	{
		SetNumberOfVertexInputAttributeDescriptions(input_stream._RequiredVertexInputAttributeDescriptions.Size());

		for (const VertexInputAttributeDescription& item : input_stream._RequiredVertexInputAttributeDescriptions)
		{
			AddVertexInputAttributeDescription(item._Location, item._Binding, item._Format, item._Offset);
		}
	}

	if (!input_stream._RequiredVertexInputBindingDescriptions.Empty())
	{
		SetNumberOfVertexInputBindingDescriptions(input_stream._RequiredVertexInputBindingDescriptions.Size());

		for (const VertexInputBindingDescription& item : input_stream._RequiredVertexInputBindingDescriptions)
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
	SetColorAttachmentLoadOperator(_RenderPipelineAsset->_GraphicsData._ColorLoadOperator);
	SetColorAttachmentStoreOperator(_RenderPipelineAsset->_GraphicsData._ColorStoreOperator);
	SetDepthStencilAttachmentLoadOperator(_RenderPipelineAsset->_GraphicsData._DepthStencilLoadOperator);
	SetDepthStencilAttachmentStoreOperator(_RenderPipelineAsset->_GraphicsData._DepthStencilStoreOperator);
	SetBlendEnabled(_RenderPipelineAsset->_GraphicsData._BlendEnabled);
	SetBlendFactorSourceColor(_RenderPipelineAsset->_GraphicsData._BlendColorSourceFactor);
	SetBlendFactorDestinationColor(_RenderPipelineAsset->_GraphicsData._BlendColorDestinationFactor);
	SetColorBlendOperator(_RenderPipelineAsset->_GraphicsData._BlendColorOperator);
	SetBlendFactorSourceAlpha(_RenderPipelineAsset->_GraphicsData._BlendAlphaSourceFactor);
	SetBlendFactorDestinationAlpha(_RenderPipelineAsset->_GraphicsData._BlendAlphaDestinationFactor);
	SetAlphaBlendOperator(_RenderPipelineAsset->_GraphicsData._BlendAlphaOperator);
	SetPolygonMode(_RenderPipelineAsset->_GraphicsData._PolygonMode);
	SetCullMode(_RenderPipelineAsset->_GraphicsData._CullMode);
	SetDepthTestEnabled(parameters._DepthTestEnabled.Valid() ? parameters._DepthTestEnabled.Get() : _RenderPipelineAsset->_GraphicsData._DepthTestEnabled);
	SetDepthWriteEnabled(_RenderPipelineAsset->_GraphicsData._DepthWriteEnabled);
	SetDepthCompareOperator(_RenderPipelineAsset->_GraphicsData._DepthCompareOperator);
	SetStencilTestEnabled(_RenderPipelineAsset->_GraphicsData._StencilTestEnabled);
	SetStencilFailOperator(_RenderPipelineAsset->_GraphicsData._StencilFailOperator);
	SetStencilPassOperator(_RenderPipelineAsset->_GraphicsData._StencilPassOperator);
	SetStencilDepthFailOperator(_RenderPipelineAsset->_GraphicsData._StencilDepthFailOperator);
	SetStencilCompareOperator(_RenderPipelineAsset->_GraphicsData._StencilCompareOperator);
	SetStencilCompareMask(_RenderPipelineAsset->_GraphicsData._StencilCompareMask);
	SetStencilWriteMask(_RenderPipelineAsset->_GraphicsData._StencilWriteMask);
	SetStencilReferenceMask(_RenderPipelineAsset->_GraphicsData._StencilReferenceMask);
	SetTopology(_RenderPipelineAsset->_GraphicsData._Topology);

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName(_RenderPipelineAsset->_Header._AssetName.Data());
#endif

	//Finalize the pipeline.
	FinalizePipeline();
}

/*
*	Executes this graphics pipeline.
*/
void GraphicsRenderPipeline::Execute(const GraphicsRenderPipelineExecuteParameters *const RESTRICT parameters) NOEXCEPT
{
	//Check if this graphics pipeline should be included in the render.
	{
		bool all_input_streams_empty{ true };

		for (const HashString input_stream_subscription : _InputStreamSubscriptions)
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

	PROFILING_SCOPE_DYNAMIC(_RenderPipelineAsset->_Header._AssetName.Data());

	//Retrieve and set the command buffer.
	CommandBuffer *const RESTRICT command_buffer{ RenderingSystem::Instance->GetGlobalCommandBuffer(CommandBufferLevel::SECONDARY) };
	SetCommandBuffer(command_buffer);

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data table.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable2());

	if (_UsesInternalRenderDataTable)
	{
		RenderDataTableHandle &current_render_data_table{ _RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

		//Storage buffers might be re-created to grow/shrink capacity, so re-bind them.
		for (const Pair<HashString, uint32> &storage_buffer_include : _StorageBufferIncludes)
		{
			RenderingSystem::Instance->BindStorageBufferToRenderDataTable
			(
				storage_buffer_include._Second,
				0,
				&current_render_data_table,
				RenderingSystem::Instance->GetBufferManager()->GetStorageBuffer(storage_buffer_include._First, RenderingSystem::Instance->GetCurrentFramebufferIndex())
			);
		}

		command_buffer->BindRenderDataTable(this, 1, current_render_data_table);
	}

	//Go over the input streams.
	for (const HashString input_stream_subscription : _InputStreamSubscriptions)
	{
		const RenderInputStream &input_stream{ RenderingSystem::Instance->GetRenderInputManager()->GetInputStream(input_stream_subscription) };

		ProcessInputStream(input_stream, command_buffer, parameters ? parameters->_PushConstantData : nullptr);
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
	if (_UsesInternalRenderDataTable)
	{
		//Destroy the render data table.
		for (RenderDataTableHandle &render_data_table : _RenderDataTables)
		{
			RenderingSystem::Instance->DestroyRenderDataTable(&render_data_table);
		}
	}

	if (_UsesInternalRenderDataTable || _UsesExternalRenderDataTable)
	{
		//Destroy the render data table layout.
		RenderingSystem::Instance->DestroyRenderDataTableLayout(&_RenderDataTableLayout);
	}
}
