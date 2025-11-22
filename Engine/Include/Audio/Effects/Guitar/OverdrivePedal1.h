#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Core/Audio.h>
#include <Audio/Effects/Core/AudioEffect.h>
#include <Audio/Effects/General/HighPassFilter.h>
#include <Audio/Effects/General/LowPassFilter.h>
#include <Audio/Effects/General/PeakFilter.h>
#include <Audio/OverSampler.h>

//Math.
#include <Math/General/Curve.h>

/*
*	This an effect modelling an overdrive pedal, similar to the Boss SD-1.
*	It has an asymmetric distortion circuit, level, drive and tone controls.
*/
class OverdrivePedal1 final : public AudioEffect
{

public:

	//The drive.
	float32 _Drive{ 0.5f };

	//The tone.
	float32 _Tone{ 0.5f };

	//The level.
	float32 _Level{ 0.5f };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE OverdrivePedal1() NOEXCEPT
	{
		//Set up the level curve.
		_LevelCurve.SetValue(0, SoundUtilities::DecibelsToGain(-48.0f));
		_LevelCurve.SetValue(1, SoundUtilities::DecibelsToGain(0.0f));
		_LevelCurve.SetValue(2, SoundUtilities::DecibelsToGain(12.0f));

		//Set up the calibration curve.
		_CalibrationCurve.SetValue(0, 0.310773f); //-10.151124dB at 0.00% drive
		_CalibrationCurve.SetValue(1, 0.295199f); //-10.597711dB at 1.56% drive
		_CalibrationCurve.SetValue(2, 0.280993f); //-11.026098dB at 3.12% drive
		_CalibrationCurve.SetValue(3, 0.268029f); //-11.436350dB at 4.69% drive
		_CalibrationCurve.SetValue(4, 0.256195f); //-11.828604dB at 6.25% drive
		_CalibrationCurve.SetValue(5, 0.245383f); //-12.203094dB at 7.81% drive
		_CalibrationCurve.SetValue(6, 0.235502f); //-12.560119dB at 9.38% drive
		_CalibrationCurve.SetValue(7, 0.226463f); //-12.900048dB at 10.94% drive
		_CalibrationCurve.SetValue(8, 0.218190f); //-13.223313dB at 12.50% drive
		_CalibrationCurve.SetValue(9, 0.210611f); //-13.530397dB at 14.06% drive
		_CalibrationCurve.SetValue(10, 0.203661f); //-13.821826dB at 15.62% drive
		_CalibrationCurve.SetValue(11, 0.197284f); //-14.098171dB at 17.19% drive
		_CalibrationCurve.SetValue(12, 0.191425f); //-14.360047dB at 18.75% drive
		_CalibrationCurve.SetValue(13, 0.186035f); //-14.608093dB at 20.31% drive
		_CalibrationCurve.SetValue(14, 0.181072f); //-14.842963dB at 21.88% drive
		_CalibrationCurve.SetValue(15, 0.176496f); //-15.065318dB at 23.44% drive
		_CalibrationCurve.SetValue(16, 0.172270f); //-15.275803dB at 25.00% drive
		_CalibrationCurve.SetValue(17, 0.168363f); //-15.475058dB at 26.56% drive
		_CalibrationCurve.SetValue(18, 0.164746f); //-15.663694dB at 28.12% drive
		_CalibrationCurve.SetValue(19, 0.161393f); //-15.842283dB at 29.69% drive
		_CalibrationCurve.SetValue(20, 0.158282f); //-16.011379dB at 31.25% drive
		_CalibrationCurve.SetValue(21, 0.155391f); //-16.171492dB at 32.81% drive
		_CalibrationCurve.SetValue(22, 0.152702f); //-16.323105dB at 34.38% drive
		_CalibrationCurve.SetValue(23, 0.150199f); //-16.466688dB at 35.94% drive
		_CalibrationCurve.SetValue(24, 0.147866f); //-16.602661dB at 37.50% drive
		_CalibrationCurve.SetValue(25, 0.145690f); //-16.731434dB at 39.06% drive
		_CalibrationCurve.SetValue(26, 0.143658f); //-16.853390dB at 40.62% drive
		_CalibrationCurve.SetValue(27, 0.141761f); //-16.968884dB at 42.19% drive
		_CalibrationCurve.SetValue(28, 0.139987f); //-17.078260dB at 43.75% drive
		_CalibrationCurve.SetValue(29, 0.138328f); //-17.181828dB at 45.31% drive
		_CalibrationCurve.SetValue(30, 0.136775f); //-17.279884dB at 46.88% drive
		_CalibrationCurve.SetValue(31, 0.135321f); //-17.372715dB at 48.44% drive
		_CalibrationCurve.SetValue(32, 0.133959f); //-17.460579dB at 50.00% drive
		_CalibrationCurve.SetValue(33, 0.132683f); //-17.543720dB at 51.56% drive
		_CalibrationCurve.SetValue(34, 0.131487f); //-17.622366dB at 53.12% drive
		_CalibrationCurve.SetValue(35, 0.130366f); //-17.696737dB at 54.69% drive
		_CalibrationCurve.SetValue(36, 0.129315f); //-17.767036dB at 56.25% drive
		_CalibrationCurve.SetValue(37, 0.128330f); //-17.833441dB at 57.81% drive
		_CalibrationCurve.SetValue(38, 0.127407f); //-17.896143dB at 59.38% drive
		_CalibrationCurve.SetValue(39, 0.126542f); //-17.955297dB at 60.94% drive
		_CalibrationCurve.SetValue(40, 0.125732f); //-18.011066dB at 62.50% drive
		_CalibrationCurve.SetValue(41, 0.124974f); //-18.063597dB at 64.06% drive
		_CalibrationCurve.SetValue(42, 0.124265f); //-18.113024dB at 65.62% drive
		_CalibrationCurve.SetValue(43, 0.123602f); //-18.159470dB at 67.19% drive
		_CalibrationCurve.SetValue(44, 0.122984f); //-18.203053dB at 68.75% drive
		_CalibrationCurve.SetValue(45, 0.122407f); //-18.243879dB at 70.31% drive
		_CalibrationCurve.SetValue(46, 0.121870f); //-18.282059dB at 71.88% drive
		_CalibrationCurve.SetValue(47, 0.121371f); //-18.317684dB at 73.44% drive
		_CalibrationCurve.SetValue(48, 0.120909f); //-18.350843dB at 75.00% drive
		_CalibrationCurve.SetValue(49, 0.120481f); //-18.381615dB at 76.56% drive
		_CalibrationCurve.SetValue(50, 0.120087f); //-18.410080dB at 78.12% drive
		_CalibrationCurve.SetValue(51, 0.119725f); //-18.436296dB at 79.69% drive
		_CalibrationCurve.SetValue(52, 0.119394f); //-18.460329dB at 81.25% drive
		_CalibrationCurve.SetValue(53, 0.119094f); //-18.482237dB at 82.81% drive
		_CalibrationCurve.SetValue(54, 0.118822f); //-18.502079dB at 84.38% drive
		_CalibrationCurve.SetValue(55, 0.118578f); //-18.519896dB at 85.94% drive
		_CalibrationCurve.SetValue(56, 0.118362f); //-18.535725dB at 87.50% drive
		_CalibrationCurve.SetValue(57, 0.118173f); //-18.549603dB at 89.06% drive
		_CalibrationCurve.SetValue(58, 0.118011f); //-18.561575dB at 90.62% drive
		_CalibrationCurve.SetValue(59, 0.117874f); //-18.571651dB at 92.19% drive
		_CalibrationCurve.SetValue(60, 0.117762f); //-18.579868dB at 93.75% drive
		_CalibrationCurve.SetValue(61, 0.117676f); //-18.586239dB at 95.31% drive
		_CalibrationCurve.SetValue(62, 0.117615f); //-18.590778dB at 96.88% drive
		_CalibrationCurve.SetValue(63, 0.117578f); //-18.593496dB at 98.44% drive
		_CalibrationCurve.SetValue(64, 0.117566f); //-18.594400dB at 100.00% drive
	}

	/*
	*	Callback for when the sample rate changed.
	*/
	FORCE_INLINE void OnSampleRateChanged() NOEXCEPT override
	{
		//Update the pre filters.
		_PreFilters._HighPassFilter.SetSampleRate(_SampleRate);
		_PreFilters._PeakFilter.SetSampleRate(_SampleRate);
		_PreFilters._LowPassFilter.SetSampleRate(_SampleRate);

		//Update the post filters.
		_PostFilters._HighPassFilter.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter1.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter2.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter3.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter4.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter5.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter6.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter7.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter8.SetSampleRate(_SampleRate);
		_PostFilters._LowPassFilter.SetSampleRate(_SampleRate);

		//Update the tone filters.
		_ToneFilters._PeakFilter.SetSampleRate(_SampleRate);
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
		//Put the inputs into the outputs, as all subsequent steps rely on that.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);
		}
			
		//Apply the pre filters.
		{
			_PreFilters._HighPassFilter.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PreFilters._PeakFilter.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PreFilters._LowPassFilter.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
		}

		//Apply the drive.
		{
			//Calculate the gain.
			const float32 gain{ SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(30.0f, 60.0f, BaseMath::InverseSquare(_Drive))) };

			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
				{
					_OverSamplers[channel_index].Feed(outputs->At(channel_index).At(sample_index));

					for (float32 &sample : _OverSamplers[channel_index].GetSamples())
					{
						sample = ApplyDistortion(sample, gain);
					}

					outputs->At(channel_index).At(sample_index) = _OverSamplers[channel_index].DownSample();
				}
			}
		}

		//Apply the post drive filters.
		{
			_PostFilters._HighPassFilter.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter1.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter2.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter3.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter4.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter5.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter6.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter7.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter8.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._LowPassFilter.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
		}

		//Apply the tone filters.
		{
			if (_Tone < 0.5f)
			{
				const float32 tone_scalar{ _Tone * 2.0f };
				_ToneFilters._PeakFilter._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(ToneFilters::START_GAIN, 0.0f, tone_scalar));
			}
			
			else
			{
				const float32 tone_scalar{ (_Tone - 0.5f) * 2.0f };
				_ToneFilters._PeakFilter._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, ToneFilters::END_GAIN, tone_scalar));
			}

			_ToneFilters._PeakFilter._Frequency = BaseMath::LinearlyInterpolate(ToneFilters::START_FREQUENCY, ToneFilters::END_FREQUENCY, _Tone);
			_ToneFilters._PeakFilter._Quality = BaseMath::LinearlyInterpolate(ToneFilters::START_QUALITY, ToneFilters::END_QUALITY, _Tone);

			_ToneFilters._PeakFilter.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
		}

		//Apply the level.
		{
			const float32 level{ _LevelCurve.Sample(_Level) };

			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				SIMD::MultiplyByScalar(outputs->At(channel_index).Data(), number_of_samples, level);
			}
		}

		//Volume calibration to ~21dB RMS.
		{
			const float32 gain{ BaseMath::LinearlyInterpolate(_CalibrationCurve.Sample(0.5f), _CalibrationCurve.Sample(_Drive), 0.5f) };

			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				SIMD::MultiplyByScalar(outputs->At(channel_index).Data(), number_of_samples, gain);
			}
		}
	}

private:

	//Constants.
	constexpr static uint64 CALIBRATION_CURVE_SIZE{ 64 + 1 };

	/*
	*	Pre filters class definition.
	*/
	class PreFilters final
	{

	public:

		//Theigh pass filter.
		HighPassFilter _HighPassFilter{ 800.0f, 1.0f, 1 };

		//The Peak filter.
		PeakFilter _PeakFilter{ 1'000.0f, Audio::DecibelsToGain(3.0f), 0.70710678118f };

		//The low pass filter.
		LowPassFilter _LowPassFilter{ 6'000.0f, 1.0f, 1 };

	};

	/*
	*	Post filters class definition.
	*/
	class PostFilters final
	{

	public:

		//The high pass filter.
		HighPassFilter _HighPassFilter{ 25.0f, 1.0f, 1 };

		//Peak filter 1.
		PeakFilter _PeakFilter1{ 13'172.0f, Audio::DecibelsToGain(-15.13f), 0.380f };

		//Peak filter 2.
		PeakFilter _PeakFilter2{ 188.25f, Audio::DecibelsToGain(2.71f), 0.526f };

		//Peak filter 3.
		PeakFilter _PeakFilter3{ 2'797.2f, Audio::DecibelsToGain(1.30f), 0.964f };

		//Peak filter 4.
		PeakFilter _PeakFilter4{ 3'713.8f, Audio::DecibelsToGain(0.65f), 1.250f };

		//Peak filter 5.
		PeakFilter _PeakFilter5{ 87.205f, Audio::DecibelsToGain(0.42f), 2.500f };

		//Peak filter 6.
		PeakFilter _PeakFilter6{ 436.81f, Audio::DecibelsToGain(0.63f), 0.743f };

		//Peak filter 7.
		PeakFilter _PeakFilter7{ 11'663.0f, Audio::DecibelsToGain(3.04f), 1.305f };

		//Peak filter 8.
		PeakFilter _PeakFilter8{ 1'814.6f, Audio::DecibelsToGain(0.63f), 2.500f };

		//The low pass filter.
		LowPassFilter _LowPassFilter{ 19'750.0f, 1.0f, 1 };

	};

	/*
	*	Tone filters class definition.
	*/
	class ToneFilters final
	{

	public:

		//Constants.
		constexpr static float32 START_FREQUENCY{ 3372.6f };
		constexpr static float32 START_GAIN{ -8.51f };
		constexpr static float32 START_QUALITY{ 0.274f };

		constexpr static float32 END_FREQUENCY{ 4758.9f };
		constexpr static float32 END_GAIN{ 9.70f };
		constexpr static float32 END_QUALITY{ 0.203f };

		//The peak filter.
		PeakFilter _PeakFilter
		{
			BaseMath::LinearlyInterpolate(START_FREQUENCY, END_FREQUENCY, 0.5f),
			Audio::DecibelsToGain(BaseMath::LinearlyInterpolate(START_GAIN, END_GAIN, 0.5f)),
			BaseMath::LinearlyInterpolate(START_QUALITY, END_QUALITY, 0.5f)
		};

	};

	//The pre filters.
	PreFilters _PreFilters;

	//The oversamplers.
	StaticArray<OverSampler<2, 32>, 2> _OverSamplers;

	//The post filters.
	PostFilters _PostFilters;

	//The tone filters.
	ToneFilters _ToneFilters;

	//The level curve.
	Curve<float32, 3> _LevelCurve;

	//The calibration curve.
	Curve<float32, CALIBRATION_CURVE_SIZE> _CalibrationCurve;

	/*
	*	Applies distortion. (:
	*/
	FORCE_INLINE NO_DISCARD float32 ApplyDistortion(const float32 sample, const float32 gain) NOEXCEPT
	{
		const float32 gain_multiplier{ BaseMath::LinearlyInterpolate(0.5f, 2.0f, std::tanh(sample * gain) * 0.5f + 0.5f) };

		return std::tanh(sample * gain * gain_multiplier);
	}

};