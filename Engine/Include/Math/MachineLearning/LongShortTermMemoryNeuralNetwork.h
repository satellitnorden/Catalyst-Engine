#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>

//Math.
#include <Math/MachineLearning/ActivationFunctions.h>
#include <Math/MachineLearning/NeuralNetwork.h>
#include <Math/MachineLearning/FullyConnectedNeuralNetwork.h>

/*
*	Class representing a long short term memory. neural network.
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

		//The number of outputs.
		uint32 _NumberOfOutputs{ 1 };

		//The hidden size.
		uint32 _HiddenSize{ 1 };

	};

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~LongShortTermMemoryNeuralNetwork() NOEXCEPT
	{

	}

	/*
	*	Initializes this neural network.
	*/
	FORCE_INLINE void Initialize(const InitializationParameters &parameters) NOEXCEPT
	{

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
		//Combine the previous hidden state and the input.
		const float32 combine{ _HiddenState + input_values[0] };

		//Run it through the forget gate.
		const float32 forget{ ForgetGate(combine) };

		//Run it through the candidate gate.
		const float32 candidate{ CandidateGate(combine) };

		//Run it through the input gate.
		const float32 input{ InputGate(combine) };

		//Run it through the output gate.
		const float32 output{ OutputGate(combine) };

		//Update the cell state.
		_CellState = _CellState * forget + candidate * input;

		//Update the hidden state.
		_HiddenState = output * ActivationFunctions::HyperbolicTangent(_CellState);
	}

	/*
	*	Corrects the results of the neural network by providing the expected output values.
	*/
	FORCE_INLINE void Correct(const ArrayProxy<float32> &expected_output_values) NOEXCEPT override
	{

	}

	/*
	*	Retrieves the outputs from the neural network.
	*/
	FORCE_INLINE void Retrieve(ArrayProxy<float32>* const RESTRICT outputs) NOEXCEPT override
	{
		//Output is just the current hidden state.
		outputs->At(0) = _HiddenState;
	}

private:

	//The cell state.
	float32 _CellState{ 0.0f };

	//The hidden state.
	float32 _HiddenState{ 0.0f };

	/*
	*	The forget gate.
	*/
	FORCE_INLINE NO_DISCARD float32 ForgetGate(const float32 X) NOEXCEPT
	{
		return ActivationFunctions::Sigmoid(X);
	}

	/*
	*	The input gate.
	*/
	FORCE_INLINE NO_DISCARD float32 InputGate(const float32 X) NOEXCEPT
	{
		return ActivationFunctions::Sigmoid(X);
	}

	/*
	*	The candidate gate.
	*/
	FORCE_INLINE NO_DISCARD float32 CandidateGate(const float32 X) NOEXCEPT
	{
		return ActivationFunctions::HyperbolicTangent(X);
	}

	/*
	*	The output gate.
	*/
	FORCE_INLINE NO_DISCARD float32 OutputGate(const float32 X) NOEXCEPT
	{
		return ActivationFunctions::Sigmoid(X);
	}

};