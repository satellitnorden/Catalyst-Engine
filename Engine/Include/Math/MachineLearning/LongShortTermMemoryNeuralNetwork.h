#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>
#include <Core/Containers/DynamicArray.h>

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
		//Set the initial cell states.
		_PreviousCellStates.Upsize<false>(parameters._HiddenSize);
		_CurrentCellStates.Upsize<false>(parameters._HiddenSize);
		
		for (uint32 i{ 0 }; i < parameters._HiddenSize; ++i)
		{
			_PreviousCellStates[i] = _CurrentCellStates[i] = 0.0f;
		}

		//Set the initial hidden states.
		_PreviousHiddenStates.Upsize<false>(parameters._HiddenSize);
		_CurrentHiddenStates.Upsize<false>(parameters._HiddenSize);

		for (uint32 i{ 0 }; i < parameters._HiddenSize; ++i)
		{
			_PreviousHiddenStates[i] = _CurrentHiddenStates[i] = 0.0f;
		}

		//Initialize the input samples.
		_InputSamples.Upsize<false>(parameters._NumberOfInputs);

		//Initialize the forget input weights.
		_ForgetInputWeights.Upsize<true>(parameters._HiddenSize);

		for (DynamicArray<float32> &weights : _ForgetInputWeights)
		{
			weights.Upsize<false>(parameters._NumberOfInputs);

			for (float32 &value : weights)
			{
				value = 0.5f;
			}
		}

		//Initialize the forget hidden weights.
		_ForgetHiddenWeights.Upsize<true>(parameters._HiddenSize);

		for (DynamicArray<float32> &weights : _ForgetHiddenWeights)
		{
			weights.Upsize<false>(parameters._HiddenSize);

			for (float32 &value : weights)
			{
				value = 0.5f;
			}
		}

		//Initialize the input input weights.
		_InputInputWeights.Upsize<true>(parameters._HiddenSize);

		for (DynamicArray<float32> &weights : _InputInputWeights)
		{
			weights.Upsize<false>(parameters._NumberOfInputs);

			for (float32 &value : weights)
			{
				value = 0.5f;
			}
		}

		//Initialize the input hidden weights.
		_InputHiddenWeights.Upsize<true>(parameters._HiddenSize);

		for (DynamicArray<float32> &weights : _InputHiddenWeights)
		{
			weights.Upsize<false>(parameters._HiddenSize);

			for (float32 &value : weights)
			{
				value = 0.5f;
			}
		}

		//Initialize the candidate input weights.
		_CandidateInputWeights.Upsize<true>(parameters._HiddenSize);

		for (DynamicArray<float32> &weights : _CandidateInputWeights)
		{
			weights.Upsize<false>(parameters._NumberOfInputs);

			for (float32 &value : weights)
			{
				value = 0.5f;
			}
		}

		//Initialize the candidate hidden weights.
		_CandidateHiddenWeights.Upsize<true>(parameters._HiddenSize);

		for (DynamicArray<float32> &weights : _CandidateHiddenWeights)
		{
			weights.Upsize<false>(parameters._HiddenSize);

			for (float32 &value : weights)
			{
				value = 0.5f;
			}
		}

		//Initialize the output input weights.
		_OutputInputWeights.Upsize<true>(parameters._HiddenSize);

		for (DynamicArray<float32> &weights : _OutputInputWeights)
		{
			weights.Upsize<false>(parameters._NumberOfInputs);

			for (float32 &value : weights)
			{
				value = 0.5f;
			}
		}

		//Initialize the output hidden weights.
		_OutputHiddenWeights.Upsize<true>(parameters._HiddenSize);

		for (DynamicArray<float32> &weights : _OutputHiddenWeights)
		{
			weights.Upsize<false>(parameters._HiddenSize);

			for (float32 &value : weights)
			{
				value = 0.5f;
			}
		}

		//Initialize the values.
		_ForgetValues.Upsize<false>(parameters._HiddenSize);
		_InputValues.Upsize<false>(parameters._HiddenSize);
		_CandidateValues.Upsize<false>(parameters._HiddenSize);
		_OutputValues.Upsize<false>(parameters._HiddenSize);
		_DeltaValues.Upsize<false>(parameters._HiddenSize);
	}

	/*
	*	Returns the number of inputs.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetNumberOfInputs() const NOEXCEPT override
	{
		return static_cast<uint32>(_ForgetInputWeights.Size());
	}

	/*
	*	Returns the number of outputs.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetNumberOfOutputs() const NOEXCEPT override
	{
		return static_cast<uint32>(_ForgetHiddenWeights.Size());
	}

	/*
	*	Runs the neural network with the given input values.
	*/
	FORCE_INLINE void Run(const ArrayProxy<float32> &input_values) NOEXCEPT override
	{
		//Set the previous cell/hidden states.
		for (uint64 i{ 0 }; i < _PreviousCellStates.Size(); ++i)
		{
			_PreviousCellStates[i] = _CurrentCellStates[i];
		}

		for (uint64 i{ 0 }; i < _PreviousHiddenStates.Size(); ++i)
		{
			_PreviousHiddenStates[i] = _CurrentHiddenStates[i];
		}

		//Set the input samples.
		for (uint64 i{ 0 }; i < _InputSamples.Size(); ++i)
		{
			_InputSamples[i] = input_values[i];
		}

		//Calculate the forget value.
		for (uint64 i{ 0 }; i < _ForgetValues.Size(); ++i)
		{
			float32 sum{ 0.0f };

			for (uint64 j{ 0 }; j < _ForgetInputWeights.Size(); ++j)
			{
				sum += _ForgetInputWeights[i][j] * _InputSamples[j];
			}

			for (uint64 j{ 0 }; j < _ForgetHiddenWeights.Size(); ++j)
			{
				sum += _ForgetHiddenWeights[i][j] * _PreviousHiddenStates[j];
			}

			_ForgetValues[i] = ActivationFunctions::Sigmoid(sum);
		}

		//Calculate the input values.
		for (uint64 i{ 0 }; i < _InputValues.Size(); ++i)
		{
			float32 sum{ 0.0f };

			for (uint64 j{ 0 }; j < _InputInputWeights.Size(); ++j)
			{
				sum += _InputInputWeights[i][j] * _InputSamples[j];
			}

			for (uint64 j{ 0 }; j < _InputHiddenWeights.Size(); ++j)
			{
				sum += _InputHiddenWeights[i][j] * _PreviousHiddenStates[j];
			}

			_InputValues[i] = ActivationFunctions::Sigmoid(sum);
		}

		//Calculate the candidate values.
		for (uint64 i{ 0 }; i < _CandidateValues.Size(); ++i)
		{
			float32 sum{ 0.0f };

			for (uint64 j{ 0 }; j < _CandidateInputWeights.Size(); ++j)
			{
				sum += _CandidateInputWeights[i][j] * _InputSamples[j];
			}

			for (uint64 j{ 0 }; j < _CandidateHiddenWeights.Size(); ++j)
			{
				sum += _CandidateHiddenWeights[i][j] * _PreviousHiddenStates[j];
			}

			_CandidateValues[i] = ActivationFunctions::HyperbolicTangent(sum);
		}

		//Calculate the output values.
		for (uint64 i{ 0 }; i < _OutputValues.Size(); ++i)
		{
			float32 sum{ 0.0f };

			for (uint64 j{ 0 }; j < _OutputInputWeights.Size(); ++j)
			{
				sum += _OutputInputWeights[i][j] * _InputSamples[j];
			}

			for (uint64 j{ 0 }; j < _OutputHiddenWeights.Size(); ++j)
			{
				sum += _OutputHiddenWeights[i][j] * _PreviousHiddenStates[j];
			}

			_OutputValues[i] = ActivationFunctions::Sigmoid(sum);
		}

		//Update the current cell states.
		for (uint64 i{ 0 }; i < _CurrentCellStates.Size(); ++i)
		{
			_CurrentCellStates[i] = (_PreviousCellStates[i] * _ForgetValues[i]) + (_InputValues[i] * _CandidateValues[i]);
		}
		
		//Update the current hidden states.
		for (uint64 i{ 0 }; i < _CurrentHiddenStates.Size(); ++i)
		{
			_CurrentHiddenStates[i] = _OutputValues[i] * ActivationFunctions::HyperbolicTangent(_PreviousCellStates[i]);
		}
	}

	/*
	*	Corrects the results of the neural network by providing the expected output values.
	*/
	FORCE_INLINE void Correct(const ArrayProxy<float32> &expected_output_values) NOEXCEPT override
	{
		//Calculate the delta values.
		for (uint64 i{ 0 }; i < _DeltaValues.Size(); ++i)
		{
			_DeltaValues[i] = expected_output_values[i] - _CurrentHiddenStates[i];
		}

		//Update the output weights.
		for (uint64 i{ 0 }; i < _OutputInputWeights.Size(); ++i)
		{
			for (uint64 j{ 0 }; j < _OutputInputWeights[i].Size(); ++j)
			{
				_OutputInputWeights[i][j] += _DeltaValues[j] * ActivationFunctions::HyperbolicTangent(_CurrentCellStates[j]) * _OutputValues[j] * (1.0f - _OutputValues[j]) * _InputSamples[i];
			}
		}

		for (uint64 i{ 0 }; i < _OutputHiddenWeights.Size(); ++i)
		{
			for (uint64 j{ 0 }; j < _OutputHiddenWeights[i].Size(); ++j)
			{
				_OutputHiddenWeights[i][j] += _DeltaValues[j] * ActivationFunctions::HyperbolicTangent(_CurrentCellStates[j]) * _OutputValues[j] * (1.0f - _OutputValues[j]) * _PreviousHiddenStates[j];
			}
		}
		
		//Update the forget weights.
		for (uint64 i{ 0 }; i < _ForgetInputWeights.Size(); ++i)
		{
			for (uint64 j{ 0 }; j < _ForgetInputWeights[i].Size(); ++j)
			{
				_ForgetInputWeights[i][j] += _DeltaValues[j] * _OutputValues[j] * (1.0f - (ActivationFunctions::HyperbolicTangent(_CurrentCellStates[j]) * ActivationFunctions::HyperbolicTangent(_CurrentCellStates[j]))) * _PreviousCellStates[j] * _ForgetValues[j] * (1.0f - _ForgetValues[j]) * _InputSamples[i];
			}
		}

		for (uint64 i{ 0 }; i < _ForgetHiddenWeights.Size(); ++i)
		{
			for (uint64 j{ 0 }; j < _ForgetHiddenWeights[i].Size(); ++j)
			{
				_ForgetHiddenWeights[i][j] += _DeltaValues[j] * _OutputValues[j] * (1.0f - (ActivationFunctions::HyperbolicTangent(_CurrentCellStates[j]) * ActivationFunctions::HyperbolicTangent(_CurrentCellStates[j]))) * _PreviousCellStates[j] * _ForgetValues[j] * (1.0f - _ForgetValues[j]) * _PreviousHiddenStates[j];
			}
		}
	
		//Update the input weights.
		for (uint64 i{ 0 }; i < _InputInputWeights.Size(); ++i)
		{
			for (uint64 j{ 0 }; j < _InputInputWeights[i].Size(); ++j)
			{
				_InputInputWeights[i][j] += _DeltaValues[j] * _OutputValues[j] * (1.0f - (ActivationFunctions::HyperbolicTangent(_CurrentCellStates[j]) * ActivationFunctions::HyperbolicTangent(_CurrentCellStates[j]))) * _CandidateValues[j] * _InputValues[j] * (1.0f - _InputValues[j]) * _InputSamples[i];
			}
		}

		for (uint64 i{ 0 }; i < _InputHiddenWeights.Size(); ++i)
		{
			for (uint64 j{ 0 }; j < _InputHiddenWeights[i].Size(); ++j)
			{
				_InputHiddenWeights[i][j] += _DeltaValues[j] * _OutputValues[j] * (1.0f - (ActivationFunctions::HyperbolicTangent(_CurrentCellStates[j]) * ActivationFunctions::HyperbolicTangent(_CurrentCellStates[j]))) * _CandidateValues[j] * _InputValues[j] * (1.0f - _InputValues[j]) * _PreviousHiddenStates[j];
			}
		}

		//Update the candidate weights.
		for (uint64 i{ 0 }; i < _CandidateInputWeights.Size(); ++i)
		{
			for (uint64 j{ 0 }; j < _CandidateInputWeights[i].Size(); ++j)
			{
				_CandidateInputWeights[i][j] += _DeltaValues[j] * _OutputValues[j] * (1.0f - (ActivationFunctions::HyperbolicTangent(_CurrentCellStates[j]) * ActivationFunctions::HyperbolicTangent(_CurrentCellStates[j]))) * _InputValues[j] * (1.0f - (_CandidateValues[j] * _CandidateValues[j])) * _InputSamples[i];
			}
		}

		for (uint64 i{ 0 }; i < _CandidateHiddenWeights.Size(); ++i)
		{
			for (uint64 j{ 0 }; j < _CandidateHiddenWeights[i].Size(); ++j)
			{
				_CandidateHiddenWeights[i][j] += _DeltaValues[j] * _OutputValues[j] * (1.0f - (ActivationFunctions::HyperbolicTangent(_CurrentCellStates[j]) * ActivationFunctions::HyperbolicTangent(_CurrentCellStates[j]))) * _InputValues[j] * (1.0f - (_CandidateValues[j] * _CandidateValues[j])) * _PreviousHiddenStates[j];
			}
		}
	}

	/*
	*	Retrieves the outputs from the neural network.
	*/
	FORCE_INLINE void Retrieve(ArrayProxy<float32>* const RESTRICT outputs) NOEXCEPT override
	{
		//The output values is the hidden state.
		for (uint64 i{ 0 }; i < outputs->Size(); ++i)
		{
			outputs->At(i) = _CurrentHiddenStates[i];
		}
	}

private:

	//The previous cell states.
	DynamicArray<float32> _PreviousCellStates;

	//The current cell states.
	DynamicArray<float32> _CurrentCellStates;

	//The previous hidden states.
	DynamicArray<float32> _PreviousHiddenStates;

	//The current hidden states.
	DynamicArray<float32> _CurrentHiddenStates;

	//The input samples.
	DynamicArray<float32> _InputSamples;

	//The forget input weights.
	DynamicArray<DynamicArray<float32>> _ForgetInputWeights;

	//The forget hidden weights.
	DynamicArray<DynamicArray<float32>> _ForgetHiddenWeights;

	//The input input weights.
	DynamicArray<DynamicArray<float32>> _InputInputWeights;

	//The input hidden weights.
	DynamicArray<DynamicArray<float32>> _InputHiddenWeights;

	//The candidate input weights.
	DynamicArray<DynamicArray<float32>> _CandidateInputWeights;

	//The candidate hidden weights.
	DynamicArray<DynamicArray<float32>> _CandidateHiddenWeights;

	//The output input weights.
	DynamicArray<DynamicArray<float32>> _OutputInputWeights;

	//The output hidden weights.
	DynamicArray<DynamicArray<float32>> _OutputHiddenWeights;

	//The forget values.
	DynamicArray<float32> _ForgetValues;

	//The input values.
	DynamicArray<float32> _InputValues;

	//The candidate values.
	DynamicArray<float32> _CandidateValues;

	//The output values.
	DynamicArray<float32> _OutputValues;

	//The delta values.
	DynamicArray<float32> _DeltaValues;

};