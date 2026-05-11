#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Content.
#include <Content/Assets/RenderPipelineAsset.h>

//Systems.
#include <Systems/RenderingSystem.h>

namespace RenderPipelineUtilities
{

	/*
	*	Creates a render data table layout from the given bindings.
	*/
	FORCE_INLINE static void CreateRenderDataTableLayoutFromBindings
	(
		const DynamicArray<RenderPipelineAsset::Binding> &bindings,
		const ShaderStage shader_stages,
		RenderDataTableLayoutHandle *const RESTRICT render_data_table_layout,
		DynamicArray<Pair<HashString, uint32>> *const RESTRICT storage_buffer_includes = nullptr
	) NOEXCEPT
	{
		//Define constants.
		constexpr uint64 MAX_BINDINGS{ 16 };

		//Do some sanity checking.
		ASSERT(bindings.Size() < MAX_BINDINGS, "Increase max bindings!");

		//Set up the bindings.
		StaticArray<RenderDataTableLayoutBinding, MAX_BINDINGS> _bindings;
		uint32 number_of_bindings{ 0 };

		for (uint64 binding_index{ 0 }; binding_index < bindings.Size(); ++binding_index)
		{
			const RenderPipelineAsset::Binding &binding{ bindings[binding_index] };

			switch (binding._Type)
			{
				case RenderPipelineAsset::Binding::Type::COMPUTE_RENDER_TARGET:
				{
					_bindings[number_of_bindings++] = RenderDataTableLayoutBinding(static_cast<uint32>(binding_index), RenderDataTableLayoutBinding::Type::StorageImage, 1, shader_stages);

					break;
				}

				case RenderPipelineAsset::Binding::Type::INPUT_RENDER_TARGET:
				{
					_bindings[number_of_bindings++] = RenderDataTableLayoutBinding(static_cast<uint32>(binding_index), RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, shader_stages);

					break;
				}

				case RenderPipelineAsset::Binding::Type::SAMPLER:
				{
					_bindings[number_of_bindings++] = RenderDataTableLayoutBinding(static_cast<uint32>(binding_index), RenderDataTableLayoutBinding::Type::Sampler, 1, shader_stages);

					break;
				}

				case RenderPipelineAsset::Binding::Type::STORAGE_BUFFER:
				{
					_bindings[number_of_bindings++] = RenderDataTableLayoutBinding(static_cast<uint32>(binding_index), RenderDataTableLayoutBinding::Type::StorageBuffer, 1, shader_stages);

					if (storage_buffer_includes)
					{
						storage_buffer_includes->Emplace(binding._StorageBufferData._Identifier, static_cast<uint32>(binding_index));
					}

					break;
				}

				case RenderPipelineAsset::Binding::Type::UNIFORM_BUFFER:
				{
					_bindings[number_of_bindings++] = RenderDataTableLayoutBinding(static_cast<uint32>(binding_index), RenderDataTableLayoutBinding::Type::UniformBuffer, 1, shader_stages);

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}
		}

		RenderingSystem::Instance->CreateRenderDataTableLayout(_bindings.Data(), number_of_bindings, render_data_table_layout);
	}
}