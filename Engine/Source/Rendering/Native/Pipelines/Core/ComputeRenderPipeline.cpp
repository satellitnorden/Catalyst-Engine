//Header file.
#include <Rendering/Native/Pipelines/Core/ComputeRenderPipeline.h>

//Rendering.
#include <Rendering/Native/Utilities/RenderPipelineUtilities.h>
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/ContentSystem.h>
#include <Systems/RenderingSystem.h>

/*
*	Initializes this graphics pipeline.
*/
void ComputeRenderPipeline::Initialize(const ComputeRenderPipelineParameters &parameters) NOEXCEPT
{
	//Reset this pipeline.
	ResetPipeline();

	//Retrieve the render pipeline asset.
	_RenderPipelineAsset = ContentSystem::Instance->GetAsset<RenderPipelineAsset>(_RenderPipelineIdentifier);

	//Set if this render pipeline needs a render data table.
	_UsesRenderDataTable = !_RenderPipelineAsset->_CommonData._Bindings.Empty();

	//If this render pipeline has bindings, create a render data table (and its layout).
	if (_UsesRenderDataTable)
	{
		//Create the render data table layout.
		RenderPipelineUtilities::CreateRenderDataTableLayoutFromBindings
		(
			_RenderPipelineAsset->_CommonData._Bindings,
			ShaderStage::COMPUTE,
			&_RenderDataTableLayout
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
						//Retrieve the render target.
						RenderTargetHandle render_target;

						//First check if we received an input render target in the parameters.
						bool found{ false };

						for (const Pair<HashString, RenderTargetHandle> &_compute_render_target : parameters._ComputeRenderTargets)
						{
							if (_compute_render_target._First == binding._ComputeRenderTargetData._Identifier)
							{
								render_target = _compute_render_target._Second;
								found = true;

								break;
							}
						}

						//Otherwise, try the shared render target manager.
						if (!found)
						{
							render_target = RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(binding._ComputeRenderTargetData._Identifier);
						}

						//Bind the input render target!
						RenderingSystem::Instance->BindStorageImageToRenderDataTable
						(
							static_cast<uint32>(binding_index),
							0,
							&render_data_table,
							render_target
						);

						//Add the compute render target.
						_ComputeRenderTargets.Emplace(render_target);

						break;
					}

					case RenderPipelineAsset::Binding::Type::INPUT_RENDER_TARGET:
					{
						//Bind the input render target!
						RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable
						(
							static_cast<uint32>(binding_index),
							0,
							&render_data_table,
							RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(binding._InputRenderTargetData._Identifier),
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

	//Set the shader.
	SetShader(_RenderPipelineAsset->_ComputeData._Shader);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(_UsesRenderDataTable ? 2 : 1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL_2));

	if (_UsesRenderDataTable)
	{
		AddRenderDataTableLayout(_RenderDataTableLayout);
	}

	//Retrieve the input stream.
	ASSERT(!_RenderPipelineAsset->_CommonData._InputStreamSubscriptions.Empty(), "Need at least one input stream subscription!");
	const RenderInputStream  input_stream{ RenderingSystem::Instance->GetRenderInputManager()->GetInputStream(_RenderPipelineAsset->_CommonData._InputStreamSubscriptions[0]) };

	//Set the push constant ranges.
	if (input_stream._RequiredPushConstantDataSize > 0)
	{
		SetNumberOfPushConstantRanges(1);
		AddPushConstantRange(ShaderStage::COMPUTE, 0, static_cast<uint32>(input_stream._RequiredPushConstantDataSize));
	}

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
void ComputeRenderPipeline::Execute() NOEXCEPT
{
	//Check if this graphics pipeline should be included in the render.
	{
		bool all_input_streams_empty{ true };

		for (const HashString input_stream_subscription : _RenderPipelineAsset->_CommonData._InputStreamSubscriptions)
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

	//Insert memory barriers for the compute render targets.
	for (const RenderTargetHandle compute_render_target : _ComputeRenderTargets)
	{
		command_buffer->ImageMemoryBarrier
		(
			this,
			AccessFlags::COLOR_ATTACHMENT_WRITE,
			AccessFlags::SHADER_READ,
			ImageLayout::GENERAL,
			ImageLayout::GENERAL,
			compute_render_target,
			PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT,
			PipelineStageFlags::COMPUTE_SHADER
		);
	}

	//Go over the input streams.
	for (const HashString input_stream_subscription : _RenderPipelineAsset->_CommonData._InputStreamSubscriptions)
	{
		const RenderInputStream &input_stream{ RenderingSystem::Instance->GetRenderInputManager()->GetInputStream(input_stream_subscription) };

		ProcessInputStream(input_stream, command_buffer);
	}

	//Insert memory barriers for the compute render targets.
	for (const RenderTargetHandle compute_render_target : _ComputeRenderTargets)
	{
		command_buffer->ImageMemoryBarrier
		(
			this,
			AccessFlags::SHADER_WRITE,
			AccessFlags::SHADER_READ,
			ImageLayout::GENERAL,
			ImageLayout::GENERAL,
			compute_render_target,
			PipelineStageFlags::COMPUTE_SHADER,
			PipelineStageFlags::FRAGMENT_SHADER
		);
	}
	
	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}

/*
*	Terminates this graphics pipeline.
*/
void ComputeRenderPipeline::Terminate() NOEXCEPT
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
