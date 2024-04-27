//Header file.
#include <Rendering/Native/Pipelines/Core/RayTracingRenderPipeline.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

/*
*	Initializes this graphics pipeline.
*/
void RayTracingRenderPipeline::Initialize(const RayTracingRenderPipelineParameters &parameters) NOEXCEPT
{
	//Retrieve the render pipeline resource.
	_RenderPipelineResource = ResourceSystem::Instance->GetRenderPipelineResource(_RenderPipelineIdentifier);

	//Reset this pipeline.
	ResetPipeline();

	//Calculate the shader stages.
	ShaderStage shader_stages{ static_cast<ShaderStage>(0) };

	if (_RenderPipelineResource->_RayGenerationShaderHandle != EMPTY_HANDLE)
	{
		shader_stages |= ShaderStage::RAY_GENERATION;
	}

	if (_RenderPipelineResource->_RayMissShaderHandle != EMPTY_HANDLE)
	{
		shader_stages |= ShaderStage::RAY_MISS;
	}

	shader_stages |= ShaderStage::RAY_CLOSEST_HIT;
	shader_stages |= ShaderStage::RAY_ANY_HIT;

	//Set if this render pipeline needs a render data table.
	_UsesRenderDataTable =	!_RenderPipelineResource->_IncludedUniformBuffers.Empty()
							|| !_RenderPipelineResource->_IncludedStorageBuffers.Empty()
							|| !_RenderPipelineResource->_SamplerProperties.Empty()
							|| !_RenderPipelineResource->_InputRenderTargets.Empty()
							|| !_RenderPipelineResource->_ComputeRenderTargets.Empty();

	//If this render pipeline has input render targets, create a render data table (and its layout).
	if (_UsesRenderDataTable)
	{
		//Create the render data table layout.
		{
			constexpr uint64 MAX_BINDINGS{ 16 };

			StaticArray<RenderDataTableLayoutBinding, MAX_BINDINGS> bindings;
			uint32 current_binding_index{ 0 };

			for (const HashString included_uniform_buffer : _RenderPipelineResource->_IncludedUniformBuffers)
			{
				bindings[current_binding_index] = RenderDataTableLayoutBinding(current_binding_index, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, shader_stages);
			
				++current_binding_index;

				ASSERT(current_binding_index <= MAX_BINDINGS, "Probably need to increase MAX_BINDINGS");
			}

			for (const HashString included_storage_buffer : _RenderPipelineResource->_IncludedStorageBuffers)
			{
				bindings[current_binding_index] = RenderDataTableLayoutBinding(current_binding_index, RenderDataTableLayoutBinding::Type::StorageBuffer, 1, shader_stages);

				++current_binding_index;

				ASSERT(current_binding_index <= MAX_BINDINGS, "Probably need to increase MAX_BINDINGS");
			}

			for (const SamplerProperties &sampler_properties : _RenderPipelineResource->_SamplerProperties)
			{
				bindings[current_binding_index] = RenderDataTableLayoutBinding(current_binding_index, RenderDataTableLayoutBinding::Type::Sampler, 1, shader_stages);

				++current_binding_index;

				ASSERT(current_binding_index <= MAX_BINDINGS, "Probably need to increase MAX_BINDINGS");
			}

			for (const Pair<HashString, SamplerProperties> &input_render_target : _RenderPipelineResource->_InputRenderTargets)
			{
				bindings[current_binding_index] = RenderDataTableLayoutBinding(current_binding_index, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, shader_stages);
			
				++current_binding_index;

				ASSERT(current_binding_index <= MAX_BINDINGS, "Probably need to increase MAX_BINDINGS");
			}

			for (const HashString compute_render_target : _RenderPipelineResource->_ComputeRenderTargets)
			{
				bindings[current_binding_index] = RenderDataTableLayoutBinding(current_binding_index, RenderDataTableLayoutBinding::Type::StorageImage, 1, shader_stages);

				++current_binding_index;

				ASSERT(current_binding_index <= MAX_BINDINGS, "Probably need to increase MAX_BINDINGS");
			}

			RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), current_binding_index, &_RenderDataTableLayout);
		}

		//Create the render data tables.
		_RenderDataTables.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

		for (uint64 i{ 0 }; i < _RenderDataTables.Size(); ++i)
		{
			RenderDataTableHandle& render_data_table{ _RenderDataTables[i] };

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

			for (const Pair<HashString, SamplerProperties> &input_render_target : _RenderPipelineResource->_InputRenderTargets)
			{
				RenderTargetHandle render_target;

				//First check if we received an input render target in the parameters.
				bool found{ false };

				/*
				for (const Pair<HashString, RenderTargetHandle>& _input_render_target : parameters._InputRenderTargets)
				{
					if (_input_render_target._First == input_render_target)
					{
						render_target = _input_render_target._Second;
						found = true;

						break;
					}
				}
				*/

				//Otherwise, try the shared render target manager.
				if (!found)
				{
					render_target = RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(input_render_target._First);
				}

				RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable
				(
					current_binding_index++,
					0,
					&render_data_table,
					render_target,
					RenderingSystem::Instance->GetSampler(input_render_target._Second)
				);
			}

			for (const HashString compute_render_target : _RenderPipelineResource->_ComputeRenderTargets)
			{
				RenderTargetHandle render_target;

				//First check if we received an input render target in the parameters.
				bool found{ false };

				for (const Pair<HashString, RenderTargetHandle> &_compute_render_target : parameters._ComputeRenderTargets)
				{
					if (_compute_render_target._First == compute_render_target)
					{
						render_target = _compute_render_target._Second;
						found = true;

						break;
					}
				}

				//Otherwise, try the shared render target manager.
				if (!found)
				{
					render_target = RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(compute_render_target);
				}

				RenderingSystem::Instance->BindStorageImageToRenderDataTable
				(
					current_binding_index++,
					0,
					&render_data_table,
					render_target
				);

				_ComputeRenderTargets.Emplace(render_target);
			}
		}
	}

	//Set the shaders.
	if (_RenderPipelineResource->_RayGenerationShaderHandle)
	{
		SetRayGenerationShader(_RenderPipelineResource->_RayGenerationShaderHandle);
	}

	if (_RenderPipelineResource->_RayMissShaderHandle)
	{
		SetNumberOfMissShaders(1);
		AddMissShader(_RenderPipelineResource->_RayMissShaderHandle);
	}

	//Add the hit groups.
	if (!_RenderPipelineResource->_RayHitGroupShaderData.Empty())
	{
		SetNumberOfHitGroups(RenderingSystem::Instance->GetRayTracingSystem()->GetHitGroups().Size());

		for (const RayTracingHitGroup &hit_group : RenderingSystem::Instance->GetRayTracingSystem()->GetHitGroups())
		{
			const RenderPipelineResource::RayHitGroupShaderData *RESTRICT ray_hit_group_shader_data{ nullptr };

			for (const RenderPipelineResource::RayHitGroupShaderData &_ray_hit_group_shader_data : _RenderPipelineResource->_RayHitGroupShaderData)
			{
				if (_ray_hit_group_shader_data._Identifier == hit_group._Identifier)
				{
					ray_hit_group_shader_data = &_ray_hit_group_shader_data;

					break;
				}
			}

			AddHitGroup
			(
				ray_hit_group_shader_data ? ray_hit_group_shader_data->_RayClosestHitShaderHandle : EMPTY_HANDLE,
				ray_hit_group_shader_data ? ray_hit_group_shader_data->_RayAnyHitShaderHandle : EMPTY_HANDLE,
				EMPTY_HANDLE
			);
		}

		//for (const RenderPipelineResource::RayHitGroupShaderData &ray_hit_group_shader_data : _RenderPipelineResource->_RayHitGroupShaderData)
		//{
		//	AddHitGroup(ray_hit_group_shader_data._RayClosestHitShaderHandle, ray_hit_group_shader_data._RayAnyHitShaderHandle, EMPTY_HANDLE);
		//}
	}

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(_UsesRenderDataTable ? 3 : 2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL_2));

	if (_UsesRenderDataTable)
	{
		AddRenderDataTableLayout(_RenderDataTableLayout);
	}

	AddRenderDataTableLayout(RenderingSystem::Instance->GetRayTracingSystem()->GetRenderDataTableLayout());

	//Retrieve the input stream.
	ASSERT(!_RenderPipelineResource->_InputStreamSubscriptions.Empty(), "Need at least one input stream subscription!");
	const RenderInputStream &input_stream{ RenderingSystem::Instance->GetRenderInputManager()->GetInputStream(_RenderPipelineResource->_InputStreamSubscriptions[0]) };

	//Set the push constant ranges.
	if (input_stream._RequiredPushConstantDataSize > 0)
	{
		SetNumberOfPushConstantRanges(1);
		AddPushConstantRange(shader_stages, 0, static_cast<uint32>(input_stream._RequiredPushConstantDataSize));
	}

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName(_RenderPipelineResource->_Header._ResourceName.Data());
#endif

	//Finalize the pipeline.
	FinalizePipeline();
}

/*
*	Executes this graphics pipeline.
*/
void RayTracingRenderPipeline::Execute() NOEXCEPT
{
	//Check if ray tracing is possible.
	if (!RenderingSystem::Instance->IsRayTracingPossible())
	{
		SetIncludeInRender(false);

		return;
	}

	//Check if this render pipeline should be included in the render.
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

	command_buffer->BindRenderDataTable(this, _UsesRenderDataTable ? 2 : 1, RenderingSystem::Instance->GetRayTracingSystem()->GetCurrentRenderDataTable());

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
			PipelineStageFlags::RAY_TRACING_SHADER
		);
	}

	//Go over the input streams.
	for (const HashString input_stream_subscription : _RenderPipelineResource->_InputStreamSubscriptions)
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
			PipelineStageFlags::RAY_TRACING_SHADER,
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
void RayTracingRenderPipeline::Terminate() NOEXCEPT
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