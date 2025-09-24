#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

/*
*	Class encapsulating a single delay line.
*	Keeps a circular buffer that it takes from with the given delay time.
*/
class DelayLine final
{

public:

	/*
	*	Initializes this delay line.
	*/
	FORCE_INLINE void Initialize(const float32 delay_time, const float32 sample_rate) NOEXCEPT
	{
		//Set the delay time.
		_DelayTime = delay_time;

		//Calculate the buffer size.
		const uint64 buffer_size{ static_cast<uint64>(_DelayTime * sample_rate) + 1 };

		//Initialize the buffer.
		_Buffer.Upsize<false>(buffer_size);
		Memory::Set(_Buffer.Data(), 0, sizeof(float32) * buffer_size);
	}

	/*
	*	Updates this delay line with a new delay time.
	*/
	FORCE_INLINE void Update(const float32 delay_time, const float32 sample_rate) NOEXCEPT
	{
		//Set the delay time.
		_DelayTime = delay_time;

		//Calculate the buffer size.
		const uint64 buffer_size{ static_cast<uint64>(_DelayTime * sample_rate) + 1 };

		//Resize the buffer.
		_Buffer.Resize<true>(buffer_size);

		//Update the current index.
		_CurrentIndex = _CurrentIndex % _Buffer.Size();
	}

	/*
	*	Returns the delay time.
	*/
	FORCE_INLINE NO_DISCARD float32 GetDelayTime() const NOEXCEPT
	{
		return _DelayTime;
	}

	/*
	*	Processes this delay line.
	*/
	FORCE_INLINE void Process(const float32 *const RESTRICT inputs, float32 *const RESTRICT outputs, const uint32 number_of_samples) NOEXCEPT
	{
		for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			outputs[sample_index] = ProcessSingle(inputs[sample_index]);
		}
	}

	/*
	*	Processes this delay line with a single sample.
	*/
	FORCE_INLINE NO_DISCARD float32 ProcessSingle(const float32 input_sample) NOEXCEPT
	{
		//Retrieve the output.
		const float32 output{ _Buffer[_CurrentIndex] };

		//Write the input.
		_Buffer[_CurrentIndex] = input_sample;

		//Update the current index.
		++_CurrentIndex;
		_CurrentIndex -= _Buffer.Size() * static_cast<uint64>(_CurrentIndex >= _Buffer.Size());

		//Return the output.
		return output;
	}

	/*
	*	Returns the current sample.
	*/
	FORCE_INLINE NO_DISCARD float32 CurrentSample() const NOEXCEPT
	{
		return _Buffer[_CurrentIndex];
	}

private:

	//The delay time.
	float32 _DelayTime;

	//The buffer.
	DynamicArray<float32> _Buffer;

	//The current index.
	uint64 _CurrentIndex{ 0 };

};