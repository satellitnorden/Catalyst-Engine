#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

/*
*	Class representing a single neuron in a neural network.
*/
class Neuron final
{

public:

	//Type aliases.
	using ActivationFunction = float32(*)(const float32 X);

	//The neuron index.
	uint64 _NeuronIndex;

	//The weights.
	DynamicArray<float32> _Weights;

	//The deltas.
	DynamicArray<float32> _Deltas;

	//The output value.
	float32 _OutputValue;

	//The gradient.
	float32 _Gradient;

	/*
	*	Sets the number of outputs.
	*/
	FORCE_INLINE void SetNumberOfOutputs(const uint64 number_of_outputs) NOEXCEPT
	{
		//Initialize the weights.
		_Weights.Upsize<false>(number_of_outputs);

		for (float32 &weight : _Weights)
		{
			weight = CatalystRandomMath::RandomFloat();
		}

		//Initialize the deltas.
		_Deltas.Upsize<false>(number_of_outputs);

		for (float32 &delta : _Deltas)
		{
			delta = 0.0f;
		}
	}

	/*
	*	Feeds forward.
	*/
	FORCE_INLINE void FeedForward(const DynamicArray<Neuron> &previous_layer_neurons, const ActivationFunction activation_function) NOEXCEPT
	{
		//Calculate the sum.
		float32 sum{ 0.0f };

		for (const Neuron &neuron : previous_layer_neurons)
		{
			sum += neuron._OutputValue * neuron._Weights[_NeuronIndex];
		}

		//Calculate the output value.
		_OutputValue = activation_function(sum);
	}

	/*
	*	Calculates the output gradient.
	*/
	FORCE_INLINE void CalculateOutputGradient(const float32 target_value) NOEXCEPT
	{
		//Calculate the gradient.
		_Gradient = (target_value - _OutputValue) * Derivative(_OutputValue);
	}

	/*
	*	Calculates the hidden gradient.
	*/
	FORCE_INLINE void CalculateHiddenGradient(const DynamicArray<Neuron> &next_layer_neurons) NOEXCEPT
	{
		//Calculate the DOW.
		float32 DOW{ 0.0f };

		for (uint64 i{ 0 }, size{ next_layer_neurons.Size() }; i < size; ++i)
		{
			DOW += _Weights[i] * next_layer_neurons[i]._Gradient;
		}

		//Calculate the gradient.
		_Gradient = DOW * Derivative(_OutputValue);
	}

	/*
	*	Updates the input weights.
	*/
	FORCE_INLINE void UpdateInputWeights(const float32 learning_rate, const float32 momentum, DynamicArray<Neuron> &previous_layer_neurons) NOEXCEPT
	{
		for (uint64 i{ 0 }, size{ previous_layer_neurons.Size() }; i < size; ++i)
		{
			Neuron &neuron{ previous_layer_neurons[i] };

			const float32 old_delta{ neuron._Deltas[_NeuronIndex] };
			const float32 new_delta{ learning_rate * neuron._OutputValue * _Gradient + momentum * old_delta };

			neuron._Deltas[_NeuronIndex] = new_delta;
			neuron._Weights[_NeuronIndex] += new_delta;
		}
	}

	/*
	*	Calculates the derivative.
	*/
	FORCE_INLINE NO_DISCARD float32 Derivative(const float32 value) NOEXCEPT
	{
		return 1.0f - (value * value);
	}

};