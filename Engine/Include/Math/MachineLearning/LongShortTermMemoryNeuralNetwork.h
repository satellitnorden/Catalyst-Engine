#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>

//Math.
#include <Math/MachineLearning/ActivationFunctions.h>
#include <Math/MachineLearning/NeuralNetwork.h>

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

		//The number of inputs.
		uint32 _NumberOfInputs;

		//The hidden size.
		uint32 _HiddenSize;

	};

	/*
	*	Initializes this neural network.
	*/
	FORCE_INLINE void Initialize(const InitializationParameters &parameters) NOEXCEPT
	{
		//Set the initial cell state.
		_PreviousCellState = _CurrentCellState = 0.0f;

		//Set the initial hidden state.
		_PreviousHiddenState = _CurrentHiddenState = 0.0f;

		//Initialize the forget input weight.
		_ForgetInputWeight = 0.5f;

		//Initialize the forget hidden weight.
		_ForgetHiddenWeight = 0.5f;

		//Initialize the input input weight.
		_InputInputWeight = 0.5f;

		//Initialize the input hidden weight.
		_InputHiddenWeight = 0.5f;

		//Initialize the candidate input weight.
		_CandidateInputWeight = 0.5f;

		//Initialize the candidate hidden weight.
		_CandidateHiddenWeight = 0.5f;

		//Initialize the output input weight.
		_OutputInputWeight = 0.5f;

		//Initialize the output hidden weight.
		_OutputHiddenWeight = 0.5f;
	}

	/*
	*	Returns the number of inputs.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetNumberOfInputs() const NOEXCEPT override
	{
		//Only one for now.
		return 1;
	}

	/*
	*	Returns the number of outputs.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetNumberOfOutputs() const NOEXCEPT override
	{
		//Only one for now.
		return 1;
	}

	/*
	*	Runs the neural network with the given input values.
	*/
	FORCE_INLINE void Run(const ArrayProxy<float32> &input_values) NOEXCEPT override
	{
		//Set the previous cell/hidden states.
		_PreviousCellState = _CurrentCellState;
		_PreviousHiddenState = _CurrentHiddenState;

		//Set the input sample.
		_InputSample = input_values[0];

		//Calculate the forget value.
		{
			_ForgetValue = ActivationFunctions::Sigmoid(_ForgetInputWeight * _InputSample + _ForgetHiddenWeight * _PreviousHiddenState);
		}

		//Calculate the input value.
		float32 input_value;

		{
			_InputValue = ActivationFunctions::Sigmoid(_InputInputWeight * _InputSample + _InputHiddenWeight * _PreviousHiddenState);
			_CandidateValue = ActivationFunctions::HyperbolicTangent(_CandidateInputWeight * _InputSample + _CandidateHiddenWeight * _PreviousHiddenState);
		
			input_value = _InputValue * _CandidateValue;
		}

		//Calculate the output value.
		{
			_OutputValue = ActivationFunctions::Sigmoid(_OutputInputWeight * _InputSample + _OutputHiddenWeight * _PreviousHiddenState);
		}

		//Update the cell state.
		_CurrentCellState = (_PreviousCellState * _ForgetValue) + input_value;

		//Update the hidden state.
		_CurrentHiddenState = _OutputValue * ActivationFunctions::HyperbolicTangent(_PreviousCellState);
	}

	/*
	*	Corrects the results of the neural network by providing the expected output values.
	*/
	FORCE_INLINE void Correct(const ArrayProxy<float32> &expected_output_values) NOEXCEPT override
	{

		//Calculate the delta value.
		const float32 delta_value{ expected_output_values[0] - _CurrentHiddenState };

		//Calculate the hyperbolic tangent of the current cell state.
		const float32 current_cell_state_hyperbolic_tangent{ ActivationFunctions::HyperbolicTangent(_CurrentCellState) };

		//Update the output weights.
		_OutputInputWeight += delta_value * current_cell_state_hyperbolic_tangent * _OutputValue * (1.0f - _OutputValue) * _InputSample;
		_OutputHiddenWeight += delta_value * current_cell_state_hyperbolic_tangent * _OutputValue * (1.0f - _OutputValue) * _PreviousHiddenState;

		//Update the forget weights.
		_ForgetInputWeight += delta_value * _OutputValue * (1.0f - (current_cell_state_hyperbolic_tangent * current_cell_state_hyperbolic_tangent)) * _PreviousCellState * _ForgetValue * (1.0f - _ForgetValue) * _InputSample;
		_ForgetHiddenWeight += delta_value * _OutputValue * (1.0f - (current_cell_state_hyperbolic_tangent * current_cell_state_hyperbolic_tangent)) * _PreviousCellState * _ForgetValue * (1.0f - _ForgetValue) * _PreviousHiddenState;
	
		//Update the input weights.
		_InputInputWeight += delta_value * _OutputValue * (1.0f - (current_cell_state_hyperbolic_tangent * current_cell_state_hyperbolic_tangent)) * _CandidateValue * _InputValue * (1.0f - _InputValue) * _InputSample;
		_InputHiddenWeight += delta_value * _OutputValue * (1.0f - (current_cell_state_hyperbolic_tangent * current_cell_state_hyperbolic_tangent)) * _CandidateValue * _InputValue * (1.0f - _InputValue) * _PreviousHiddenState;

		//Update the candidate weights.
		_CandidateInputWeight += delta_value * _OutputValue * (1.0f - (current_cell_state_hyperbolic_tangent * current_cell_state_hyperbolic_tangent)) * _InputValue * (1.0f - (_CandidateValue * _CandidateValue)) * _InputSample;
		_CandidateHiddenWeight += delta_value * _OutputValue * (1.0f - (current_cell_state_hyperbolic_tangent * current_cell_state_hyperbolic_tangent)) * _InputValue * (1.0f - (_CandidateValue * _CandidateValue)) * _PreviousHiddenState;
	}

	/*
	*	Retrieves the outputs from the neural network.
	*/
	FORCE_INLINE void Retrieve(ArrayProxy<float32>* const RESTRICT outputs) NOEXCEPT override
	{
		//The output values is the hidden state.
		for (uint64 i{ 0 }; i < outputs->Size(); ++i)
		{
			outputs->At(i) = _CurrentHiddenState;
		}
	}

private:

	//The previous cell state.
	float32 _PreviousCellState;

	//The current cell state.
	float32 _CurrentCellState;

	//The previous hidden state.
	float32 _PreviousHiddenState;

	//The current hidden state.
	float32 _CurrentHiddenState;

	//The input sample.
	float32 _InputSample;

	//The forget input weight.
	float32 _ForgetInputWeight;

	//The forget hidden weight.
	float32 _ForgetHiddenWeight;

	//The input input weight.
	float32 _InputInputWeight;

	//The input hidden weight.
	float32 _InputHiddenWeight;

	//The candidate input weight.
	float32 _CandidateInputWeight;

	//The candidate hidden weight.
	float32 _CandidateHiddenWeight;

	//The output input weight.
	float32 _OutputInputWeight;

	//The output hidden weight.
	float32 _OutputHiddenWeight;

	//The forget value.
	float32 _ForgetValue;

	//The input value.
	float32 _InputValue;

	//The candidate value.
	float32 _CandidateValue;

	//The output value.
	float32 _OutputValue;

};