#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>
#include <Core/Containers/DynamicArray.h>

//File.
#include <File/Core/BinaryFile.h>

//Math.
#include <Math/MachineLearning/Neuron.h>

//Handy macro for setting the activation function on a neural network.
#define NEURAL_NETWORK_SET_ACTIVATION_FUNCTION(NEURAL_NETWORK, FUNCTION)										\
{																												\
	(NEURAL_NETWORK).SetActivationFunction([](const float32 X) { return FUNCTION; }, "return "#FUNCTION";");	\
}

/*
*	Class representing one neural network, with as many input/intermediate/output neurons as the user specifies.
*	Can also load/save training data to disk, to be able to load this data in when needed.
*/
class NeuralNetwork final
{

public:

	/*
	*	Sets the activation function.
	*/
	FORCE_INLINE void SetActivationFunction(const Neuron::ActivationFunction activation_function, const char *const RESTRICT activation_function_string) NOEXCEPT
	{
		_ActivationFunction = activation_function;
		_ActivationFunctionString = activation_function_string;
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
			NEURAL_NETWORK_SET_ACTIVATION_FUNCTION(*this, 1.0f / (1.0f + exp(-X));
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
		BinaryFile<IOMode::Out> output_file{ file_path };

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
		BinaryFile<IOMode::In> input_file{ file_path };

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

	/*
	*	Condenses the entire neural network into a single function in the given file path, that can be used in C++ wherever.
	*/
	FORCE_INLINE void ExportFunction(const char *const RESTRICT function_description, const char *const RESTRICT function_name, const char *const RESTRICT file_path) NOEXCEPT
	{
		ASSERT(_OutputLayer._Neurons.Size() == 1, "Only has support for one output, for now!");

		//Open the output file.
		std::ofstream output_file{ file_path };

		//Write the function comment.
		output_file << "/*" << std::endl;
		output_file << "*\tThis function is autogenerated from a neural network." << std::endl;
		output_file << "*\t" << function_description << std::endl;
		output_file << "*/" << std::endl;

		//Write the function declaration.
		output_file << "FORCE_INLINE NO_DISCARD float32 " << function_name << "(";

		for (uint64 i{ 0 }, size{ _InputLayer._Neurons.Size() }; i < size; ++i)
		{
			output_file << "const float32 input_" << i;

			if (i < (size - 1))
			{
				output_file << ", ";
			}
		}

		output_file << ") NOEXCEPT" << std::endl;

		//Write the function body.
		output_file << "{" << std::endl;

		//Define the transform function.
		output_file << "\t//Define the activation function." << std::endl;
		output_file << "\tconstexpr auto ACTIVATION_FUNCTION{ [](const float32 X){ " << _ActivationFunctionString << " } };" << std::endl;
		output_file << std::endl;

		//Declare and calculate all hidden layer output values.
		output_file << "\t//Declare and calculate all hidden layer output values." << std::endl;

		for (uint64 hidden_layer_index{ 0 }; hidden_layer_index < _HiddenLayers.Size(); ++hidden_layer_index)
		{
			//Cache the previous layer neurons.
			const DynamicArray<Neuron> &previous_layer_neurons{ hidden_layer_index == 0 ? _InputLayer._Neurons : _HiddenLayers[hidden_layer_index - 1]._Neurons };

			for (uint64 neuron_index{ 0 }; neuron_index < _HiddenLayers[hidden_layer_index]._Neurons.Size(); ++neuron_index)
			{
				output_file << "\tconst float32 hidden_layer_" << hidden_layer_index << "_" << neuron_index << "_output_value{ ";

				output_file << "ACTIVATION_FUNCTION(";

				for (uint64 previous_neuron_index{ 0 }; previous_neuron_index < previous_layer_neurons.Size(); ++previous_neuron_index)
				{
					output_file << "(";

					if (hidden_layer_index == 0)
					{
						output_file << "input_" << previous_neuron_index;
					}

					else
					{
						output_file << "hidden_layer_" <<(hidden_layer_index - 1) << "_" << previous_neuron_index << "_output_value";
					}

					output_file << " * " << previous_layer_neurons[previous_neuron_index]._Weights[neuron_index] << "f";

					output_file << ")";

					if (previous_neuron_index < (previous_layer_neurons.LastIndex()))
					{
						output_file << " + ";
					}
				}

				output_file << ")";

				output_file << " };" << std::endl;
			}
		}

		output_file << std::endl;

		//Now return the final output.
		{

			output_file << "\t//Now return the final output." << std::endl;

			output_file << "\treturn ACTIVATION_FUNCTION(";

			//Cache the previous layer neurons.
			const DynamicArray<Neuron> &previous_layer_neurons{ _HiddenLayers.Back()._Neurons };

			for (uint64 previous_neuron_index{ 0 }; previous_neuron_index < previous_layer_neurons.Size(); ++previous_neuron_index)
			{
				output_file << "(";

				output_file << "hidden_layer_" << _HiddenLayers.LastIndex() << "_" << previous_neuron_index << "_output_value";

				output_file << " * " << previous_layer_neurons[previous_neuron_index]._Weights[0] << "f";

				output_file << ")";

				if (previous_neuron_index < (previous_layer_neurons.LastIndex()))
				{
					output_file << " + ";
				}
			}

			output_file << ");" << std::endl;
		}

		output_file << "}";

		//Close the output file.
		output_file.close();
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

	//The activation function string.
	const char *RESTRICT _ActivationFunctionString{ nullptr };

};