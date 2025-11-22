#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>
#include <Audio/Effects/General/HighPassFilter.h>
#include <Audio/Effects/General/LowPassFilter.h>
#include <Audio/Effects/General/PeakFilter.h>
#include <Audio/OverSampler.h>

//Math.
#include <Math/General/Curve.h>

/*
*	This simulates a simple clean amp.
*/
class CleanAmp1 final : public AudioEffect
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
	FORCE_INLINE CleanAmp1() NOEXCEPT
	{
		//Set up the output curve.
		_OutputCurve.SetValue(0, SoundUtilities::DecibelsToGain(-48.0f));
		_OutputCurve.SetValue(1, SoundUtilities::DecibelsToGain(0.0f));
		_OutputCurve.SetValue(2, SoundUtilities::DecibelsToGain(12.0f));

		//Set up the calibration curve.
		_CalibrationCurve.SetValue(0, 6.495035f); //16.251629dB at 0.00% gain
		_CalibrationCurve.SetValue(1, 5.800741f); //15.269669dB at 1.56% gain
		_CalibrationCurve.SetValue(2, 5.191275f); //14.305481dB at 3.12% gain
		_CalibrationCurve.SetValue(3, 4.655542f); //13.359406dB at 4.69% gain
		_CalibrationCurve.SetValue(4, 4.184076f); //12.431992dB at 6.25% gain
		_CalibrationCurve.SetValue(5, 3.768519f); //11.523415dB at 7.81% gain
		_CalibrationCurve.SetValue(6, 3.401775f); //10.634111dB at 9.38% gain
		_CalibrationCurve.SetValue(7, 3.077813f); //9.764845dB at 10.94% gain
		_CalibrationCurve.SetValue(8, 2.791165f); //8.915710dB at 12.50% gain
		_CalibrationCurve.SetValue(9, 2.537246f); //8.087250dB at 14.06% gain
		_CalibrationCurve.SetValue(10, 2.312030f); //7.279869dB at 15.62% gain
		_CalibrationCurve.SetValue(11, 2.112005f); //6.493896dB at 17.19% gain
		_CalibrationCurve.SetValue(12, 1.934110f); //5.729622dB at 18.75% gain
		_CalibrationCurve.SetValue(13, 1.775770f); //4.987736dB at 20.31% gain
		_CalibrationCurve.SetValue(14, 1.634559f); //4.268013dB at 21.88% gain
		_CalibrationCurve.SetValue(15, 1.508518f); //3.571007dB at 23.44% gain
		_CalibrationCurve.SetValue(16, 1.395848f); //2.896763dB at 25.00% gain
		_CalibrationCurve.SetValue(17, 1.294999f); //2.245390dB at 26.56% gain
		_CalibrationCurve.SetValue(18, 1.204601f); //1.616867dB at 28.12% gain
		_CalibrationCurve.SetValue(19, 1.123475f); //1.011271dB at 29.69% gain
		_CalibrationCurve.SetValue(20, 1.050584f); //0.428616dB at 31.25% gain
		_CalibrationCurve.SetValue(21, 0.984965f); //-0.131580dB at 32.81% gain
		_CalibrationCurve.SetValue(22, 0.925825f); //-0.669420dB at 34.38% gain
		_CalibrationCurve.SetValue(23, 0.872442f); //-1.185272dB at 35.94% gain
		_CalibrationCurve.SetValue(24, 0.824209f); //-1.679256dB at 37.50% gain
		_CalibrationCurve.SetValue(25, 0.780529f); //-2.152220dB at 39.06% gain
		_CalibrationCurve.SetValue(26, 0.740942f); //-2.604311dB at 40.62% gain
		_CalibrationCurve.SetValue(27, 0.704987f); //-3.036377dB at 42.19% gain
		_CalibrationCurve.SetValue(28, 0.672289f); //-3.448881dB at 43.75% gain
		_CalibrationCurve.SetValue(29, 0.642524f); //-3.842215dB at 45.31% gain
		_CalibrationCurve.SetValue(30, 0.615367f); //-4.217312dB at 46.88% gain
		_CalibrationCurve.SetValue(31, 0.590557f); //-4.574768dB at 48.44% gain
		_CalibrationCurve.SetValue(32, 0.567864f); //-4.915108dB at 50.00% gain
		_CalibrationCurve.SetValue(33, 0.547082f); //-5.238951dB at 51.56% gain
		_CalibrationCurve.SetValue(34, 0.528017f); //-5.547041dB at 53.12% gain
		_CalibrationCurve.SetValue(35, 0.510509f); //-5.839934dB at 54.69% gain
		_CalibrationCurve.SetValue(36, 0.494415f); //-6.118165dB at 56.25% gain
		_CalibrationCurve.SetValue(37, 0.479607f); //-6.382286dB at 57.81% gain
		_CalibrationCurve.SetValue(38, 0.465972f); //-6.632811dB at 59.38% gain
		_CalibrationCurve.SetValue(39, 0.453405f); //-6.870267dB at 60.94% gain
		_CalibrationCurve.SetValue(40, 0.441817f); //-7.095156dB at 62.50% gain
		_CalibrationCurve.SetValue(41, 0.431133f); //-7.307778dB at 64.06% gain
		_CalibrationCurve.SetValue(42, 0.421273f); //-7.508722dB at 65.62% gain
		_CalibrationCurve.SetValue(43, 0.412177f); //-7.698324dB at 67.19% gain
		_CalibrationCurve.SetValue(44, 0.403789f); //-7.876919dB at 68.75% gain
		_CalibrationCurve.SetValue(45, 0.396060f); //-8.044778dB at 70.31% gain
		_CalibrationCurve.SetValue(46, 0.388941f); //-8.202315dB at 71.88% gain
		_CalibrationCurve.SetValue(47, 0.382395f); //-8.349764dB at 73.44% gain
		_CalibrationCurve.SetValue(48, 0.376378f); //-8.487507dB at 75.00% gain
		_CalibrationCurve.SetValue(49, 0.370870f); //-8.615567dB at 76.56% gain
		_CalibrationCurve.SetValue(50, 0.365830f); //-8.734425dB at 78.12% gain
		_CalibrationCurve.SetValue(51, 0.361236f); //-8.844183dB at 79.69% gain
		_CalibrationCurve.SetValue(52, 0.357064f); //-8.945084dB at 81.25% gain
		_CalibrationCurve.SetValue(53, 0.353299f); //-9.037143dB at 82.81% gain
		_CalibrationCurve.SetValue(54, 0.349914f); //-9.120771dB at 84.38% gain
		_CalibrationCurve.SetValue(55, 0.346899f); //-9.195932dB at 85.94% gain
		_CalibrationCurve.SetValue(56, 0.344236f); //-9.262883dB at 87.50% gain
		_CalibrationCurve.SetValue(57, 0.341917f); //-9.321594dB at 89.06% gain
		_CalibrationCurve.SetValue(58, 0.339922f); //-9.372402dB at 90.62% gain
		_CalibrationCurve.SetValue(59, 0.338253f); //-9.415155dB at 92.19% gain
		_CalibrationCurve.SetValue(60, 0.336896f); //-9.450093dB at 93.75% gain
		_CalibrationCurve.SetValue(61, 0.335848f); //-9.477136dB at 95.31% gain
		_CalibrationCurve.SetValue(62, 0.335102f); //-9.496457dB at 96.88% gain
		_CalibrationCurve.SetValue(63, 0.334658f); //-9.507972dB at 98.44% gain
		_CalibrationCurve.SetValue(64, 0.334509f); //-9.511841dB at 100.00% gain
	}

	/*
	*	Callback for when the sample rate changed.
	*/
	FORCE_INLINE void OnSampleRateChanged() NOEXCEPT override
	{
		//Update the pre filters.
		_PreFilters._PeakFilter1.SetSampleRate(_SampleRate);
		_PreFilters._PeakFilter2.SetSampleRate(_SampleRate);
		_PreFilters._PeakFilter3.SetSampleRate(_SampleRate);
		_PreFilters._PeakFilter4.SetSampleRate(_SampleRate);

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

		//Update the tone filters.
		_ToneFilters._BassFilter1.SetSampleRate(_SampleRate);
		_ToneFilters._BassFilter2.SetSampleRate(_SampleRate);

		_ToneFilters._MidFilter1.SetSampleRate(_SampleRate);
		_ToneFilters._MidFilter2.SetSampleRate(_SampleRate);
		_ToneFilters._MidFilter3.SetSampleRate(_SampleRate);
		_ToneFilters._MidFilter4.SetSampleRate(_SampleRate);

		_ToneFilters._TrebleFilter1.SetSampleRate(_SampleRate);
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
			_PreFilters._PeakFilter1.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PreFilters._PeakFilter2.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PreFilters._PeakFilter3.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
			_PreFilters._PeakFilter4.Process(context, *outputs, outputs, number_of_channels, number_of_samples);
		}

		//Apply the gain.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			//Calculate the gain.
			const float32 gain{ SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, 32.0f, BaseMath::InverseSquare(_Gain))) };

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

		//Peak filter 1.
		PeakFilter _PeakFilter1{ 100.0f, SoundUtilities::DecibelsToGain(-6.0f), 0.70710678118654752440f };

		//Peak filter 2.
		PeakFilter _PeakFilter2{ 300.0f, SoundUtilities::DecibelsToGain(-6.0f), 0.70710678118654752440f };

		//Peak filter 3.
		PeakFilter _PeakFilter3{ 3'000.0f, SoundUtilities::DecibelsToGain(6.0f), 0.70710678118654752440f };

		//Peak filter 4.
		PeakFilter _PeakFilter4{ 10'000.0f, SoundUtilities::DecibelsToGain(-6.0f), 0.70710678118654752440f };

	};

	/*
	*	Post filters class definition.
	*/
	class PostFilters final
	{

	public:

		//Peak filter 1.
		PeakFilter _PeakFilter1{ 1'163.2f, SoundUtilities::DecibelsToGain(-14.79f), 0.487f };

		//Peak filter 2.
		PeakFilter _PeakFilter2{ 24.331f, SoundUtilities::DecibelsToGain(-6.92f), 1.223f };

		//Peak filter 3.
		PeakFilter _PeakFilter3{ 622.11f, SoundUtilities::DecibelsToGain(-4.67f), 1.621f };

		//Peak filter 4.
		PeakFilter _PeakFilter4{ 3'088.2f, SoundUtilities::DecibelsToGain(-3.39f), 1.768f };

		//Peak filter 5.
		PeakFilter _PeakFilter5{ 12'722.0f, SoundUtilities::DecibelsToGain(3.46f), 1.098f };

		//Peak filter 6.
		PeakFilter _PeakFilter6{ 25.546f, SoundUtilities::DecibelsToGain(-3.23f), 1.693f };

		//Peak filter 7.
		PeakFilter _PeakFilter7{ 66.381f, SoundUtilities::DecibelsToGain(2.27f), 2.293f };

		//Peak filter 8.
		PeakFilter _PeakFilter8{ 27.316f, SoundUtilities::DecibelsToGain(-2.14f), 2.102f };

		//Peak filter 9.
		PeakFilter _PeakFilter9{ 3'568.5f, SoundUtilities::DecibelsToGain(-1.20f), 1.250f };

		//Peak filter 10.
		PeakFilter _PeakFilter10{ 351.27f, SoundUtilities::DecibelsToGain(-0.89f), 0.599f };

		//Peak filter 11.
		PeakFilter _PeakFilter11{ 1'293.1f, SoundUtilities::DecibelsToGain(0.94f), 4.204f };

		//Peak filter 12.
		PeakFilter _PeakFilter12{ 66.624f, SoundUtilities::DecibelsToGain(1.09f), 2.500f };

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

		//Mid filter 4.
		PeakFilter _MidFilter4{ 100.0f, 1.0f, 1.0f };

		////////////
		// TREBLE //
		////////////

		//Treble filter 1.
		PeakFilter _TrebleFilter1{ 100.0f, 1.0f, 1.0f };

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
		//Just a simple distortion algorithm will be fine for now, it's not the main purpose of this plugin. (:
		return std::atan(sample * gain);
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

				_ToneFilters._BassFilter1._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(-20.77f, 0.0f, bass_scalar));
				_ToneFilters._BassFilter2._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(2.26f, 0.0f, bass_scalar));
			}

			else
			{
				const float32 bass_scalar{ (_Bass - 0.5f) * 2.0f };

				_ToneFilters._BassFilter1._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, 10.60f, bass_scalar));
				_ToneFilters._BassFilter2._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, -1.90f, bass_scalar));
			}

			_ToneFilters._BassFilter1._Frequency = BaseMath::LinearlyInterpolate(93.340f, 43.507f, _Bass);
			_ToneFilters._BassFilter1._Quality = BaseMath::LinearlyInterpolate(0.487f, 0.625f, _Bass);
			_ToneFilters._BassFilter1.Process(context, inputs, outputs, number_of_channels, number_of_samples);

			_ToneFilters._BassFilter2._Frequency = BaseMath::LinearlyInterpolate(1'125.4f, 695.76f, _Bass);
			_ToneFilters._BassFilter2._Quality = BaseMath::LinearlyInterpolate(1.363f, 1.768f, _Bass);
			_ToneFilters._BassFilter2.Process(context, inputs, outputs, number_of_channels, number_of_samples);
		}

		//Apply mid.
		{
			if (_Mid < 0.5f)
			{
				const float32 mid_scalar{ _Mid * 2.0f };

				_ToneFilters._MidFilter1._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(-2.41f, 0.0f, mid_scalar));
				_ToneFilters._MidFilter2._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(-0.30f, 0.0f, mid_scalar));
				_ToneFilters._MidFilter3._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.21f, 0.0f, mid_scalar));
				_ToneFilters._MidFilter4._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(-0.16f, 0.0f, mid_scalar));
			}

			else
			{
				const float32 mid_scalar{ (_Mid - 0.5f) * 2.0f };

				_ToneFilters._MidFilter1._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, 6.78f, mid_scalar));
				_ToneFilters._MidFilter2._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, -1.48f, mid_scalar));
				_ToneFilters._MidFilter3._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, 2.76f, mid_scalar));
				_ToneFilters._MidFilter4._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, 2.18f, mid_scalar));
			}

			_ToneFilters._MidFilter1._Frequency = BaseMath::LinearlyInterpolate(889.89f, 1'477.5f, _Mid);
			_ToneFilters._MidFilter1._Quality = BaseMath::LinearlyInterpolate(1.250f, 0.097f, _Mid);
			_ToneFilters._MidFilter1.Process(context, inputs, outputs, number_of_channels, number_of_samples);

			_ToneFilters._MidFilter2._Frequency = BaseMath::LinearlyInterpolate(6'861.1f, 4'945.8f, _Mid);
			_ToneFilters._MidFilter2._Quality = BaseMath::LinearlyInterpolate(0.526f, 0.776f, _Mid);
			_ToneFilters._MidFilter2.Process(context, inputs, outputs, number_of_channels, number_of_samples);

			_ToneFilters._MidFilter3._Frequency = BaseMath::LinearlyInterpolate(1'203.8f, 819.37f, _Mid);
			_ToneFilters._MidFilter3._Quality = BaseMath::LinearlyInterpolate(2.973f, 1.768f, _Mid);
			_ToneFilters._MidFilter3.Process(context, inputs, outputs, number_of_channels, number_of_samples);

			_ToneFilters._MidFilter4._Frequency = BaseMath::LinearlyInterpolate(53.843f, 36.264f, _Mid);
			_ToneFilters._MidFilter4._Quality = BaseMath::LinearlyInterpolate(0.625f, 1.146f, _Mid);
			_ToneFilters._MidFilter4.Process(context, inputs, outputs, number_of_channels, number_of_samples);
		}

		//Apply treble.
		{
			if (_Treble < 0.5f)
			{
				const float32 treble_scalar{ _Treble * 2.0f };

				_ToneFilters._TrebleFilter1._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(-15.86f, 0.0f, treble_scalar));
				_ToneFilters._TrebleFilter2._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(2.77f, 0.0f, treble_scalar));
			}

			else
			{
				const float32 treble_scalar{ (_Treble - 0.5f) * 2.0f };

				_ToneFilters._TrebleFilter1._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, 15.79f, treble_scalar));
				_ToneFilters._TrebleFilter2._Gain = SoundUtilities::DecibelsToGain(BaseMath::LinearlyInterpolate(0.0f, -2.55f, treble_scalar));
			}

			_ToneFilters._TrebleFilter1._Frequency = BaseMath::LinearlyInterpolate(7'104.7f, 5538.1f, _Treble);
			_ToneFilters._TrebleFilter1._Quality = BaseMath::LinearlyInterpolate(0.392f, 0.136f, _Treble);
			_ToneFilters._TrebleFilter1.Process(context, inputs, outputs, number_of_channels, number_of_samples);

			_ToneFilters._TrebleFilter2._Frequency = BaseMath::LinearlyInterpolate(530.61f, 159.83f, _Treble);
			_ToneFilters._TrebleFilter2._Quality = BaseMath::LinearlyInterpolate(1.250f, 0.743f, _Treble);
			_ToneFilters._TrebleFilter2.Process(context, inputs, outputs, number_of_channels, number_of_samples);
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
			_ToneFilters._DepthFilter1.Process(context, inputs, outputs, number_of_channels, number_of_samples);
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
			_ToneFilters._PresenceFilter1.Process(context, inputs, outputs, number_of_channels, number_of_samples);
		}
	}

};