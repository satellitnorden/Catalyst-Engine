#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//Audio.
#include <Audio/Core/Loudness.h>
#include <Audio/Effects/Core/AudioEffect.h>

/*
*	Simple utility class for calibrating an audio effect, for example if you have a distortion knob that changes the output volume of the effect,
*	this can be used to measure the output gain and generate an output curve for that parameter.
*/
class AudioEffectCalibrator final
{

public:

	/*
	*	Pass class definition.
	*/
	class Pass final
	{

	public:

		//The name.
		const char *RESTRICT _Name;

		//The number of sample points on the output curve.
		uint32 _NumberOfSamplePoints;

		//The update function.
		void (*_Update)(AudioEffect *const RESTRICT effect, const uint32 sample_point_index);

	};

	/*
	*	Sets the audio effect.
	*/
	FORCE_INLINE void SetEffect(AudioEffect *const RESTRICT effect) NOEXCEPT
	{
		_Effect = effect;
	}

	/*
	*	Sets the target gain.
	*/
	FORCE_INLINE void SetTargetGain(const float32 target_gain) NOEXCEPT
	{
		_TargetGain = target_gain;
	}

	/*
	*	Sets the test signal file path.
	*/
	FORCE_INLINE void SetTestSignalFilePath(const char *const RESTRICT test_signal_file_path) NOEXCEPT
	{
		_TestSignalFilePath = test_signal_file_path;
	}

	/*
	*	Sets the passes.
	*/
	FORCE_INLINE void SetPasses(const Pass *const RESTRICT passes, const uint32 number_of_passes) NOEXCEPT
	{
		_Passes = passes;
		_NumberOfPasses = number_of_passes;
	}

	/*
	*	Calibrates!
	*/
	FORCE_INLINE void Calibrate() NOEXCEPT
	{
		//Load the test signal.
		DynamicArray<float32> test_signal;
		float32 sample_rate;

		{
			AudioFile<float32> audio_file;

			if (!audio_file.load(_TestSignalFilePath.Data()))
			{
				ASSERT(false, "Couldn't load audio file!");
				return;
			}

			test_signal.Upsize<false>(audio_file.samples[0].size());
			Memory::Copy(test_signal.Data(), audio_file.samples[0].data(), sizeof(float32) * audio_file.samples[0].size());

			sample_rate = static_cast<float32>(audio_file.getSampleRate());
		}

		//Set up the input/output buffers.
		DynamicArray<DynamicArray<float32>> inputs;
		inputs.Upsize<true>(1);
		inputs[0].Upsize<false>(test_signal.Size());
		Memory::Copy(inputs[0].Data(), test_signal.Data(), sizeof(float32) * test_signal.Size());

		DynamicArray<DynamicArray<float32>> outputs;
		outputs.Upsize<true>(1);
		outputs[0].Upsize<false>(test_signal.Size());

		//Set the sample rate on the effect.
		_Effect->SetSampleRate(sample_rate);

		//Do all passes.
		for (uint32 pass_index{ 0 }; pass_index < _NumberOfPasses; ++pass_index)
		{
			//Iterate over all sample points.
			for (uint32 sample_point_index{ 0 }; sample_point_index < _Passes[pass_index]._NumberOfSamplePoints; ++sample_point_index)
			{
				//Call the update function.
				_Passes[pass_index]._Update(_Effect, sample_point_index);

				//Reset the output buffer.
				Memory::Set(outputs[0].Data(), 0, sizeof(float32) * outputs[0].Size());

				//Process!
				AudioProcessContext context;

				_Effect->Process(context, inputs, &outputs, 1, static_cast<uint32>(outputs[0].Size()));

				//Measure the loudness.
				const float32 loudness{ Loudness::Measure(sample_rate, outputs[0].Data(), outputs[0].Size()) };

				//Calculate the compensation.
				const float32 compensation{ _TargetGain - loudness };

				//Calculate the compensation gain.
				const float32 compensation_gain{ Audio::DecibelsToGain(compensation) };

				//Log!
				char buffer[256];
				sprintf_s(buffer, "_%sOutputCurve.SetValue(%llu, %ff); //%fdB\n", _Passes[pass_index]._Name, sample_point_index, compensation_gain, compensation);

				PRINT_TO_OUTPUT(buffer);
			}
		}
	}

private:

	//The effect.
	AudioEffect *RESTRICT _Effect{ nullptr };

	//The target gain.
	float32 _TargetGain{ 1.0f };

	//The test signal file path.
	DynamicString _TestSignalFilePath;

	//The passes.
	const Pass *RESTRICT _Passes{ nullptr };

	//The number of passes.
	uint32 _NumberOfPasses{ 0 };

};