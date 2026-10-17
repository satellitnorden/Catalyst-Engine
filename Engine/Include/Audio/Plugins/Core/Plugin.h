#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Audio.
#include <Audio/Core/AudioProcessContext.h>

namespace Audio
{

	/*
	*	Base class for an audio plugin.
	*	Exposes functionality for building audio plugins that can be loaded by a DAW, such as .clap or .vst3 plugins.
	*/
	class Plugin
	{

	public:

		/*
		*	Default destructor.
		*/
		FORCE_INLINE virtual ~Plugin() NOEXCEPT
		{

		}

		/*
		*	Returns the name of this plugin.
		*/
		FORCE_INLINE virtual const char *const RESTRICT Name() const NOEXCEPT = 0;

		/*
		*	Callback for this plugin to process the given buffer.
		*/
		FORCE_INLINE virtual void Process
		(
			const AudioProcessContext &context,
			const DynamicArray<DynamicArray<float32>> &inputs,
			DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
			const uint8 number_of_channels,
			const uint32 number_of_samples
		) NOEXCEPT
		{
			for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
			{
				for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
				{
					outputs->At(channel_index).At(sample_index) = inputs.At(channel_index).At(sample_index);
				}
			}
		}

	};
}