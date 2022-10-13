#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/MachineLearning/ActivationFunctions.h>
#include <Math/MachineLearning/NeuralNetwork.h>

#define LSTM_USE_FINAL_LAYER (0)

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

		//The hidden size.
		uint32 _HiddenSize{ 1 };

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
				value = 0.0f;
			}
		}

		//Initialize the forget hidden weights.
		_ForgetHiddenWeights.Upsize<true>(parameters._HiddenSize);

		for (DynamicArray<float32> &weights : _ForgetHiddenWeights)
		{
			weights.Upsize<false>(parameters._HiddenSize);

			for (float32 &value : weights)
			{
				value = 0.0f;
			}
		}

		//Initialize the input input weights.
		_InputInputWeights.Upsize<true>(parameters._HiddenSize);

		for (DynamicArray<float32> &weights : _InputInputWeights)
		{
			weights.Upsize<false>(parameters._NumberOfInputs);

			for (float32 &value : weights)
			{
				value = 0.0f;
			}
		}

		//Initialize the input hidden weights.
		_InputHiddenWeights.Upsize<true>(parameters._HiddenSize);

		for (DynamicArray<float32> &weights : _InputHiddenWeights)
		{
			weights.Upsize<false>(parameters._HiddenSize);

			for (float32 &value : weights)
			{
				value = 0.0f;
			}
		}

		//Initialize the candidate input weights.
		_CandidateInputWeights.Upsize<true>(parameters._HiddenSize);

		for (DynamicArray<float32> &weights : _CandidateInputWeights)
		{
			weights.Upsize<false>(parameters._NumberOfInputs);

			for (float32 &value : weights)
			{
				value = 0.0f;
			}
		}

		//Initialize the candidate hidden weights.
		_CandidateHiddenWeights.Upsize<true>(parameters._HiddenSize);

		for (DynamicArray<float32> &weights : _CandidateHiddenWeights)
		{
			weights.Upsize<false>(parameters._HiddenSize);

			for (float32 &value : weights)
			{
				value = 0.0f;
			}
		}

		//Initialize the output input weights.
		_OutputInputWeights.Upsize<true>(parameters._HiddenSize);

		for (DynamicArray<float32> &weights : _OutputInputWeights)
		{
			weights.Upsize<false>(parameters._NumberOfInputs);

			for (float32 &value : weights)
			{
				value = 0.0f;
			}
		}

		//Initialize the output hidden weights.
		_OutputHiddenWeights.Upsize<true>(parameters._HiddenSize);

		for (DynamicArray<float32> &weights : _OutputHiddenWeights)
		{
			weights.Upsize<false>(parameters._HiddenSize);

			for (float32 &value : weights)
			{
				value = 0.0f;
			}
		}

#if LSTM_USE_FINAL_LAYER
		//Initialize the final weights.
		_FinalWeights.Upsize<true>(parameters._NumberOfOutputs);

		for (DynamicArray<float32> &weights : _FinalWeights)
		{
			weights.Upsize<false>(parameters._HiddenSize);

			for (float32 &value : weights)
			{
				value = 0.0f;
			}
		}
#endif

		//Initialize the values.
		_ForgetValues.Upsize<false>(parameters._HiddenSize);
		_InputValues.Upsize<false>(parameters._HiddenSize);
		_CandidateValues.Upsize<false>(parameters._HiddenSize);
		_OutputValues.Upsize<false>(parameters._HiddenSize);
		_DeltaValues.Upsize<false>(parameters._HiddenSize);

		for (float32 &value : _DeltaValues)
		{
			value = 0.0f;
		}

#if LSTM_USE_FINAL_LAYER
		//Initialize the final stuff.
		_FinalOutputs.Upsize<false>(parameters._NumberOfOutputs);
		_FinalGradients.Upsize<false>(parameters._NumberOfOutputs);
#endif
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

			for (uint64 j{ 0 }; j < _ForgetInputWeights[i].Size(); ++j)
			{
				sum += _ForgetInputWeights[i][j] * _InputSamples[j];
			}

			for (uint64 j{ 0 }; j < _ForgetHiddenWeights[i].Size(); ++j)
			{
				sum += _ForgetHiddenWeights[i][j] * _PreviousHiddenStates[j];
			}

			_ForgetValues[i] = ActivationFunctions::Sigmoid(sum);
		}

		//Calculate the input values.
		for (uint64 i{ 0 }; i < _InputValues.Size(); ++i)
		{
			float32 sum{ 0.0f };

			for (uint64 j{ 0 }; j < _InputInputWeights[i].Size(); ++j)
			{
				sum += _InputInputWeights[i][j] * _InputSamples[j];
			}

			for (uint64 j{ 0 }; j < _InputHiddenWeights[i].Size(); ++j)
			{
				sum += _InputHiddenWeights[i][j] * _PreviousHiddenStates[j];
			}

			_InputValues[i] = ActivationFunctions::Sigmoid(sum);
		}

		//Calculate the candidate values.
		for (uint64 i{ 0 }; i < _CandidateValues.Size(); ++i)
		{
			float32 sum{ 0.0f };

			for (uint64 j{ 0 }; j < _CandidateInputWeights[i].Size(); ++j)
			{
				sum += _CandidateInputWeights[i][j] * _InputSamples[j];
			}

			for (uint64 j{ 0 }; j < _CandidateHiddenWeights[i].Size(); ++j)
			{
				sum += _CandidateHiddenWeights[i][j] * _PreviousHiddenStates[j];
			}

			_CandidateValues[i] = ActivationFunctions::HyperbolicTangent(sum);
		}

		//Calculate the output values.
		for (uint64 i{ 0 }; i < _OutputValues.Size(); ++i)
		{
			float32 sum{ 0.0f };

			for (uint64 j{ 0 }; j < _OutputInputWeights[i].Size(); ++j)
			{
				sum += _OutputInputWeights[i][j] * _InputSamples[j];
			}

			for (uint64 j{ 0 }; j < _OutputHiddenWeights[i].Size(); ++j)
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

#if LSTM_USE_FINAL_LAYER
		//Calculate the final outputs.
		for (uint64 i{ 0 }; i < _FinalOutputs.Size(); ++i)
		{
			_FinalOutputs[i] = 0.0f;

			for (uint64 j{ 0 }; j < _FinalWeights[i].Size(); ++j)
			{
				_FinalOutputs[i] += _CurrentHiddenStates[j] * _FinalWeights[i][j];
			}
		}
#endif
	}

	/*
	*	Corrects the results of the neural network by providing the expected output values.
	*/
	FORCE_INLINE void Correct(const ArrayProxy<float32> &expected_output_values) NOEXCEPT override
	{
#if LSTM_USE_FINAL_LAYER
		//Calculate the final gradients.
		for (uint64 i{ 0 }; i < _FinalGradients.Size(); ++i)
		{
			_FinalGradients[i] = (expected_output_values[i] - _FinalOutputs[i]) * Derivative(_FinalOutputs[i]);
		
		}

		//Calculate the delta values.
		for (uint64 i{ 0 }; i < _DeltaValues.Size(); ++i)
		{
			for (uint64 j{ 0 }; j < _FinalGradients.Size(); ++j)
			{
				const float32 old_delta{ _DeltaValues[i] };
				const float32 new_delta{ _LearningRate * _CurrentHiddenStates[i] * _FinalGradients[j] + _Momentum * old_delta };

				_DeltaValues[i] = new_delta;
				_FinalWeights[j][i] += new_delta;
			}
		}
#else
		for (uint64 i{ 0 }; i < _DeltaValues.Size(); ++i)
		{
			_DeltaValues[i] = _LearningRate * (expected_output_values[i] - _CurrentHiddenStates[i]);
		}
#endif

		//Update the output weights.
		for (uint64 i{ 0 }; i < _OutputInputWeights.Size(); ++i)
		{
			for (uint64 j{ 0 }; j < _OutputInputWeights[i].Size(); ++j)
			{
				const float32 gradient{ _DeltaValues[i] * ActivationFunctions::HyperbolicTangent(_CurrentCellStates[i]) * _OutputValues[i] * (1.0f - _OutputValues[i]) * _InputSamples[j] };
				
				_OutputInputWeights[i][j] += gradient;
			}
		}

		for (uint64 i{ 0 }; i < _OutputHiddenWeights.Size(); ++i)
		{
			for (uint64 j{ 0 }; j < _OutputHiddenWeights[i].Size(); ++j)
			{
				const float32 gradient{ _DeltaValues[j] * ActivationFunctions::HyperbolicTangent(_CurrentCellStates[i]) * _OutputValues[i] * (1.0f - _OutputValues[i]) * _PreviousHiddenStates[i] };

				_OutputHiddenWeights[i][j] += gradient;
			}
		}
		
		//Update the forget weights.
		for (uint64 i{ 0 }; i < _ForgetInputWeights.Size(); ++i)
		{
			for (uint64 j{ 0 }; j < _ForgetInputWeights[i].Size(); ++j)
			{
				const float32 gradient{ _DeltaValues[i] * _OutputValues[i] * (1.0f - (ActivationFunctions::HyperbolicTangent(_CurrentCellStates[i]) * ActivationFunctions::HyperbolicTangent(_CurrentCellStates[i]))) * _PreviousCellStates[i] * _ForgetValues[i] * (1.0f - _ForgetValues[i]) * _InputSamples[j] };
				
				_ForgetInputWeights[i][j] += gradient;
			}
		}

		for (uint64 i{ 0 }; i < _ForgetHiddenWeights.Size(); ++i)
		{
			for (uint64 j{ 0 }; j < _ForgetHiddenWeights[i].Size(); ++j)
			{
				const float32 gradient{ _DeltaValues[i] * _OutputValues[i] * (1.0f - (ActivationFunctions::HyperbolicTangent(_CurrentCellStates[i]) * ActivationFunctions::HyperbolicTangent(_CurrentCellStates[i]))) * _PreviousCellStates[i] * _ForgetValues[i] * (1.0f - _ForgetValues[i]) * _PreviousHiddenStates[i] };

				_ForgetHiddenWeights[i][j] += gradient;
			}
		}
	
		//Update the input weights.
		for (uint64 i{ 0 }; i < _InputInputWeights.Size(); ++i)
		{
			for (uint64 j{ 0 }; j < _InputInputWeights[i].Size(); ++j)
			{
				const float32 gradient{ _DeltaValues[i] * _OutputValues[i] * (1.0f - (ActivationFunctions::HyperbolicTangent(_CurrentCellStates[i]) * ActivationFunctions::HyperbolicTangent(_CurrentCellStates[i]))) * _CandidateValues[i] * _InputValues[i] * (1.0f - _InputValues[i]) * _InputSamples[j] };
				
				_InputInputWeights[i][j] += gradient;
			}
		}

		for (uint64 i{ 0 }; i < _InputHiddenWeights.Size(); ++i)
		{
			for (uint64 j{ 0 }; j < _InputHiddenWeights[i].Size(); ++j)
			{
				const float32 gradient{ _DeltaValues[i] * _OutputValues[i] * (1.0f - (ActivationFunctions::HyperbolicTangent(_CurrentCellStates[i]) * ActivationFunctions::HyperbolicTangent(_CurrentCellStates[i]))) * _CandidateValues[i] * _InputValues[i] * (1.0f - _InputValues[i]) * _PreviousHiddenStates[i] };
				
				_InputHiddenWeights[i][j] += gradient;
			}
		}

		//Update the candidate weights.
		for (uint64 i{ 0 }; i < _CandidateInputWeights.Size(); ++i)
		{
			for (uint64 j{ 0 }; j < _CandidateInputWeights[i].Size(); ++j)
			{
				const float32 gradient{ _DeltaValues[i] * _OutputValues[i] * (1.0f - (ActivationFunctions::HyperbolicTangent(_CurrentCellStates[i]) * ActivationFunctions::HyperbolicTangent(_CurrentCellStates[i]))) * _InputValues[i] * (1.0f - (_CandidateValues[i] * _CandidateValues[i])) * _InputSamples[j] };
				
				_CandidateInputWeights[i][j] += gradient;
			}
		}

		for (uint64 i{ 0 }; i < _CandidateHiddenWeights.Size(); ++i)
		{
			for (uint64 j{ 0 }; j < _CandidateHiddenWeights[i].Size(); ++j)
			{
				const float32 gradient{ _DeltaValues[i] * _OutputValues[i] * (1.0f - (ActivationFunctions::HyperbolicTangent(_CurrentCellStates[i]) * ActivationFunctions::HyperbolicTangent(_CurrentCellStates[i]))) * _InputValues[i] * (1.0f - (_CandidateValues[i] * _CandidateValues[i])) * _PreviousHiddenStates[i] };
				
				_CandidateHiddenWeights[i][j] += gradient;
			}
		}
	}

	/*
	*	Retrieves the outputs from the neural network.
	*/
	FORCE_INLINE void Retrieve(ArrayProxy<float32>* const RESTRICT outputs) NOEXCEPT override
	{
#if LSTM_USE_FINAL_LAYER
		for (uint64 i{ 0 }; i < outputs->Size(); ++i)
		{
			outputs->At(i) = _FinalOutputs[i];
		}
#else
		//The output values is the hidden state.
		for (uint64 i{ 0 }; i < outputs->Size(); ++i)
		{
			outputs->At(i) = _CurrentHiddenStates[i];
		}
#endif
	}

private:

	//The learning rate.
	float32 _LearningRate{ 0.01f };

	//The momentum.
	float32 _Momentum{ 0.01f };

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

#if LSTM_USE_FINAL_LAYER
	//The final weights.
	DynamicArray<DynamicArray<float32>> _FinalWeights;

	//The final outputs.
	DynamicArray<float32> _FinalOutputs;

	//The final gradients.
	DynamicArray<float32> _FinalGradients;
#endif

	//The delta values.
	DynamicArray<float32> _DeltaValues;

	/*
	*	Calculates the derivative for the given value.
	*/
	FORCE_INLINE NO_DISCARD float32 Derivative(const float32 X) NOEXCEPT
	{
		return 1.0f - (X * X);
	}

};