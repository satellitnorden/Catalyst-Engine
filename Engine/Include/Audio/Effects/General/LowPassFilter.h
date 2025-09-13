#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>
#include <Audio/Biquad.h>

//Math.
#include <Math/Core/BaseMath.h>

//STL.
#include <cmath>

class LowPassFilter final : public AudioEffect
{

public:

	//Constants.
	constexpr static float32 DEFAULT_FREQUENCY{ 20'000.0f };
	constexpr static float32 DEFAULT_QUALITY{ 1.0f };
	constexpr static uint32 DEFAULT_ORDER{ 2 };

	//The frequency.
	float32 _Frequency{ DEFAULT_FREQUENCY };

	//The quality.
	float32 _Quality{ DEFAULT_QUALITY };

	//The order.
	uint32 _Order{ DEFAULT_ORDER };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE LowPassFilter(const float32 frequency = DEFAULT_FREQUENCY, const float32 quality = DEFAULT_QUALITY, const uint32 order = DEFAULT_ORDER) NOEXCEPT
		:
		_Frequency(frequency),
		_Quality(quality),
		_Order(order)
	{
		//Update the previous frequency.
		_PreviousFrequency = _Frequency;

		//Update the previous quality.
		_PreviousQuality = _Quality;

		//Update the previous order.
		_PreviousOrder = _Order;

		//Reset.
		Reset();
	}

	/*
	*	Callback for when the sample rate changed.
	*/
	FORCE_INLINE void OnSampleRateChanged() NOEXCEPT override
	{
		//Reset.
		Reset();
	}

	/*
	*	Callback for this audio effect to process the given buffer.
	*/
	FORCE_INLINE void Process
	(
		const AudioProcessContext &context,
		const DynamicArray<DynamicArray<float32>> &inputs,
		DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
		const uint8 number_of_channels,
		const uint32 number_of_samples
	) NOEXCEPT override
	{
		//Copy inputs into outputs as all subsequent steps works inplace.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);
		}

		//Skip if the frequency is zero.
		if (_Frequency <= FLOAT32_EPSILON)
		{
			return;
		}

		//If the values has changed, reset
		if (_PreviousFrequency != _Frequency
			|| _PreviousQuality != _Quality
			|| _PreviousOrder != _Order)
		{
			//Reset.
			Reset();

			//Update the previous values.
			_PreviousFrequency = _Frequency;
			_PreviousQuality = _Quality;
			_PreviousOrder = _Order;
		}

		//Process all channels.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			//Process all second order filters.
			for (SecondOrderFilter &second_order_filter : _SecondOrderFilters)
			{
				second_order_filter._Biquads[channel_index].Process(outputs->At(channel_index).Data(), outputs->At(channel_index).Data(), number_of_samples);
			}

			//Process first order filters.
			for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
			{
				float32 output_sample{ outputs->At(channel_index).At(sample_index) };

				for (FirstOrderFilter &first_order_filter : _FirstOrderFilters)
				{
					output_sample = ProcessFirstOrderFilter(channel_index, output_sample, &first_order_filter);
				}

				outputs->At(channel_index).At(sample_index) = output_sample;
			}
		}
	}

private:

	/*
	*	Second order filter class definition.
	*/
	class SecondOrderFilter final
	{

	public:

		//The biquads.
		StaticArray<Biquad, 2> _Biquads;

	};

	/*
	*	First order filter class definition.
	*/
	class FirstOrderFilter final
	{

	public:

		//The previous output samples.
		StaticArray<float32, 2> _PreviousOutputSamples{ 0.0f };

		//The alpha.
		float32 _Alpha;

	};

	//The previous frequency.
	float32 _PreviousFrequency{ DEFAULT_FREQUENCY };

	//The previous quality.
	float32 _PreviousQuality{ DEFAULT_QUALITY };

	//The previous order.
	uint32 _PreviousOrder{ DEFAULT_ORDER };

	//The second order filters.
	DynamicArray<SecondOrderFilter> _SecondOrderFilters;

	//The first order filters.
	DynamicArray<FirstOrderFilter> _FirstOrderFilters;

	/*
	*	Resets.
	*/
	FORCE_INLINE void Reset() NOEXCEPT
	{
		//Calculate the number of second order filters.
		const uint32 number_of_second_order_filters{ _Order / 2 };

		//Add new ones or destroy old ones if needed.
		if (number_of_second_order_filters > _SecondOrderFilters.Size())
		{
			const uint64 number_of_new_second_order_filters{ number_of_second_order_filters - _SecondOrderFilters.Size() };

			for (uint64 i{ 0 }; i < number_of_new_second_order_filters; ++i)
			{
				_SecondOrderFilters.Emplace();
				SecondOrderFilter &second_order_filter{ _SecondOrderFilters.Back() };

				Memory::Set(&second_order_filter, 0, sizeof(SecondOrderFilter));
			}
		}

		else if (number_of_second_order_filters < _SecondOrderFilters.Size())
		{
			const uint64 number_of_old_second_order_filters{ _SecondOrderFilters.Size() - number_of_second_order_filters };

			for (uint64 i{ 0 }; i < number_of_old_second_order_filters; ++i)
			{
				_SecondOrderFilters.Pop();
			}
		}

		//Calculate the number of first order filters.
		const uint32 number_of_first_order_filters{ _Order % 2 };

		//Add new ones or destroy old ones if needed.
		if (number_of_first_order_filters > _FirstOrderFilters.Size())
		{
			const uint64 number_of_new_first_order_filters{ number_of_first_order_filters - _FirstOrderFilters.Size() };

			for (uint64 i{ 0 }; i < number_of_new_first_order_filters; ++i)
			{
				_FirstOrderFilters.Emplace();
				FirstOrderFilter &first_order_filter{ _FirstOrderFilters.Back() };

				Memory::Set(&first_order_filter, 0, sizeof(FirstOrderFilter));
			}
		}

		else if (number_of_first_order_filters < _FirstOrderFilters.Size())
		{
			const uint64 number_of_old_first_order_filters{ _FirstOrderFilters.Size() - number_of_first_order_filters };

			for (uint64 i{ 0 }; i < number_of_old_first_order_filters; ++i)
			{
				_FirstOrderFilters.Pop();
			}
		}

		//Reset the second order filters.
		for (uint32 i{ 0 }; i < number_of_second_order_filters; ++i)
		{
			SecondOrderFilter &second_order_filter{ _SecondOrderFilters[i] };

			if (_Frequency <= FLOAT32_EPSILON)
			{
				continue;
			}

			for (Biquad &biquad : second_order_filter._Biquads)
			{
				biquad.InitializeLowPass(_Frequency, _Quality, _SampleRate);
			}
		}

		//Reset the first order filters.
		for (uint32 i{ 0 }; i < number_of_first_order_filters; ++i)
		{
			FirstOrderFilter &first_order_filter{ _FirstOrderFilters[i] };

			if (_Frequency <= FLOAT32_EPSILON)
			{
				continue;
			}

			const float32 RC{ 1.0f / (2.0f * BaseMathConstants::PI * _Frequency) };
			const float32 T{ 1.0f / _SampleRate };

			first_order_filter._Alpha = RC / (RC + T);
		}
	}

	/*
	*	Process the given first order filter.
	*/
	FORCE_INLINE NO_DISCARD float32 ProcessFirstOrderFilter(const uint8 channel_index, const float32 input_sample, FirstOrderFilter* const RESTRICT first_order_filter) NOEXCEPT
	{
		const float32 output_sample
		{
			input_sample * (1.0f - first_order_filter->_Alpha) + first_order_filter->_PreviousOutputSamples[channel_index] * first_order_filter->_Alpha
		};

		first_order_filter->_PreviousOutputSamples[channel_index] = output_sample;

		return output_sample;
	}

};