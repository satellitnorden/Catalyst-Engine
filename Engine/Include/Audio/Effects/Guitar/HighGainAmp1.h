#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>
#include <Audio/Effects/General/HighPassFilter.h>
#include <Audio/Effects/General/HighShelfFilter.h>
#include <Audio/Effects/General/LowPassFilter.h>
#include <Audio/Effects/General/PeakFilter.h>
#include <Audio/OverSampler.h>

//Math.
#include <Math/General/Curve.h>

/*
*	This simulates a high gain amp, similar in style to 5150's.
*/
class HighGainAmp1 final : public AudioEffect
{

public:

	//The gain.
	float32 _Gain{ 0.5f };

	//The bass.
	float32 _Bass{ 0.5f };

	//The mid.
	float32 _Mid{ 0.5f };

	//The treble.
	float32 _Treble{ 0.5f };

	//The treble.
	float32 _Depth{ 0.5f };

	//The presence.
	float32 _Presence{ 0.5f };

	//The output.
	float32 _Output{ 0.5f };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE HighGainAmp1() NOEXCEPT
	{
		//Set up the output curve.
		_OutputCurve.SetValue(0, Audio::DecibelsToGain(-48.0f));
		_OutputCurve.SetValue(1, Audio::DecibelsToGain(0.0f));
		_OutputCurve.SetValue(2, Audio::DecibelsToGain(12.0f));

		//Set up the calibration curve.
		_CalibrationCurve.SetValue(0, 0.128799f); //-17.801769dB at 0.00% gain
		_CalibrationCurve.SetValue(1, 0.121810f); //-18.286316dB at 1.56% gain
		_CalibrationCurve.SetValue(2, 0.115620f); //-18.739363dB at 3.12% gain
		_CalibrationCurve.SetValue(3, 0.110123f); //-19.162462dB at 4.69% gain
		_CalibrationCurve.SetValue(4, 0.105229f); //-19.557325dB at 6.25% gain
		_CalibrationCurve.SetValue(5, 0.100859f); //-19.925732dB at 7.81% gain
		_CalibrationCurve.SetValue(6, 0.096945f); //-20.269459dB at 9.38% gain
		_CalibrationCurve.SetValue(7, 0.093431f); //-20.590145dB at 10.94% gain
		_CalibrationCurve.SetValue(8, 0.090268f); //-20.889292dB at 12.50% gain
		_CalibrationCurve.SetValue(9, 0.087416f); //-21.168179dB at 14.06% gain
		_CalibrationCurve.SetValue(10, 0.084839f); //-21.428049dB at 15.62% gain
		_CalibrationCurve.SetValue(11, 0.082509f); //-21.670008dB at 17.19% gain
		_CalibrationCurve.SetValue(12, 0.080397f); //-21.895174dB at 18.75% gain
		_CalibrationCurve.SetValue(13, 0.078482f); //-22.104630dB at 20.31% gain
		_CalibrationCurve.SetValue(14, 0.076741f); //-22.299480dB at 21.88% gain
		_CalibrationCurve.SetValue(15, 0.075155f); //-22.480793dB at 23.44% gain
		_CalibrationCurve.SetValue(16, 0.073709f); //-22.649565dB at 25.00% gain
		_CalibrationCurve.SetValue(17, 0.072388f); //-22.806726dB at 26.56% gain
		_CalibrationCurve.SetValue(18, 0.071177f); //-22.953144dB at 28.12% gain
		_CalibrationCurve.SetValue(19, 0.070068f); //-23.089663dB at 29.69% gain
		_CalibrationCurve.SetValue(20, 0.069048f); //-23.217003dB at 31.25% gain
		_CalibrationCurve.SetValue(21, 0.068110f); //-23.335833dB at 32.81% gain
		_CalibrationCurve.SetValue(22, 0.067245f); //-23.446764dB at 34.38% gain
		_CalibrationCurve.SetValue(23, 0.066448f); //-23.550371dB at 35.94% gain
		_CalibrationCurve.SetValue(24, 0.065712f); //-23.647110dB at 37.50% gain
		_CalibrationCurve.SetValue(25, 0.065032f); //-23.737495dB at 39.06% gain
		_CalibrationCurve.SetValue(26, 0.064403f); //-23.821939dB at 40.62% gain
		_CalibrationCurve.SetValue(27, 0.063820f); //-23.900812dB at 42.19% gain
		_CalibrationCurve.SetValue(28, 0.063281f); //-23.974503dB at 43.75% gain
		_CalibrationCurve.SetValue(29, 0.062782f); //-24.043358dB at 45.31% gain
		_CalibrationCurve.SetValue(30, 0.062318f); //-24.107670dB at 46.88% gain
		_CalibrationCurve.SetValue(31, 0.061889f); //-24.167753dB at 48.44% gain
		_CalibrationCurve.SetValue(32, 0.061490f); //-24.223867dB at 50.00% gain
		_CalibrationCurve.SetValue(33, 0.061120f); //-24.276295dB at 51.56% gain
		_CalibrationCurve.SetValue(34, 0.060777f); //-24.325254dB at 53.12% gain
		_CalibrationCurve.SetValue(35, 0.060458f); //-24.370983dB at 54.69% gain
		_CalibrationCurve.SetValue(36, 0.060161f); //-24.413673dB at 56.25% gain
		_CalibrationCurve.SetValue(37, 0.059886f); //-24.453556dB at 57.81% gain
		_CalibrationCurve.SetValue(38, 0.059630f); //-24.490751dB at 59.38% gain
		_CalibrationCurve.SetValue(39, 0.059392f); //-24.525452dB at 60.94% gain
		_CalibrationCurve.SetValue(40, 0.059171f); //-24.557802dB at 62.50% gain
		_CalibrationCurve.SetValue(41, 0.058966f); //-24.587957dB at 64.06% gain
		_CalibrationCurve.SetValue(42, 0.058776f); //-24.616035dB at 65.62% gain
		_CalibrationCurve.SetValue(43, 0.058599f); //-24.642149dB at 67.19% gain
		_CalibrationCurve.SetValue(44, 0.058436f); //-24.666428dB at 68.75% gain
		_CalibrationCurve.SetValue(45, 0.058284f); //-24.688942dB at 70.31% gain
		_CalibrationCurve.SetValue(46, 0.058144f); //-24.709839dB at 71.88% gain
		_CalibrationCurve.SetValue(47, 0.058016f); //-24.729116dB at 73.44% gain
		_CalibrationCurve.SetValue(48, 0.057897f); //-24.746944dB at 75.00% gain
		_CalibrationCurve.SetValue(49, 0.057787f); //-24.763344dB at 76.56% gain
		_CalibrationCurve.SetValue(50, 0.057687f); //-24.778395dB at 78.12% gain
		_CalibrationCurve.SetValue(51, 0.057596f); //-24.792168dB at 79.69% gain
		_CalibrationCurve.SetValue(52, 0.057513f); //-24.804707dB at 81.25% gain
		_CalibrationCurve.SetValue(53, 0.057438f); //-24.816063dB at 82.81% gain
		_CalibrationCurve.SetValue(54, 0.057370f); //-24.826284dB at 84.38% gain
		_CalibrationCurve.SetValue(55, 0.057310f); //-24.835415dB at 85.94% gain
		_CalibrationCurve.SetValue(56, 0.057257f); //-24.843466dB at 87.50% gain
		_CalibrationCurve.SetValue(57, 0.057210f); //-24.850525dB at 89.06% gain
		_CalibrationCurve.SetValue(58, 0.057171f); //-24.856552dB at 90.62% gain
		_CalibrationCurve.SetValue(59, 0.057137f); //-24.861654dB at 92.19% gain
		_CalibrationCurve.SetValue(60, 0.057110f); //-24.865770dB at 93.75% gain
		_CalibrationCurve.SetValue(61, 0.057089f); //-24.868977dB at 95.31% gain
		_CalibrationCurve.SetValue(62, 0.057074f); //-24.871244dB at 96.88% gain
		_CalibrationCurve.SetValue(63, 0.057065f); //-24.872620dB at 98.44% gain
		_CalibrationCurve.SetValue(64, 0.057062f); //-24.873066dB at 100.00% gain

		//Initialize the SIMD backend.
		SIMD::InitializeBackend();
	}

	/*
	*	Callback for when the sample rate changed.
	*/
	FORCE_INLINE void OnSampleRateChanged() NOEXCEPT override
	{
		//Update the pre filters.
		_PreFilters._HighPassFilter.SetSampleRate(_SampleRate);
		_PreFilters._PeakFilter1.SetSampleRate(_SampleRate);
		_PreFilters._PeakFilter2.SetSampleRate(_SampleRate);
		_PreFilters._PeakFilter3.SetSampleRate(_SampleRate);
		_PreFilters._PeakFilter4.SetSampleRate(_SampleRate);
		_PreFilters._LowPassFilter.SetSampleRate(_SampleRate);

		//Update the post filters.
		_PostFilters._PeakFilter1.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter2.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter3.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter4.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter5.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter6.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter7.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter8.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter9.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter10.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter11.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter12.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter13.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter14.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter15.SetSampleRate(_SampleRate);
		_PostFilters._PeakFilter16.SetSampleRate(_SampleRate);

		//Update the tone filters.
		_ToneFilters._BassFilter1.SetSampleRate(_SampleRate);
		_ToneFilters._BassFilter2.SetSampleRate(_SampleRate);

		_ToneFilters._MidFilter1.SetSampleRate(_SampleRate);
		_ToneFilters._MidFilter2.SetSampleRate(_SampleRate);
		_ToneFilters._MidFilter3.SetSampleRate(_SampleRate);

		_ToneFilters._TrebleFilter11.SetSampleRate(_SampleRate);
		_ToneFilters._TrebleFilter12.SetSampleRate(_SampleRate);
		_ToneFilters._TrebleFilter2.SetSampleRate(_SampleRate);

		_ToneFilters._DepthFilter1.SetSampleRate(_SampleRate);

		_ToneFilters._PresenceFilter1.SetSampleRate(_SampleRate);
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
		//Apply the pre filters.
		{
			_PreFilters._HighPassFilter.Process(context, inputs, outputs, number_of_channels, number_of_samples);

			_PreFilters._PeakFilter1.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PreFilters._PeakFilter2.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PreFilters._PeakFilter3.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PreFilters._PeakFilter4.Process(context, *outputs, outputs, number_of_channels, number_of_samples);

			_PreFilters._LowPassFilter.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
		}

		//Apply the gain.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			//Calculate the gain.
			const float32 gain{ SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(48.0f, 96.0f, BaseMath::InverseSquare(_Gain))) };

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

		//Apply the post filters.
		{
			_PostFilters._PeakFilter1.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter2.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter3.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter4.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter5.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter6.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter7.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter8.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter9.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter10.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter11.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter12.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter13.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter14.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter15.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PostFilters._PeakFilter16.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
		}

		//Apply th tone filters.
		ApplyToneFilters(context, *outputs, outputs, number_of_channels, number_of_samples);

		//Apply the output.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			const float32 output{ _OutputCurve.Sample(_Output) };
			SIMD::MultiplyByScalar(outputs->At(channel_index).Data(), number_of_samples, output);
		}

		//Volume calibration to ~21dB RMS.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			const float32 gain{ BaseMath::LinearlyInterpolate(_CalibrationCurve.Sample(0.5f), _CalibrationCurve.Sample(_Gain), 0.125f) };
			SIMD::MultiplyByScalar(outputs->At(channel_index).Data(), number_of_samples, gain);
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

		//The high pass filter.
		HighPassFilter _HighPassFilter{ 400.0f, HighPassFilter::DEFAULT_QUALITY, 2 };

		//Peak filter 1.
		PeakFilter _PeakFilter1{ 250.0f, Audio::DecibelsToGain(-12.0f), 1.0f };

		//Peak filter 2.
		PeakFilter _PeakFilter2{ 3'500.0f, Audio::DecibelsToGain(-6.0f), 1.0f };

		//Peak filter 3.
		PeakFilter _PeakFilter3{ 500.0f, Audio::DecibelsToGain(-3.0f), 1.0f };

		//Peak filter 4.
		PeakFilter _PeakFilter4{ 1'250.0f, Audio::DecibelsToGain(3.0f), 1.0f };

		//The low pass filter.
		LowPassFilter _LowPassFilter{ 10'000.0f, LowPassFilter::DEFAULT_QUALITY, 2 };

	};

	/*
	*	Post filters class definition.
	*/
	class PostFilters final
	{

	public:

		//Peak filter 1.
		PeakFilter _PeakFilter1{ 14'679.0f, Audio::DecibelsToGain(6.88f), 1.250f };

		//Peak filter 2.
		PeakFilter _PeakFilter2{ 85.411f, Audio::DecibelsToGain(6.14f), 0.537f };

		//Peak filter 3.
		PeakFilter _PeakFilter3{ 652.91f, Audio::DecibelsToGain(-4.65f), 1.029f };

		//Peak filter 4.
		PeakFilter _PeakFilter4{ 215.30f, Audio::DecibelsToGain(3.02f), 2.500f };

		//Peak filter 5.
		PeakFilter _PeakFilter5{ 37.589f, Audio::DecibelsToGain(1.96f), 1.768f };

		//Peak filter 6.
		PeakFilter _PeakFilter6{ 826.27f, Audio::DecibelsToGain(-1.36f), 0.811f };

		//Peak filter 7.
		PeakFilter _PeakFilter7{ 412.22f, Audio::DecibelsToGain(-1.39f), 3.536f };

		//Peak filter 8.
		PeakFilter _PeakFilter8{ 4'335.3f, Audio::DecibelsToGain(1.04f), 0.156f };

		//Peak filter 9.
		PeakFilter _PeakFilter9{ 1'210.7f, Audio::DecibelsToGain(-1.20f), 2.973f };

		//Peak filter 10.
		PeakFilter _PeakFilter10{ 130.56f, Audio::DecibelsToGain(0.85f), 2.500f };

		//Peak filter 11.
		PeakFilter _PeakFilter11{ 17'450.0f, Audio::DecibelsToGain(1.43f), 3.536f };

		//Peak filter 12.
		PeakFilter _PeakFilter12{ 11'691.0f, Audio::DecibelsToGain(-1.19f), 2.500f };

		//Peak filter 13.
		PeakFilter _PeakFilter13{ 440.26f, Audio::DecibelsToGain(-0.75f), 2.973f };

		//Peak filter 14.
		PeakFilter _PeakFilter14{ 268.06f, Audio::DecibelsToGain(0.93f), 7.071f };

		//Peak filter 15.
		PeakFilter _PeakFilter15{ 57.872f, Audio::DecibelsToGain(0.25f), 0.442f };

		//Peak filter 16.
		PeakFilter _PeakFilter16{ 1241.9f, Audio::DecibelsToGain(-0.35f), 1.768f };

	};

	/*
	*	Tone filters class definition.
	*/
	class ToneFilters final
	{

	public:

		//////////
		// BASS //
		//////////

		//Bass filter 1.
		PeakFilter _BassFilter1{ 100.0f, 1.0f, 1.0f };

		//Bass filter 2.
		PeakFilter _BassFilter2{ 100.0f, 1.0f, 1.0f };

		/////////
		// MID //
		/////////

		//Mid filter 1.
		PeakFilter _MidFilter1{ 100.0f, 1.0f, 1.0f };

		//Mid filter 2.
		PeakFilter _MidFilter2{ 100.0f, 1.0f, 1.0f };

		//Mid filter 3.
		PeakFilter _MidFilter3{ 100.0f, 1.0f, 1.0f };

		////////////
		// TREBLE //
		////////////

		//Treble filter 1.1.
		PeakFilter _TrebleFilter11{ 16'716.0f, 1.0f, 0.118f };

		//Treble filter 1.2.
		HighShelfFilter _TrebleFilter12{ 859.69f, 1.0f };

		//Treble filter 2.
		PeakFilter _TrebleFilter2{ 100.0f, 1.0f, 1.0f };

		///////////
		// DEPTH //
		///////////

		//Depth filter 1.
		PeakFilter _DepthFilter1{ 100.0f, 1.0f, 1.0f };

		//////////////
		// PRESENCE //
		//////////////

		//Presence filter 1.
		PeakFilter _PresenceFilter1{ 100.0f, 1.0f, 1.0f };

	};

	//The pre filters.
	PreFilters _PreFilters;

	//The oversamplers.
	StaticArray<OverSampler<2, 32>, 2> _OverSamplers;

	//The post filters.
	PostFilters _PostFilters;

	//The tone filters.
	ToneFilters _ToneFilters;

	//The output curve.
	Curve<float32, 3> _OutputCurve;

	//The calibration curve.
	Curve<float32, CALIBRATION_CURVE_SIZE> _CalibrationCurve;

	/*
	*	Applies distortion. (:
	*	https://www.desmos.com/calculator/dzpujomgss
	*/
	FORCE_INLINE NO_DISCARD float32 ApplyDistortion(const float32 sample, const float32 gain) NOEXCEPT
	{
		//Calculate the sign multiplier.
		const float32 sign_multiplier{ std::tanh(sample * gain) };

		//Calculate the gain multipliers.
		const float32 gain_multiplier_1{ BaseMath::LinearlyInterpolate(0.5f, 2.0f, BaseMath::Absolute<float32>(sign_multiplier)) };
		const float32 gain_multiplier_2{ BaseMath::LinearlyInterpolate(0.25f, 4.0f, sign_multiplier * 0.5f + 0.5f) };

		//Calculate the different distortions.
		const float32 distortion_1{ std::atan(sample * gain * gain_multiplier_1 * gain_multiplier_2) };
		const float32 distortion_2{ std::tanh(sample * sample * sample * sample * (gain - 1.0f) * gain_multiplier_1 * gain_multiplier_2 * (sample >= 0.0f ? 1.0f : -1.0f) * 0.25f) * (BaseMathConstants::HALF_PI - 1) };

		//Return the final blend!
		return distortion_1 - distortion_2;
	}

	/*
	*	Applies the tone filters.
	*/
	FORCE_INLINE void ApplyToneFilters
	(
		const AudioProcessContext &context,
		const DynamicArray<DynamicArray<float32>> &inputs,
		DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
		const uint8 number_of_channels,
		const uint32 number_of_samples
	) NOEXCEPT
	{
		//Apply bass.
		{
			if (_Bass < 0.5f)
			{
				const float32 bass_scalar{ _Bass * 2.0f };

				_ToneFilters._BassFilter1._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(-8.82f, 0.0f, bass_scalar));
				_ToneFilters._BassFilter2._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(1.30f, 0.0f, bass_scalar));
			}

			else
			{
				const float32 bass_scalar{ (_Bass - 0.5f) * 2.0f };

				_ToneFilters._BassFilter1._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, 7.83f, bass_scalar));
				_ToneFilters._BassFilter2._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, -0.96f, bass_scalar));
			}

			_ToneFilters._BassFilter1._Frequency = BaseMath::LinearlyInterpolate(20.011f, 15.639f, _Bass);
			_ToneFilters._BassFilter1._Quality = BaseMath::LinearlyInterpolate(0.153f, 0.372f, _Bass);
			_ToneFilters._BassFilter1.Process(context, *outputs, outputs, number_of_channels, number_of_samples);

			_ToneFilters._BassFilter2._Frequency = BaseMath::LinearlyInterpolate(808.55f, 394.12f, _Bass);
			_ToneFilters._BassFilter2._Quality = BaseMath::LinearlyInterpolate(1.250f, 1.250f, _Bass);
			_ToneFilters._BassFilter2.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
		}

		//Apply mid.
		{
			if (_Mid < 0.5f)
			{
				const float32 mid_scalar{ _Mid * 2.0f };

				_ToneFilters._MidFilter1._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(-3.30f, 0.0f, mid_scalar));
				_ToneFilters._MidFilter2._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(-3.16f, 0.0f, mid_scalar));
				_ToneFilters._MidFilter3._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.94f, 0.0f, mid_scalar));
			}

			else
			{
				const float32 mid_scalar{ (_Mid - 0.5f) * 2.0f };

				_ToneFilters._MidFilter1._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, 1.84f, mid_scalar));
				_ToneFilters._MidFilter2._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, 1.22f, mid_scalar));
				_ToneFilters._MidFilter3._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, -0.24f, mid_scalar));
			}

			_ToneFilters._MidFilter1._Frequency = BaseMath::LinearlyInterpolate(11'157.0f, 2'243.2f, _Mid);
			_ToneFilters._MidFilter1._Quality = BaseMath::LinearlyInterpolate(0.034f, 0.050f, _Mid);
			_ToneFilters._MidFilter1.Process(context, *outputs, outputs, number_of_channels, number_of_samples);

			_ToneFilters._MidFilter2._Frequency = BaseMath::LinearlyInterpolate(740.30f, 674.11f, _Mid);
			_ToneFilters._MidFilter2._Quality = BaseMath::LinearlyInterpolate(1.928f, 1.250f, _Mid);
			_ToneFilters._MidFilter2.Process(context, *outputs, outputs, number_of_channels, number_of_samples);

			_ToneFilters._MidFilter3._Frequency = BaseMath::LinearlyInterpolate(10'607.0f, 7'500.0f, _Mid);
			_ToneFilters._MidFilter3._Quality = BaseMath::LinearlyInterpolate(0.372f, 0.625f, _Mid);
			_ToneFilters._MidFilter3.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
		}

		//Apply treble.
		{
			if (_Treble < 0.5f)
			{
				const float32 treble_scalar{ _Treble * 2.0f };

				_ToneFilters._TrebleFilter11._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(-7.50f, 0.0f, treble_scalar));
				_ToneFilters._TrebleFilter11.Process(context, *outputs, outputs, number_of_channels, number_of_samples);

				_ToneFilters._TrebleFilter2._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(1.01f, 0.0f, treble_scalar));
			}

			else
			{
				const float32 treble_scalar{ (_Treble - 0.5f) * 2.0f };

				_ToneFilters._TrebleFilter12._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, 4.23f, treble_scalar));
				_ToneFilters._TrebleFilter12.Process(context, *outputs, outputs, number_of_channels, number_of_samples);

				_ToneFilters._TrebleFilter2._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, -0.83f, treble_scalar));
			}

			_ToneFilters._TrebleFilter2._Frequency = BaseMath::LinearlyInterpolate(19.870f, 19.870f, _Treble);
			_ToneFilters._TrebleFilter2._Quality = BaseMath::LinearlyInterpolate(0.043f, 0.055f, _Treble);
			_ToneFilters._TrebleFilter2.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
		}

		//Apply depth.
		{
			if (_Depth < 0.5f)
			{
				const float32 depth_scalar{ _Depth * 2.0f };

				_ToneFilters._DepthFilter1._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(-4.81f, 0.0f, depth_scalar));
			}

			else
			{
				const float32 depth_scalar{ (_Depth - 0.5f) * 2.0f };

				_ToneFilters._DepthFilter1._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, 6.85f, depth_scalar));
			}

			_ToneFilters._DepthFilter1._Frequency = BaseMath::LinearlyInterpolate(34.037f, 77.379f, _Depth);
			_ToneFilters._DepthFilter1._Quality = BaseMath::LinearlyInterpolate(0.055f, 0.776f, _Depth);
			_ToneFilters._DepthFilter1.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
		}

		//Apply presence.
		{
			if (_Presence < 0.5f)
			{
				const float32 presence_scalar{ _Presence * 2.0f };

				_ToneFilters._PresenceFilter1._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(-0.69f, 0.0f, presence_scalar));
			}

			else
			{
				const float32 presence_scalar{ (_Presence - 0.5f) * 2.0f };

				_ToneFilters._PresenceFilter1._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, 14.75f, presence_scalar));
			}

			_ToneFilters._PresenceFilter1._Frequency = BaseMath::LinearlyInterpolate(1'325.8f, 4'592.3f, _Presence);
			_ToneFilters._PresenceFilter1._Quality = BaseMath::LinearlyInterpolate(0.110f, 0.561f, _Presence);
			_ToneFilters._PresenceFilter1.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
		}
	}

};