#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>

//Math.
#include <Math/MachineLearning/ActivationFunctions.h>
#include <Math/MachineLearning/NeuralNetwork.h>

/*
*	Class representing a long short term memory cell.
*/
class LongShortTermMemoryCell final
{

public:

	//The index.
	uint64 _Index;

	//The weights.
	DynamicArray<float32> _Weights;

	//The deltas.
	DynamicArray<float32> _Deltas;

	//The output value.
	float32 _OutputValue;

	//The gradient.
	float32 _Gradient;

	//The cell state.
	float32 _CellState;

	//The hidden state.
	float32 _HiddenState;

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
	FORCE_INLINE void FeedForward(const DynamicArray<LongShortTermMemoryCell> &previous_layer_cells) NOEXCEPT
	{
		//Calculate the input.
		float32 summed_input{ 0.0f };

		for (const LongShortTermMemoryCell &cell : previous_layer_cells)
		{
			summed_input += cell._OutputValue * cell._Weights[_Index];
		}

		//Combine the input with the hidden state.
		const float32 combine{ summed_input + _HiddenState };

		//Run the combined input and hidden state through the forget gate.
		const float32 forget{ ActivationFunctions::Sigmoid(combine) };

		//Run the combined input and hidden state through the input gate.
		const float32 input{ ActivationFunctions::Sigmoid(combine) };

		//Run the combined input and hidden state through the candidate gate.
		const float32 candidate{ ActivationFunctions::HyperbolicTangent(combine) };

		//Run the combined input and hidden state through the output gate.
		const float32 output{ ActivationFunctions::Sigmoid(combine) };

		//Update the cell state.
		_CellState = _CellState * forget + candidate * input;

		//Update the hidden state.
		_HiddenState = output * ActivationFunctions::HyperbolicTangent(_CellState);

		//Calculate the output value.
		_OutputValue = _HiddenState;
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
	FORCE_INLINE void CalculateHiddenGradient(const DynamicArray<LongShortTermMemoryCell> &next_layer_cells) NOEXCEPT
	{
		//Calculate the DOW.
		float32 DOW{ 0.0f };

		for (uint64 i{ 0 }, size{ next_layer_cells.Size() }; i < size; ++i)
		{
			DOW += _Weights[i] * next_layer_cells[i]._Gradient;
		}

		//Calculate the gradient.
		_Gradient = DOW * Derivative(_OutputValue);
	}

	/*
	*	Updates the input weights.
	*/
	FORCE_INLINE void UpdateInputWeights(const float32 learning_rate, const float32 momentum, DynamicArray<LongShortTermMemoryCell> &previous_layer_cells) NOEXCEPT
	{
		for (uint64 i{ 0 }, size{ previous_layer_cells.Size() }; i < size; ++i)
		{
			LongShortTermMemoryCell &cell{ previous_layer_cells[i] };

			const float32 old_delta{ cell._Deltas[_Index] };
			const float32 new_delta{ learning_rate * cell._OutputValue * _Gradient + momentum * old_delta };

			cell._Deltas[_Index] = new_delta;
			cell._Weights[_Index] += new_delta;
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
*	Class representing a long shot term memory.
*/
class LongShortTermMemoryNeuralNetwork final : public NeuralNetwork
{

public:

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

		//The number of cells per hidden layer.
		uint32 _NumberOfCellsPerHiddenLayer{ 1 };

		//The number of outputs.
		uint32 _NumberOfOutputs{ 1 };

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

		//Add the input cells.
		for (uint32 i{ 0 }; i < parameters._NumberOfInputs; ++i)
		{
			_InputLayer._Cells.Emplace();
			LongShortTermMemoryCell &cell{ _InputLayer._Cells.Back() };

			cell._Index = _InputLayer._Cells.LastIndex();
			cell._CellState = 0.5f;
			cell._HiddenState = 0.5f;
		}

		//Add the hidden layers.
		for (uint32 i{ 0 }; i < parameters._NumberOfHiddenLayers; ++i)
		{
			_HiddenLayers.Emplace();
			Layer &hidden_layer{ _HiddenLayers.Back() };

			for (uint32 j{ 0 }; j < parameters._NumberOfCellsPerHiddenLayer; ++j)
			{
				hidden_layer._Cells.Emplace();
				LongShortTermMemoryCell &cell{ hidden_layer._Cells.Back() };

				cell._Index = hidden_layer._Cells.LastIndex();
				cell._CellState = 0.5f;
				cell._HiddenState = 0.5f;
			}
		}

		//Add the output cells.
		for (uint32 i{ 0 }; i < parameters._NumberOfOutputs; ++i)
		{
			_OutputLayer._Cells.Emplace();
			LongShortTermMemoryCell &cell{ _OutputLayer._Cells.Back() };

			cell._Index = _OutputLayer._Cells.LastIndex();
			cell._CellState = 0.5f;
			cell._HiddenState = 0.5f;
		}

		//Prepare the input layer.
		for (LongShortTermMemoryCell &cell : _InputLayer._Cells)
		{
			cell.SetNumberOfOutputs(_HiddenLayers.Empty() ? _OutputLayer._Cells.Size() : _HiddenLayers[0]._Cells.Size());
		}

		//Prepare the hidden layers.
		for (uint64 i{ 0 }, size{ _HiddenLayers.Size() }; i < size; ++i)
		{
			for (LongShortTermMemoryCell &cell : _HiddenLayers[i]._Cells)
			{
				cell.SetNumberOfOutputs(i == size - 1 ? _OutputLayer._Cells.Size() : _HiddenLayers[i + 1]._Cells.Size());
			}
		}
	}

	/*
	*	Returns the number of inputs.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetNumberOfInputs() const NOEXCEPT override
	{
		return static_cast<uint32>(_InputLayer._Cells.Size());
	}

	/*
	*	Returns the number of outputs.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetNumberOfOutputs() const NOEXCEPT override
	{
		return static_cast<uint32>(_InputLayer._Cells.Size());
	}

	/*
	*	Runs the neural network with the given input values.
	*/
	FORCE_INLINE void Run(const ArrayProxy<float32> &input_values) NOEXCEPT override
	{
		//Set the output value on all cells in the input layer.
		for (uint64 i{ 0 }, size{ _InputLayer._Cells.Size() }; i < size; ++i)
		{
			_InputLayer._Cells[i]._OutputValue = input_values[i];
		}

		//Feed forward into all hidden layers.
		for (uint64 i{ 0 }, size{ _HiddenLayers.Size() }; i < size; ++i)
		{
			//Cache the previous layer cells.
			const DynamicArray<LongShortTermMemoryCell> &previous_layer_cells{ i == 0 ? _InputLayer._Cells : _HiddenLayers[i - 1]._Cells };

			for (LongShortTermMemoryCell &cell : _HiddenLayers[i]._Cells)
			{
				cell.FeedForward(previous_layer_cells);
			}
		}

		//Feed forward into the output layer.
		{
			//Cache the previous layer cells.
			const DynamicArray<LongShortTermMemoryCell> &previous_layer_cells{ _HiddenLayers.Empty() ? _InputLayer._Cells : _HiddenLayers.Back()._Cells };

			for (LongShortTermMemoryCell &cell : _OutputLayer._Cells)
			{
				cell.FeedForward(previous_layer_cells);
			}
		}
	}

	/*
	*	Corrects the results of the neural network by providing the expected output values.
	*/
	FORCE_INLINE void Correct(const ArrayProxy<float32> &expected_output_values) NOEXCEPT override
	{
		//Calculate the output gradients for the output layer.
		for (uint64 i{ 0 }, size{ _OutputLayer._Cells.Size() }; i < size; ++i)
		{
			_OutputLayer._Cells[i].CalculateOutputGradient(expected_output_values[i]);
		}

		//Calculate the output gradients for the hidden layers.
		if (!_HiddenLayers.Empty())
		{
			for (int64 i{ static_cast<int64>(_HiddenLayers.LastIndex()) }; i >= 0; --i)
			{
				//Cache the next layer cells.
				const DynamicArray<LongShortTermMemoryCell> &next_layer_cells{ i == _HiddenLayers.LastIndex() ? _OutputLayer._Cells : _HiddenLayers[i + 1]._Cells };

				for (LongShortTermMemoryCell &cell : _HiddenLayers[i]._Cells)
				{
					cell.CalculateHiddenGradient(next_layer_cells);
				}
			}
		}

		//Update the input weights for the output layer.
		{
			//Cache the previous layer cells.
			DynamicArray<LongShortTermMemoryCell> &previous_layer_cells{ _HiddenLayers.Empty() ? _InputLayer._Cells : _HiddenLayers.Back()._Cells };

			for (LongShortTermMemoryCell &cell : _OutputLayer._Cells)
			{
				cell.UpdateInputWeights(_LearningRate, _Momentum, previous_layer_cells);
			}
		}

		//Update the input weights for all hidden layers.
		if (!_HiddenLayers.Empty())
		{
			for (int64 i{ static_cast<int64>(_HiddenLayers.LastIndex()) }; i >= 0; --i)
			{
				//Cache the previous layer cells.
				DynamicArray<LongShortTermMemoryCell> &previous_layer_cells{ i == 0 ? _InputLayer._Cells : _HiddenLayers[i - 1]._Cells };

				for (LongShortTermMemoryCell &cell : _HiddenLayers[i]._Cells)
				{
					cell.UpdateInputWeights(_LearningRate, _Momentum, previous_layer_cells);
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
			outputs->At(i) = _OutputLayer._Cells[i]._OutputValue;
		}
	}

private:

	/*
	*	Layer class definition.
	*/
	class Layer final
	{

	public:

		//The cells.
		DynamicArray<LongShortTermMemoryCell> _Cells;

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

};