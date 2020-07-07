#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/MachineLearning/Neuron.h>

/*
*	Class representing one neural network, with as many input/intermediate/output neurons as the user specifies.
*	Can also load/save training data to disk, to be able to load this data in when needed.
*/
class NeuralNetwork final
{

public:

	/*
	*	Sets the learning rate.
	*/
	FORCE_INLINE void SetLearningRate(const float32 value) NOEXCEPT
	{
		_LearningRate = value;
	}

	/*
	*	Sets the momentum.
	*/
	FORCE_INLINE void SetMomentum(const float32 value) NOEXCEPT
	{
		_Momentum = value;
	}

	/*
	*	Adds an input neuron.
	*/
	FORCE_INLINE void AddInputNeuron() NOEXCEPT
	{
		_InputLayer._Neurons.Emplace();
		Neuron &neuron{ _InputLayer._Neurons.Back() };

		neuron._LayerIndex = 0;
		neuron._NeuronIndex = _InputLayer._Neurons.LastIndex();
	}

	/*
	*	Adds a hidden layer.
	*/
	FORCE_INLINE void AddHiddenLayer(const uint64 number_of_neurons) NOEXCEPT
	{
		_HiddenLayers.Emplace();
		Layer &hidden_layer{ _HiddenLayers.Back() };

		for (uint64 i{ 0 }; i < number_of_neurons; ++i)
		{
			hidden_layer._Neurons.Emplace();
			Neuron &neuron{ hidden_layer._Neurons.Back() };

			neuron._LayerIndex = 0;
			neuron._NeuronIndex = hidden_layer._Neurons.LastIndex();
		}
	}

	/*
	*	Adds an output neuron.
	*/
	FORCE_INLINE void AddOutputNeuron() NOEXCEPT
	{
		_OutputLayer._Neurons.Emplace();
		Neuron &neuron{ _OutputLayer._Neurons.Back() };

		neuron._LayerIndex = _HiddenLayers.Size() + 1;
		neuron._NeuronIndex = _OutputLayer._Neurons.LastIndex();
	}

	/*
	*	Prepares the neural network.
	*/
	FORCE_INLINE void Prepare() NOEXCEPT
	{
		//Prepare the input layer.
		for (Neuron &neuron : _InputLayer._Neurons)
		{
			neuron.SetNumberOfOutputs(_HiddenLayers[0]._Neurons.Size());
		}

		//Prepare the hidden layers.
		for (uint64 i{ 0 }, size{ _HiddenLayers.Size() }; i < size; ++i)
		{
			for (Neuron &neuron : _HiddenLayers[i]._Neurons)
			{
				neuron.SetNumberOfOutputs(i == size - 1 ? _OutputLayer._Neurons.Size() : _HiddenLayers[i + 1]._Neurons.Size());
			}
		}
	}

	/*
	*	Runs the neural network with the given input values.
	*/
	FORCE_INLINE void Run(const ArrayProxy<float32> &input_values) NOEXCEPT
	{
		//Set the output value on all neurons in the input layer.
		for (uint64 i{ 0 }, size{ _InputLayer._Neurons.Size() }; i < size; ++i)
		{
			_InputLayer._Neurons[i]._OutputValue = input_values[i];
		}

		//Feed forward into all hidden layers.
		for (uint64 i{ 0 }, size{ _HiddenLayers.Size() }; i < size; ++i)
		{
			//Cache the previous layer neurons.
			const DynamicArray<Neuron> &previous_layer_neurons{ i == 0 ? _InputLayer._Neurons : _HiddenLayers[i - 1]._Neurons };

			for (Neuron &neuron : _HiddenLayers[i]._Neurons)
			{
				neuron.FeedForward(previous_layer_neurons);
			}
		}

		//Feed forward into the output layer.
		{
			//Cache the previous layer neurons.
			const DynamicArray<Neuron> &previous_layer_neurons{ _HiddenLayers.Back()._Neurons };

			for (Neuron &neuron : _OutputLayer._Neurons)
			{
				neuron.FeedForward(previous_layer_neurons);
			}
		}
	}

	/*
	*	Corrects the results of the neural network by providing the expected output values.
	*/
	FORCE_INLINE void Correct(const ArrayProxy<float32> &expected_output_values) NOEXCEPT
	{
		//Calculate the error.
		_Error = 0.0f;

		for (uint64 i{ 0 }, size{ _OutputLayer._Neurons.Size() }; i < size; ++i)
		{
			const float32 delta{ expected_output_values[i] - _OutputLayer._Neurons[i]._OutputValue };

			_Error += delta * delta;
		}

		_Error /= static_cast<float32>(_OutputLayer._Neurons.Size());
		_Error = sqrt(_Error);

		//Calculate the output gradients for the output layer.
		for (uint64 i{ 0 }, size{ _OutputLayer._Neurons.Size() }; i < size; ++i)
		{
			_OutputLayer._Neurons[i].CalculateOutputGradients(expected_output_values[i]);
		}

		//Calculate the output gradients for the hidden layers.
		for (int64 i{ static_cast<int64>(_HiddenLayers.LastIndex()) }; i >= 0; --i)
		{
			//Cache the next layer neurons.
			const DynamicArray<Neuron> &next_layer_neurons{ i == _HiddenLayers.LastIndex() ? _OutputLayer._Neurons : _HiddenLayers[i + 1]._Neurons };

			for (Neuron &neuron : _HiddenLayers[i]._Neurons)
			{
				neuron.CalculateHiddenGradients(next_layer_neurons);
			}
		}

		//Update the input weights for the output layer.
		{
			//Cache the previous layer neurons.
			DynamicArray<Neuron> &previous_layer_neurons{ _HiddenLayers.Back()._Neurons };

			for (Neuron &neuron : _OutputLayer._Neurons)
			{
				neuron.UpdateInputWeights(_LearningRate, _Momentum, previous_layer_neurons);
			}
		}

		//Update the input weights for all hidden layers.
		for (int64 i{ static_cast<int64>(_HiddenLayers.LastIndex()) }; i >= 0; --i)
		{
			//Cache the previous layer neurons.
			DynamicArray<Neuron> &previous_layer_neurons{ i == 0 ? _InputLayer._Neurons : _HiddenLayers[i - 1]._Neurons };

			for (Neuron &neuron : _HiddenLayers[i]._Neurons)
			{
				neuron.UpdateInputWeights(_LearningRate, _Momentum, previous_layer_neurons);
			}
		}
	}

	/*
	*	Retrieves the outputs from the neural network.
	*/
	FORCE_INLINE void Retrieve(ArrayProxy<float32> *const RESTRICT outputs) NOEXCEPT
	{
		//Retrieve the output values.
		for (uint64 i{ 0 }, size{ outputs->Size() }; i < size; ++i)
		{
			outputs->At(i) = _OutputLayer._Neurons[i]._OutputValue;
		}
	}

private:

	/*
	*	Layer class definition.
	*/
	class Layer final
	{

	public:

		//The neurons.
		DynamicArray<Neuron> _Neurons;

	};

	//The learning rate.
	float32 _LearningRate{ 0.5f };

	//The momentum.
	float32 _Momentum{ 0.5f };

	//The input layer.
	Layer _InputLayer;

	//The hidden layers.
	DynamicArray<Layer> _HiddenLayers;

	//The output layer.
	Layer _OutputLayer;

	//The error.
	float32 _Error;

};