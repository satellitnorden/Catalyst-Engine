#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>
#include <Core/Containers/DynamicArray.h>

//File.
#include <File/Core/BinaryInputFile.h>
#include <File/Core/BinaryOutputFile.h>

//Math.
#include <Math/MachineLearning/ActivationFunctions.h>
#include <Math/MachineLearning/NeuralNetwork.h>

/*
*	Class representing a single neuron.
*/
class Neuron final
{

public:

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
	FORCE_INLINE void FeedForward(const DynamicArray<Neuron> &previous_layer_neurons, const NeuralNetworkActivationFunction activation_function) NOEXCEPT
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
class FullyConnectedNeuralNetwork final : public NeuralNetwork
{

public:

	//Constants.
	constexpr static float32 BIAS_VALUE{ 1.0f };

	/*
	*	Initialization parameters class definition.
	*/
	class InitializationParameters final
	{

	public:

		//The learning rate.
		float32 _LearningRate{ 0.01f };

		//The momentum.
		float32 _Momentum{ 0.01f };

		//The number of inputs.
		uint32 _NumberOfInputs{ 1 };

		//The number of hidden layers.
		uint32 _NumberOfHiddenLayers{ 1 };

		//The number of elements per hidden layer.
		uint32 _NumberOfNeuronsPerHiddenLayer{ 1 };

		//The number of outputs.
		uint32 _NumberOfOutputs{ 1 };

		//The hidden layers activation function.
		NeuralNetworkActivationFunction _HiddenLayersActivationFunction{ ActivationFunctions::HyperbolicTangent };

		//The output layer activation function.
		NeuralNetworkActivationFunction _OutputLayerActivationFunction{ ActivationFunctions::HyperbolicTangent };

	};

	/*
	*	Initializes this neural network.
	*/
	FORCE_INLINE void Initialize(const InitializationParameters &parameters) NOEXCEPT
	{
		//Set the learning rate.
		_LearningRate = parameters._LearningRate;

		//Set the momentum.
		_Momentum = parameters._Momentum;

		//Add the input neurons.
		for (uint32 i{ 0 }; i <= parameters._NumberOfInputs; ++i)
		{
			_InputLayer._Neurons.Emplace();
			Neuron &neuron{ _InputLayer._Neurons.Back() };

			neuron._NeuronIndex = _InputLayer._Neurons.LastIndex();
		}

		//Add the hidden layers.
		for (uint32 i{ 0 }; i < parameters._NumberOfHiddenLayers; ++i)
		{
			_HiddenLayers.Emplace();
			Layer &hidden_layer{ _HiddenLayers.Back() };

			for (uint32 j{ 0 }; j <= parameters._NumberOfNeuronsPerHiddenLayer; ++j)
			{
				hidden_layer._Neurons.Emplace();
				Neuron &neuron{ hidden_layer._Neurons.Back() };

				neuron._NeuronIndex = hidden_layer._Neurons.LastIndex();
			}
		}

		//Add the output neurons.
		for (uint32 i{ 0 }; i < parameters._NumberOfOutputs; ++i)
		{
			_OutputLayer._Neurons.Emplace();
			Neuron &neuron{ _OutputLayer._Neurons.Back() };

			neuron._NeuronIndex = _OutputLayer._Neurons.LastIndex();
		}

		//Set the hidden layers activation function.
		_HiddenLayersActivationFunction = parameters._HiddenLayersActivationFunction;

		//Set the output layer activation function.
		_OutputLayerActivationFunction = parameters._OutputLayerActivationFunction;

		//Prepare the input layer.
		for (Neuron &neuron : _InputLayer._Neurons)
		{
			neuron.SetNumberOfOutputs(_HiddenLayers.Empty() ? _OutputLayer._Neurons.Size() : _HiddenLayers[0]._Neurons.Size());
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
	*	Returns the number of inputs.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetNumberOfInputs() const NOEXCEPT override
	{
		return static_cast<uint32>(_InputLayer._Neurons.Size());
	}

	/*
	*	Returns the number of outputs.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetNumberOfOutputs() const NOEXCEPT override
	{
		return static_cast<uint32>(_InputLayer._Neurons.Size());
	}

	/*
	*	Runs the neural network with the given input values.
	*/
	FORCE_INLINE void Run(const ArrayProxy<float32> &input_values) NOEXCEPT override
	{
		//Set the output value on all neurons in the input layer.
		for (uint64 i{ 0 }, size{ _InputLayer._Neurons.Size() }; i < size; ++i)
		{
			_InputLayer._Neurons[i]._OutputValue = i < _InputLayer._Neurons.LastIndex() ? input_values[i] : BIAS_VALUE;
		}

		//Feed forward into all hidden layers.
		for (uint64 layer_index{ 0 }, size{ _HiddenLayers.Size() }; layer_index < size; ++layer_index)
		{
			//Cache the previous layer neurons.
			const DynamicArray<Neuron> &previous_layer_neurons{ layer_index == 0 ? _InputLayer._Neurons : _HiddenLayers[layer_index - 1]._Neurons };

			for (uint64 neuron_index{ 0 }; neuron_index < _HiddenLayers[layer_index]._Neurons.Size(); ++neuron_index)
			{
				if (neuron_index == _HiddenLayers[layer_index]._Neurons.LastIndex())
				{
					_HiddenLayers[layer_index]._Neurons[neuron_index]._OutputValue = BIAS_VALUE;
				}
				
				else
				{
					_HiddenLayers[layer_index]._Neurons[neuron_index].FeedForward(previous_layer_neurons, _HiddenLayersActivationFunction);
				}
			}
		}

		//Feed forward into the output layer.
		{
			//Cache the previous layer neurons.
			const DynamicArray<Neuron> &previous_layer_neurons{ _HiddenLayers.Empty() ? _InputLayer._Neurons : _HiddenLayers.Back()._Neurons };

			for (Neuron &neuron : _OutputLayer._Neurons)
			{
				neuron.FeedForward(previous_layer_neurons, _OutputLayerActivationFunction);
			}
		}
	}

	/*
	*	Corrects the results of the neural network by providing the expected output values.
	*/
	FORCE_INLINE void Correct(const ArrayProxy<float32> &expected_output_values) NOEXCEPT override
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
			_OutputLayer._Neurons[i].CalculateOutputGradient(expected_output_values[i]);
		}

		//Calculate the output gradients for the hidden layers.
		if (!_HiddenLayers.Empty())
		{
			for (int64 i{ static_cast<int64>(_HiddenLayers.LastIndex()) }; i >= 0; --i)
			{
				//Cache the next layer neurons.
				const DynamicArray<Neuron> &next_layer_neurons{ i == _HiddenLayers.LastIndex() ? _OutputLayer._Neurons : _HiddenLayers[i + 1]._Neurons };

				for (Neuron &neuron : _HiddenLayers[i]._Neurons)
				{
					neuron.CalculateHiddenGradient(next_layer_neurons);
				}
			}
		}

		//Update the input weights for the output layer.
		{
			//Cache the previous layer neurons.
			DynamicArray<Neuron> &previous_layer_neurons{ _HiddenLayers.Empty() ? _InputLayer._Neurons : _HiddenLayers.Back()._Neurons };

			for (Neuron &neuron : _OutputLayer._Neurons)
			{
				neuron.UpdateInputWeights(_LearningRate, _Momentum, previous_layer_neurons);
			}
		}

		//Update the input weights for all hidden layers.
		if (!_HiddenLayers.Empty())
		{
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
	}

	/*
	*	Retrieves the outputs from the neural network.
	*/
	FORCE_INLINE void Retrieve(ArrayProxy<float32>* const RESTRICT outputs) NOEXCEPT override
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
		BinaryOutputFile output_file{ file_path };

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
		BinaryInputFile input_file{ file_path };

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

	//The hidden layers activation function.
	NeuralNetworkActivationFunction _HiddenLayersActivationFunction{ ActivationFunctions::HyperbolicTangent };

	//The output layer activation function.
	NeuralNetworkActivationFunction _OutputLayerActivationFunction{ ActivationFunctions::HyperbolicTangent };

};