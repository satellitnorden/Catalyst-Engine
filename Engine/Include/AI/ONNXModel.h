#if defined(CATALYST_HAS_ONNX)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

/*
*	This class contains an ONNX model, loaded from a '.onnx' file, and takes care of setting it up and exposes some functionality for interacting with the model.
*/
class ONNXModel final
{

public:

	/*
	*	Default destructor.
	*/
	~ONNXModel() NOEXCEPT;

	/*
	*	Initializes this ONNX model with a model file path.
	*/
	void Initialize(const char *const RESTRICT model_file_path, const uint32 input_size, const uint32 output_size) NOEXCEPT;

	/*
	*	Initializes this ONNX model with raw data.
	*/
	void Initialize(const void *const RESTRICT model_data, const uint64 model_data_length, const uint32 input_size, const uint32 output_size) NOEXCEPT;

	/*
	*	Runs this ONNX model. Returns the outputs.
	*/
	NO_DISCARD const float32 *const RESTRICT Run(const float32 *const RESTRICT inputs) NOEXCEPT;

private:

	//The underlying data.
	void *RESTRICT _Data{ nullptr };

};
#endif