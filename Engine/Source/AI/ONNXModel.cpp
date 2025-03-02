//Header file.
#include <AI/ONNXModel.h>

//Core.
#include <Core/Containers/DynamicArray.h>
#include <Core/Utilities/StringUtilities.h>

//Third party.
#include <ThirdParty/onnxruntime/onnxruntime_cxx_api.h>

/*
*	ONNX data class definition.
*/
class ONNXData final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE ONNXData(const char *const RESTRICT model_file_path, const uint32 input_size, const uint32 output_size) NOEXCEPT
		:
		_Environment(ORT_LOGGING_LEVEL_INFO, "ONNX Environment"),
		_Session(_Environment, GetModelFilePath(model_file_path), Ort::SessionOptions()),
		_InputTensor(nullptr),
		_OutputTensor(nullptr)
	{
		//Upsize the input &output buffers.
		_InputBuffer.Upsize<false>(input_size);
		_OutputBuffer.Upsize<false>(output_size);

		//Create the tensors.
		{
			//Set up the memory info.
			const Ort::MemoryInfo memory_info{ Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault) };

			//Set up the input tensor.
			const int64_t input_tensor_shape[]{ 1, input_size, 1 };
			_InputTensor = Ort::Value::CreateTensor(memory_info, _InputBuffer.Data(), _InputBuffer.Size(), input_tensor_shape, ARRAY_LENGTH(input_tensor_shape));

			//Set up the output tensor.
			const int64_t output_tensor_shape[]{ 1, output_size };
			_OutputTensor = Ort::Value::CreateTensor(memory_info, _OutputBuffer.Data(), _OutputBuffer.Size(), output_tensor_shape, ARRAY_LENGTH(output_tensor_shape));
		}
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~ONNXData() NOEXCEPT
	{

	}

	/*
	*	Runs this ONNX model. Returns the outputs.
	*/
	FORCE_INLINE NO_DISCARD const float32 *const RESTRICT Run(const float32 *const RESTRICT inputs) NOEXCEPT
	{
		//Define constants.
		constexpr const char *INPUT_NAMES[]
		{
			"input"
		};
		constexpr const char *OUTPUT_NAMES[]
		{
			"output"
		};

		//Fill up the input buffer.
		for (uint64 i{ 0 }; i < _InputBuffer.Size(); ++i)
		{
			_InputBuffer[i] = inputs[i];
		}

		//Set up the run options.
		Ort::RunOptions run_options;

		//Run the session!
		_Session.Run(run_options, &INPUT_NAMES[0], &_InputTensor, static_cast<size_t>(1), &OUTPUT_NAMES[0], &_OutputTensor, static_cast<size_t>(1));

		//Return the output.
		return _OutputBuffer.Data();
	}

private:

	//The model file path buffer.
	ORTCHAR_T _ModelFilePathBuffer[MAXIMUM_FILE_PATH_LENGTH];

	//The environment.
	Ort::Env _Environment;

	//The session.
	Ort::Session _Session;

	//The input buffer.
	DynamicArray<float32> _InputBuffer;

	//The output buffer.
	DynamicArray<float32> _OutputBuffer;

	//The input tensor.
	Ort::Value _InputTensor;

	//The output tensor.
	Ort::Value _OutputTensor;

	/*
	*	Returns the model file path.
	*/
	FORCE_INLINE NO_DISCARD const ORTCHAR_T* const RESTRICT GetModelFilePath(const char *const RESTRICT model_file_path) NOEXCEPT
	{
		const uint64 length{ StringUtilities::StringLength(model_file_path) };

		for (uint64 i{ 0 }; i < length; ++i)
		{
			_ModelFilePathBuffer[i] = static_cast<ORTCHAR_T>(model_file_path[i]);
		}

		_ModelFilePathBuffer[length] = static_cast<ORTCHAR_T>('\0');

		return _ModelFilePathBuffer;
	}
};

/*
*	Default destructor.
*/
ONNXModel::~ONNXModel() NOEXCEPT
{
	//Free the data, if there is one.
	if (_Data)
	{
		static_cast<ONNXData *const RESTRICT>(_Data)->~ONNXData();
	}
}

/*
*	Initializes this ONNX model.
*/
void ONNXModel::Initialize(const char *const RESTRICT model_file_path, const uint32 input_size, const uint32 output_size) NOEXCEPT
{
	//Allocate the data.
	_Data = new (Memory::Allocate(sizeof(ONNXData))) ONNXData(model_file_path, input_size, output_size);
}

/*
*	Runs this ONNX model. Returns the outputs.
*/
NO_DISCARD const float32 *const RESTRICT ONNXModel::Run(const float32 *const RESTRICT inputs) NOEXCEPT
{
	//Run the model.
	return static_cast<ONNXData *const RESTRICT>(_Data)->Run(inputs);
}