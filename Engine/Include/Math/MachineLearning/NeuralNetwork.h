#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>

/*
*	Base class for a neural network.
*/
class NeuralNetwork
{

public:

	/*
	*	Default destructor.
	*/
	FORCE_INLINE virtual ~NeuralNetwork() NOEXCEPT
	{

	}

	/*
	*	Returns the number of inputs.
	*/
	FORCE_INLINE virtual NO_DISCARD uint32 GetNumberOfInputs() const NOEXCEPT = 0;

	/*
	*	Returns the number of outputs.
	*/
	FORCE_INLINE virtual NO_DISCARD uint32 GetNumberOfOutputs() const NOEXCEPT = 0;

	/*
	*	Runs the neural network with the given input values.
	*/
	FORCE_INLINE virtual void Run(const ArrayProxy<float32> &input_values) NOEXCEPT = 0;

	/*
	*	Corrects the results of the neural network by providing the expected output values.
	*/
	FORCE_INLINE virtual void Correct(const ArrayProxy<float32> &expected_output_values) NOEXCEPT = 0;

	/*
	*	Retrieves the outputs from the neural network.
	*/
	FORCE_INLINE virtual void Retrieve(ArrayProxy<float32> *const RESTRICT outputs) NOEXCEPT = 0;

};