#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>
#include <Core/Containers/DynamicArray.h>

//File.
#include <File/Core/BinaryFile.h>

/*
*	Class representing a single neuron.
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
			weight = 0.5f;
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

/*
*	Class representing a fully connected (or dense) neural network, with as many input/intermediate/output neurons as the user specifies.
*	Can also load/save training data to disk, to be able to load this data in when needed.
*/
class FullyConnectedNeuralNetwork final
{

public:

	/*
	*	Sets the activation function.
	*/
	FORCE_INLINE void SetActivationFunction(const Neuron::ActivationFunction activation_function) NOEXCEPT
	{
		_ActivationFunction = activation_function;
	}

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

		neuron._NeuronIndex = _OutputLayer._Neurons.LastIndex();
	}

	/*
	*	Prepares the neural network.
	*/
	FORCE_INLINE void Prepare() NOEXCEPT
	{
		//If the activation function isn't set, just use the default.
		if (!_ActivationFunction)
		{
			SetActivationFunction([](const float32 X)
			{
				return 1.0f / (1.0f + exp(-X));
			});
		}

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
				neuron.FeedForward(previous_layer_neurons, _ActivationFunction);
			}
		}

		//Feed forward into the output layer.
		{
			//Cache the previous layer neurons.
			const DynamicArray<Neuron> &previous_layer_neurons{ _HiddenLayers.Back()._Neurons };

			for (Neuron &neuron : _OutputLayer._Neurons)
			{
				neuron.FeedForward(previous_layer_neurons, _ActivationFunction);
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

		//Update the average error.
		_AverageError = CatalystBaseMath::LinearlyInterpolate(_Error, _AverageError, 0.9f);

		//Calculate the output gradients for the output layer.
		for (uint64 i{ 0 }, size{ _OutputLayer._Neurons.Size() }; i < size; ++i)
		{
			_OutputLayer._Neurons[i].CalculateOutputGradient(expected_output_values[i]);
		}

		//Calculate the output gradients for the hidden layers.
		for (int64 i{ static_cast<int64>(_HiddenLayers.LastIndex()) }; i >= 0; --i)
		{
			//Cache the next layer neurons.
			const DynamicArray<Neuron> &next_layer_neurons{ i == _HiddenLayers.LastIndex() ? _OutputLayer._Neurons : _HiddenLayers[i + 1]._Neurons };

			for (Neuron &neuron : _HiddenLayers[i]._Neurons)
			{
				neuron.CalculateHiddenGradient(next_layer_neurons);
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
	*	Returns the average error.
	*/
	FORCE_INLINE NO_DISCARD float32 AverageError() const NOEXCEPT
	{
		return _AverageError;
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

	/*
	*	Exports the training data to the given file path.
	*/
	FORCE_INLINE void ExportTrainingData(const char *const RESTRICT file_path) NOEXCEPT
	{
		//Open the output file.
		BinaryFile<BinaryFileMode::OUT> output_file{ file_path };

		//Write all weights for all neurons.
		for (const Neuron &neuron : _InputLayer._Neurons)
		{
			for (const float32 weight : neuron._Weights)
			{
				output_file.Write(&weight, sizeof(float32));
			}
		}

		for (const Layer &hidden_layer : _HiddenLayers)
		{
			for (const Neuron &neuron : hidden_layer._Neurons)
			{
				for (const float32 weight : neuron._Weights)
				{
					output_file.Write(&weight, sizeof(float32));
				}
			}
		}

		for (const Neuron &neuron : _OutputLayer._Neurons)
		{
			for (const float32 weight : neuron._Weights)
			{
				output_file.Write(&weight, sizeof(float32));
			}
		}

		//Close the output file.
		output_file.Close();
	}

	/*
	*	Imports the training data from the given file path.
	*/
	FORCE_INLINE void ImportTrainingData(const char *const RESTRICT file_path) NOEXCEPT
	{
		//Open the input file.
		BinaryFile<BinaryFileMode::IN> input_file{ file_path };

		//Write all weights for all neurons.
		for (Neuron &neuron : _InputLayer._Neurons)
		{
			for (float32 &weight : neuron._Weights)
			{
				float32 imported_weight{ 0.0f };
				input_file.Read(&imported_weight, sizeof(float32));

				weight = imported_weight;
			}
		}

		for (Layer &hidden_layer : _HiddenLayers)
		{
			for (Neuron &neuron : hidden_layer._Neurons)
			{
				for (float32 &weight : neuron._Weights)
				{
					float32 imported_weight{ 0.0f };
					input_file.Read(&imported_weight, sizeof(float32));

					weight = imported_weight;
				}
			}
		}

		for (Neuron &neuron : _OutputLayer._Neurons)
		{
			for (float32 &weight : neuron._Weights)
			{
				float32 imported_weight{ 0.0f };
				input_file.Read(&imported_weight, sizeof(float32));

				weight = imported_weight;
			}
		}

		//Close the input file.
		input_file.Close();
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

	//The average error.
	float32 _AverageError{ 0.0f };

	//The activation function.
	Neuron::ActivationFunction _ActivationFunction{ nullptr };

};