#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Audio.
#include <Audio/Core/Audio.h>
#include <Audio/Utilities/PerceptualLoudnessFilter.h>

/*
*	Simple compressor calibrator utility class,
*	that can automatically tweak the threshold of a compressor to achieve a target (maximum) gain reduction,
*	and automatically compensates the makeup gain.
*/
class CompressorCalibrator final
{

public:

	//The target gain reduction.
	float32 _TargetGainReduction{ 1.0f };

	//Denotes if this compressor calibrator should calibrate.
	bool _ShouldCalibrate{ false };

	/*
	*	Initializes this compressor calibrator.
	*/
	FORCE_INLINE void Initialize
	(
		const float32 sample_rate,
		float32 *const RESTRICT threshold,
		const float32 minimum_threshold,
		const float32 maximum_threshold,
		float32 *const RESTRICT makeup,
		const float32 minimum_makeup,
		const float32 maximum_makeup
	) NOEXCEPT
	{
		//Initialize the perceptual loudness filters.
		_InputPerceptualLoudnessFilter.Initialize(sample_rate);
		_OutputPerceptualLoudnessFilter.Initialize(sample_rate);

		//Set the threshold.
		_Threshold = threshold;

		//Set the minimum threshold.
		_MinimumThreshold = minimum_threshold;

		//Set the maximum threshold.
		_MaximumThreshold = maximum_threshold;

		//Set the makeup.
		_Makeup = makeup;

		//Set the minimum makeup.
		_MinimumMakeup = minimum_makeup;

		//Set the maximum makeup.
		_MaximumMakeup = maximum_makeup;
	}

	/*
	*	Sets the sample rate.
	*/
	FORCE_INLINE void SetSampleRate(const float32 sample_rate) NOEXCEPT
	{
		//Initialize the perceptual loudness filters.
		_InputPerceptualLoudnessFilter.Initialize(sample_rate);
		_OutputPerceptualLoudnessFilter.Initialize(sample_rate);
	}

	/*
	*	Pre-updates calibration.
	*/
	FORCE_INLINE void PreUpdateCalibration
	(
		const DynamicArray<DynamicArray<float32>> &inputs,
		const uint8 number_of_channels,
		const uint32 number_of_samples
	) NOEXCEPT
	{
		//If we weren't calibrating, but is now, start the calibration.
		if (!_IsCalibrating && _ShouldCalibrate)
		{
			StartCalibration();
		}

		//Or if we were calibrating, but is no longer, stop it.
		if (_IsCalibrating && !_ShouldCalibrate)
		{
			StopCalibration();
		}

		//If we're not calibrating, nothing more to do. (:
		if (!_IsCalibrating)
		{
			return;
		}

		//Update the input RMS.
		for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			//Sum all of the channels.
			float32 sum{ 0.0f };

			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				sum += inputs.At(channel_index).At(sample_index);
			}

			sum /= static_cast<float32>(number_of_channels);

			//Apply the input perceptual loudness filter.
			sum = _InputPerceptualLoudnessFilter.Process(sum);

			//At to the input RMS.
			_InputRMS += static_cast<float64>(sum * sum);
		}
	}

	/*
	*	Post-updates calibration.
	*/
	FORCE_INLINE void PostUpdateCalibration
	(
		const DynamicArray<DynamicArray<float32>> &outputs,
		const uint8 number_of_channels,
		const uint32 number_of_samples,
		const float32 gain_reduction
	) NOEXCEPT
	{
		//If we're not calibrating, nothing more to do. (:
		if (!_IsCalibrating)
		{
			return;
		}

		//Increase the threshold if the gain reduction is higher than the target gain reduction.
		if (gain_reduction > Audio::GainToDecibels(_TargetGainReduction))
		{
			_CurrentThreshold = BaseMath::Minimum<float32>(_CurrentThreshold + 0.1f, 0.0f);
			(*_Threshold) = Audio::DecibelsToGain(_CurrentThreshold);
			_NumberOfRMSSamples = 0;
			_InputRMS = 0.0;
			_OutputRMS = 0.0;
		}

		else
		{
			//Update the output RMS.
			for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
			{
				//Sum all of the channels.
				float32 sum{ 0.0f };

				for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
				{
					sum += outputs.At(channel_index).At(sample_index);
				}

				sum /= static_cast<float32>(number_of_channels);

				//Apply the output perceptual loudness filter.
				sum = _OutputPerceptualLoudnessFilter.Process(sum);

				//At to the output RMS.
				_OutputRMS += static_cast<float64>(sum * sum);

				//Increment the number of RMS samples.
				++_NumberOfRMSSamples;
			}
		}
	}

private:

	//Denotes whether or not this compressor calibrator is calibrating.
	bool _IsCalibrating{ false };

	//The threshold.
	float32 *RESTRICT _Threshold{ nullptr };

	//The minimum threshold.
	float32 _MinimumThreshold{ 0.0f };
	
	//The maximum threshold.
	float32 _MaximumThreshold{ 0.0f };

	//The makeup.
	float32 *RESTRICT _Makeup{ nullptr };

	//The minimum makeup.
	float32 _MinimumMakeup{ 0.0f };

	//The maximum makeup.
	float32 _MaximumMakeup{ 0.0f };

	//The current threshold.
	float32 _CurrentThreshold{ 0.0f };

	//The input perceptual loudness filter.
	PerceptualLoudnessFilter _InputPerceptualLoudnessFilter;

	//The output perceptual loudness filter.
	PerceptualLoudnessFilter _OutputPerceptualLoudnessFilter;

	//The number of RMS samples.
	uint64 _NumberOfRMSSamples{ 0 };

	//The input RMS.
	float64 _InputRMS{ 0.0 };

	//The output RMS.
	float64 _OutputRMS{ 0.0 };

	/*
	*	Starts calibration.
	*/
	FORCE_INLINE void StartCalibration() NOEXCEPT
	{
		//Reset the calibration data.
		_CurrentThreshold = _MinimumThreshold;
		_NumberOfRMSSamples = 0;
		_InputRMS = 0.0;
		_OutputRMS = 0.0;

		//Set the threshold.
		(*_Threshold) = Audio::DecibelsToGain(_CurrentThreshold);

		//Set the makeup.
		(*_Makeup) = 1.0f;

		//We are now calibrating!
		_IsCalibrating = true;
	}

	/*
	*	Stops calibration.
	*/
	FORCE_INLINE void StopCalibration() NOEXCEPT
	{
		//Set the threshold.
		(*_Threshold) = Audio::DecibelsToGain(_CurrentThreshold);

		//Finalize the input/output RMS.
		if (_NumberOfRMSSamples > 0)
		{
			_InputRMS /= static_cast<float64>(_NumberOfRMSSamples);
			_InputRMS = _InputRMS > 0.0 ? std::sqrt(_InputRMS) : 1.0;

			_OutputRMS /= static_cast<float64>(_NumberOfRMSSamples);
			_OutputRMS = _OutputRMS > 0.0 ? std::sqrt(_OutputRMS) : 1.0;
		}

		else
		{
			_InputRMS = 1.0;
			_OutputRMS = 1.0;
		}

		//Calculate the loudness compensation.
		float32 loudness_compensation{ Audio::GainToDecibels(static_cast<float32>(_InputRMS)) - Audio::GainToDecibels(static_cast<float32>(_OutputRMS)) };
		loudness_compensation = BaseMath::Minimum<float32>(loudness_compensation, _MaximumMakeup);

		//Set the makeup.
		(*_Makeup) = Audio::DecibelsToGain(loudness_compensation);

		//We are no longer calibrating!
		_IsCalibrating = false;
	}

};