//Header file.
#include <Content/AssetCompilers/RenderPipelineAssetCompiler.h>

//Core.
#include <Core/General/Pair.h>

//File.
#include <File/Core/File.h>
#include <File/Core/BinaryOutputFile.h>
#include <File/Utilities/TextParsingUtilities.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/Compilation/GLSLCompilation.h>

//Systems.
#include <Systems/LogSystem.h>
#include <Systems/TaskSystem.h>

//STD.
#include <filesystem>
#include <fstream>
#include <string>

//Defines.
#define ENGINE_RENDERING_PATH "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Rendering"
#define GAME_RENDERING_PATH "..\\..\\..\\Rendering"

#define COMPILE_SINGLE_THREADED (1)
#define LOAD_SINGLE_THREADED (1)

/*
*	Extra data class definition.
*/
class ExtraData final
{

public:

	/*
	*	Push constant data value class definition.
	*/
	class PushConstantDataValue final
	{

	public:

		//The type.
		DynamicString _Type;

		//The name.
		DynamicString _Name;

	};

	/*
	*	Output parameter class definition.
	*/
	class OutputParameter final
	{

	public:

		//The type.
		DynamicString _Type;

		//The name.
		DynamicString _Name;

	};

	/*
	*	Input parameter class definition.
	*/
	class InputParameter final
	{

	public:

		//The type.
		DynamicString _Type;

		//The name.
		DynamicString _Name;

	};

	/*
	*	Ray tracing payload.
	*/
	class RayTracingPayload final
	{

	public:

		//The index.
		uint32 _Index;

		//The type.
		DynamicString _Type;

		//The name.
		DynamicString _Name;

	};

	//The push constant data values.
	DynamicArray<PushConstantDataValue> _PushConstantDataValues;

	//The vertex input parameters.
	DynamicArray<InputParameter> _VertexInputParameters;

	//The vertex output parameters.
	DynamicArray<OutputParameter> _VertexOutputParameters;

	//The fragment input parameters.
	DynamicArray<InputParameter> _FragmentInputParameters;

	//The compute local size.
	struct
	{
		//The width.
		uint32 _Width;

		//The height.
		uint32 _Height;

		//The depth.
		uint32 _Depth;
	} _ComputeLocalSize;

	//The ray generation payloads.
	DynamicArray<RayTracingPayload> _RayGenerationPayloads;

	//The ray miss payloads.
	DynamicArray<DynamicArray<RayTracingPayload>> _RayMissPayloads;

	//The ray any hit payloads.
	DynamicArray<DynamicArray<RayTracingPayload>> _RayAnyHitPayloads;

	//The ray closest hit payloads.
	DynamicArray<DynamicArray<RayTracingPayload>> _RayClosestHitPayloads;

};

//The shader stage -> string lookup.
constexpr Pair<ShaderStage, const char *const RESTRICT> SHADER_STAGE_TO_STRING_LOOKUP[]
{
	{ ShaderStage::COMPUTE, "Compute" },
	{ ShaderStage::FRAGMENT, "Fragment" },
	{ ShaderStage::GEOMETRY, "Geometry" },
	{ ShaderStage::RAY_ANY_HIT, "RayAnyHit" },
	{ ShaderStage::RAY_CLOSEST_HIT, "RayClosestHit" },
	{ ShaderStage::RAY_GENERATION, "RayGeneration" },
	{ ShaderStage::RAY_INTERSECTION, "RayInteresection" },
	{ ShaderStage::RAY_MISS, "RayMiss" },
	{ ShaderStage::TESSELLATION_CONTROL, "TessellationControl" },
	{ ShaderStage::TESSELLATION_EVALUATION, "TessellationEvaluation" },
	{ ShaderStage::VERTEX, "Vertex" }
};

/*
*	Parses an attachment load operator.
*/
FORCE_INLINE NO_DISCARD AttachmentLoadOperator ParseAttachmentLoadOperator(const char *const RESTRICT string) NOEXCEPT
{
	if (StringUtilities::IsEqual(string, "LOAD"))
	{
		return AttachmentLoadOperator::LOAD;
	}

	else if (StringUtilities::IsEqual(string, "CLEAR"))
	{
		return AttachmentLoadOperator::CLEAR;
	}

	else if (StringUtilities::IsEqual(string, "DONT_CARE"))
	{
		return AttachmentLoadOperator::DONT_CARE;
	}

	else
	{
		ASSERT(false, "Unknown attachment load operator string: %s", string);

		return static_cast<AttachmentLoadOperator>(0);
	}
}

/*
*	Parses an attachment store operator.
*/
FORCE_INLINE NO_DISCARD AttachmentStoreOperator ParseAttachmentStoreOperator(const char* const RESTRICT string) NOEXCEPT
{
	if (StringUtilities::IsEqual(string, "STORE"))
	{
		return AttachmentStoreOperator::STORE;
	}

	else if (StringUtilities::IsEqual(string, "DONT_CARE"))
	{
		return AttachmentStoreOperator::DONT_CARE;
	}

	else
	{
		ASSERT(false, "Unknown attachment store operator string: %s", string);

		return static_cast<AttachmentStoreOperator>(0);
	}
}

/*
*	Parses a blend factor.
*/
FORCE_INLINE NO_DISCARD BlendFactor ParseBlendFactor(const char *const RESTRICT string) NOEXCEPT
{
	if (StringUtilities::IsEqual(string, "ZERO"))
	{
		return BlendFactor::ZERO;
	}

	else if (StringUtilities::IsEqual(string, "ONE"))
	{
		return BlendFactor::ONE;
	}

	else if (StringUtilities::IsEqual(string, "SOURCE_COLOR"))
	{
		return BlendFactor::SOURCE_COLOR;
	}

	else if (StringUtilities::IsEqual(string, "ONE_MINUS_SOURCE_COLOR"))
	{
		return BlendFactor::ONE_MINUS_SOURCE_COLOR;
	}

	else if (StringUtilities::IsEqual(string, "DESTINATION_COLOR"))
	{
		return BlendFactor::DESTINATION_COLOR;
	}

	else if (StringUtilities::IsEqual(string, "ONE_MINUS_DESTINATION_COLOR"))
	{
		return BlendFactor::ONE_MINUS_DESTINATION_COLOR;
	}

	else if (StringUtilities::IsEqual(string, "SOURCE_ALPHA"))
	{
		return BlendFactor::SOURCE_ALPHA;
	}

	else if (StringUtilities::IsEqual(string, "ONE_MINUS_SOURCE_ALPHA"))
	{
		return BlendFactor::ONE_MINUS_SOURCE_ALPHA;
	}

	else if (StringUtilities::IsEqual(string, "DESTINATION_ALPHA"))
	{
		return BlendFactor::DESTINATION_ALPHA;
	}

	else if (StringUtilities::IsEqual(string, "ONE_MINUS_DESTINATION_ALPHA"))
	{
		return BlendFactor::ONE_MINUS_DESTINATION_ALPHA;
	}

	else
	{
		ASSERT(false, "Unknown blend factor: %s", string);

		return static_cast<BlendFactor>(0);
	}
}

/*
*	Parses a blend operator.
*/
FORCE_INLINE NO_DISCARD BlendOperator ParseBlendOperator(const char *const RESTRICT string) NOEXCEPT
{
	if (StringUtilities::IsEqual(string, "ADD"))
	{
		return BlendOperator::ADD;
	}

	else if (StringUtilities::IsEqual(string, "SUBTRACT"))
	{
		return BlendOperator::SUBTRACT;
	}

	else if (StringUtilities::IsEqual(string, "REVERSE_SUBTRACT"))
	{
		return BlendOperator::REVERSE_SUBTRACT;
	}

	else if (StringUtilities::IsEqual(string, "MIN"))
	{
		return BlendOperator::MIN;
	}

	else if (StringUtilities::IsEqual(string, "MAX"))
	{
		return BlendOperator::MAX;
	}

	else
	{
		ASSERT(false, "Unknown blend operator: %s", string);

		return static_cast<BlendOperator>(0);
	}
}

/*
*	Parses a compare operator.
*/
FORCE_INLINE NO_DISCARD CompareOperator ParseCompareOperator(const char *const RESTRICT string) NOEXCEPT
{
	if (StringUtilities::IsEqual(string, "ALWAYS"))
	{
		return CompareOperator::Always;
	}

	else if (StringUtilities::IsEqual(string, "EQUAL"))
	{
		return CompareOperator::Equal;
	}

	else if (StringUtilities::IsEqual(string, "GREATER"))
	{
		return CompareOperator::Greater;
	}

	else if (StringUtilities::IsEqual(string, "GREATER_OR_EQUAL"))
	{
		return CompareOperator::GreaterOrEqual;
	}

	else if (StringUtilities::IsEqual(string, "LESS"))
	{
		return CompareOperator::Less;
	}

	else if (StringUtilities::IsEqual(string, "LESS_OR_EQUAL"))
	{
		return CompareOperator::LessOrEqual;
	}

	else if (StringUtilities::IsEqual(string, "NEVER"))
	{
		return CompareOperator::Never;
	}

	else if (StringUtilities::IsEqual(string, "NOT_EQUAL"))
	{
		return CompareOperator::NotEqual;
	}

	else
	{
		ASSERT(false, "Unknown compare operator string: %s", string);

		return static_cast<CompareOperator>(0);
	}
}

/*
*	Parses a stencil operator.
*/
FORCE_INLINE NO_DISCARD StencilOperator ParseStencilOperator(const char *const RESTRICT string) NOEXCEPT
{
	if (StringUtilities::IsEqual(string, "KEEP"))
	{
		return StencilOperator::Keep;
	}

	else if (StringUtilities::IsEqual(string, "ZERO"))
	{
		return StencilOperator::Zero;
	}

	else if (StringUtilities::IsEqual(string, "REPLACE"))
	{
		return StencilOperator::Replace;
	}

	else if (StringUtilities::IsEqual(string, "INCREMENT_AND_CLAMP"))
	{
		return StencilOperator::IncrementAndClamp;
	}

	else if (StringUtilities::IsEqual(string, "DECREMENT_AND_CLAMP"))
	{
		return StencilOperator::DecrementAndClamp;
	}

	else if (StringUtilities::IsEqual(string, "INVERT"))
	{
		return StencilOperator::Invert;
	}

	else if (StringUtilities::IsEqual(string, "INCREMENT_AND_WRAP"))
	{
		return StencilOperator::IncrementAndWrap;
	}

	else if (StringUtilities::IsEqual(string, "DECREMENT_AND_WRAP"))
	{
		return StencilOperator::DecrementAndWrap;
	}

	else
	{
		ASSERT(false, "Unknown stencil operator string: %s", string);

		return static_cast<StencilOperator>(0);
	}
}


/*
*	Returns the string for the given shader stage.
*/
FORCE_INLINE NO_DISCARD const char *const RESTRICT ShaderStageString(const ShaderStage shader_stage) NOEXCEPT
{
	for (const Pair<ShaderStage, const char *const RESTRICT> &shader_stage_to_string_lookup : SHADER_STAGE_TO_STRING_LOOKUP)
	{
		if (shader_stage_to_string_lookup._First == shader_stage)
		{
			return shader_stage_to_string_lookup._Second;
		}
	}

	ASSERT(false, "Couldn't find shader stage string!");

	return "";
}

/*
*	Shader stage lines class definition.
*/
class ShaderStageLines final
{

public:

	//The shader stage.
	ShaderStage _ShaderStage;

	//The ray tracing hit group this shader belongs to (if any).
	DynamicString _RayTracingHitGroup;

	//The lines.
	DynamicArray<DynamicString> _Lines;

};

/*
*	Default constructor.
*/
RenderPipelineAssetCompiler::RenderPipelineAssetCompiler() NOEXCEPT
{
	//Set the flags.
	_Flags = Flags::ALWAYS_COMPILE;
}

/*
*	Returns the asset type identifier.
*/
NO_DISCARD HashString RenderPipelineAssetCompiler::AssetTypeIdentifier() const NOEXCEPT
{
	return RenderPipelineAsset::TYPE_IDENTIFIER;
}

/*
*	Returns the current version.
*/
NO_DISCARD uint64 RenderPipelineAssetCompiler::CurrentVersion() const NOEXCEPT
{
	return 1;
}

/*
*	Compiles a single asset with the given compile context.
*/
void RenderPipelineAssetCompiler::Compile(const CompileContext &compile_context) NOEXCEPT
{
	//Set up the compile data.
	CompileData *const RESTRICT compile_data{ new (_CompileDataAllocator.Allocate()) CompileData() };

	//Set the collection.
	compile_data->_Collection = compile_context._Collection;

	//Set the file path.
	compile_data->_FilePath = compile_context._FilePath;

	//Set the name.
	compile_data->_Name = compile_context._Name;

	//Set the compilation domain.
	compile_data->_CompilationDomain = compile_context._CompilationDomain;

#if COMPILE_SINGLE_THREADED
	CompileInternal(compile_data);
#else
	//Set up the task.
	Task *const RESTRICT task{ static_cast<Task *const RESTRICT>(compile_context._TaskAllocator->Allocate()) };

	task->_Function = [](void *const RESTRICT arguments)
	{
		RenderPipelineAssetCompiler::Instance->CompileInternal(static_cast<CompileData *const RESTRICT>(arguments));
	};
	task->_Arguments = compile_data;
	task->_ExecutableOnSameThread = false;

	//Execute the task!
	TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, task);

	//Add the task to the list.
	compile_context._Tasks->Emplace(task);
#endif
}

/*
*	Loads a single asset with the given load context.
*/
NO_DISCARD Asset *const RESTRICT RenderPipelineAssetCompiler::Load(const LoadContext &load_context) NOEXCEPT
{
	//Allocate the asset.
	RenderPipelineAsset *const RESTRICT new_asset{ new (_AssetAllocator.Allocate()) RenderPipelineAsset() };

	//Set up the load data.
	LoadData *const RESTRICT load_data{ new (_LoadDataAllocator.Allocate()) LoadData() };

	load_data->_StreamArchivePosition = load_context._StreamArchivePosition;
	load_data->_StreamArchive = load_context._StreamArchive;
	load_data->_Asset = new_asset;

#if LOAD_SINGLE_THREADED
	LoadInternal(load_data);
#else
	//Set up the task.
	Task *const RESTRICT task{ static_cast<Task *const RESTRICT>(load_context._TaskAllocator->Allocate()) };

	task->_Function = [](void *const RESTRICT arguments)
	{
		RenderPipelineAssetCompiler::Instance->LoadInternal(static_cast<LoadData *const RESTRICT>(arguments));
	};
	task->_Arguments = load_data;
	task->_ExecutableOnSameThread = false;

	//Execute the task!
	TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, task);

	//Add the task to the list.
	load_context._Tasks->Emplace(task);
#endif

	//Return the new asset!
	return new_asset;
}

/*
*	Compiles internally.
*/
void RenderPipelineAssetCompiler::CompileInternal(CompileData *const RESTRICT compile_data) NOEXCEPT
{
	PROFILING_SCOPE("RenderPipelineAssetCompiler::CompileInternal");

	//We should be able to store everything directly in an asset and then serialize that.
	RenderPipelineAsset asset;

	//Store extra data.
	ExtraData extra_data;

	//Retrieve the lines.
	DynamicArray<DynamicString> lines;

	//Start with the global render data.
	RetrieveLines(ENGINE_RENDERING_PATH "\\Global Render Data\\GlobalRenderData.global_render_data", &lines);

	//Then add in the lines from the file.
	RetrieveLines(compile_data->_FilePath.Data(), &lines);

	//Resolve includes.
	ResolveIncludes(&lines);

	//Strip comments.
	StripComments(&lines);

	//Consume settings.
	ConsumeSettings(&asset, &extra_data, &lines);

	//Split the shader stages.
	DynamicArray<ShaderStageLines> shader_stages;
	SplitShaderStages(lines, &shader_stages);

	//Compile the GLSL shaders.
	CompileGLSLShaders(*compile_data, extra_data, shader_stages);

	//Determine the collection directory.
	char collection_directory_path[MAXIMUM_FILE_PATH_LENGTH];

	if (compile_data->_Collection)
	{
		sprintf_s(collection_directory_path, "%s\\COLLECTION %s", GetCompiledDirectoryPath(compile_data->_CompilationDomain), compile_data->_Collection.Data());
	}

	else
	{
		sprintf_s(collection_directory_path, "%s\\COLLECTION Default", GetCompiledDirectoryPath(compile_data->_CompilationDomain));
	}

	//Create the compiled directory, if it doesn't exist.
	File::CreateDirectory(collection_directory_path);

	//Determine the directory path.
	char directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(directory_path, "%s\\RenderPipelines", collection_directory_path);

	//Create the compiled directory, if it doesn't exist.
	File::CreateDirectory(directory_path);

	//Determine the output file path.
	char output_file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(output_file_path, "%s\\%s.ca", directory_path, compile_data->_Name.Data());

	//Open the output file.
	BinaryOutputFile output_file{ output_file_path };

	//Write the asset header to the file.
	AssetHeader asset_header{ AssetTypeIdentifier(), CurrentVersion(), HashString(compile_data->_Name.Data()), compile_data->_Name.Data() };
	output_file.Write(&asset_header, sizeof(AssetHeader));

	//Write the common data.
	{
		//Write the input stream subscriptions.
		{
			const uint64 number_of_input_stream_subscriptions{ asset._CommonData._InputStreamSubscriptions.Size() };
			output_file.Write(&number_of_input_stream_subscriptions, sizeof(uint64));

			if (number_of_input_stream_subscriptions > 0)
			{
				output_file.Write(asset._CommonData._InputStreamSubscriptions.Data(), number_of_input_stream_subscriptions * sizeof(HashString));
			}
		}
	}

	//Write the graphics data.
	{
		//Write the topology.
		output_file.Write(&asset._GraphicsData._Topology, sizeof(Topology));

		//Write the polygon mode.
		output_file.Write(&asset._GraphicsData._PolygonMode, sizeof(PolygonMode));

		//Write the depth/stencil load operator.
		output_file.Write(&asset._GraphicsData._DepthStencilLoadOperator, sizeof(AttachmentLoadOperator));

		//Write the depth/stencil store operator.
		output_file.Write(&asset._GraphicsData._DepthStencilStoreOperator, sizeof(AttachmentStoreOperator));

		//Write the color load operator.
		output_file.Write(&asset._GraphicsData._ColorLoadOperator, sizeof(AttachmentLoadOperator));

		//Write the color store operator.
		output_file.Write(&asset._GraphicsData._ColorStoreOperator, sizeof(AttachmentStoreOperator));

		//Write whether or not the depth test is enabled.
		output_file.Write(&asset._GraphicsData._DepthTestEnabled, sizeof(bool));

		//Write whether or not the stencil test is enabled.
		output_file.Write(&asset._GraphicsData._StencilTestEnabled, sizeof(bool));

		//Write whether or not depth write is enabled.
		output_file.Write(&asset._GraphicsData._DepthWriteEnabled, sizeof(bool));

		//Write the depth compare operator.
		output_file.Write(&asset._GraphicsData._DepthCompareOperator, sizeof(CompareOperator));

		//Write the stencil compare operator.
		output_file.Write(&asset._GraphicsData._StencilCompareOperator, sizeof(CompareOperator));

		//Write the stencil fail operator.
		output_file.Write(&asset._GraphicsData._StencilFailOperator, sizeof(StencilOperator));

		//Write the stencil pass operator.
		output_file.Write(&asset._GraphicsData._StencilPassOperator, sizeof(StencilOperator));

		//Write the stencil depth fail operator.
		output_file.Write(&asset._GraphicsData._StencilDepthFailOperator, sizeof(StencilOperator));

		//Write the stencil reference mask.
		output_file.Write(&asset._GraphicsData._StencilReferenceMask, sizeof(uint32));

		//Write the stencil compare mask.
		output_file.Write(&asset._GraphicsData._StencilCompareMask, sizeof(uint32));

		//Write the stencil write mask.
		output_file.Write(&asset._GraphicsData._StencilWriteMask, sizeof(uint32));

		//Write the render resolution.
		output_file.Write(&asset._GraphicsData._RenderResolution, sizeof(HashString));

		//Write the depth buffer.
		output_file.Write(&asset._GraphicsData._DepthBuffer, sizeof(HashString));

		//Write whether or not blend is enabled.
		output_file.Write(&asset._GraphicsData._BlendEnabled, sizeof(bool));

		//Write the blend color source factor.
		output_file.Write(&asset._GraphicsData._BlendColorSourceFactor, sizeof(BlendFactor));

		//Write the blend color destination factor.
		output_file.Write(&asset._GraphicsData._BlendColorDestinationFactor, sizeof(BlendFactor));

		//Write the blend color operator.
		output_file.Write(&asset._GraphicsData._BlendColorOperator, sizeof(BlendOperator));

		//Write the blend alpha source factor.
		output_file.Write(&asset._GraphicsData._BlendAlphaSourceFactor, sizeof(BlendFactor));

		//Write the blend alpha destination factor.
		output_file.Write(&asset._GraphicsData._BlendAlphaDestinationFactor, sizeof(BlendFactor));

		//Write the blend alpha operator.
		output_file.Write(&asset._GraphicsData._BlendAlphaOperator, sizeof(BlendOperator));
	}

	//Close the output file.
	output_file.Close();
}

/*
*	Loads internally.
*/
void RenderPipelineAssetCompiler::LoadInternal(LoadData *const RESTRICT load_data) NOEXCEPT
{
	PROFILING_SCOPE("RenderPipelineAssetCompiler::LoadInternal");

	//Read the data.
	uint64 stream_archive_position{ load_data->_StreamArchivePosition };

	//Read the common data.
	{
		//Read the input stream subscriptions.
		{
			uint64 number_of_input_stream_subscriptions;
			load_data->_StreamArchive->Read(&number_of_input_stream_subscriptions, sizeof(uint64), &stream_archive_position);

			if (number_of_input_stream_subscriptions > 0)
			{
				load_data->_Asset->_CommonData._InputStreamSubscriptions.Upsize<false>(number_of_input_stream_subscriptions);
				load_data->_StreamArchive->Read(load_data->_Asset->_CommonData._InputStreamSubscriptions.Data(), number_of_input_stream_subscriptions * sizeof(HashString), &stream_archive_position);
			}
		}
	}

	//Read the graphics data.
	{
		//Read the topology.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._Topology, sizeof(Topology), &stream_archive_position);

		//Read the polygon mode.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._PolygonMode, sizeof(PolygonMode), &stream_archive_position);

		//Read the depth/stencil load operator.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._DepthStencilLoadOperator, sizeof(AttachmentLoadOperator), &stream_archive_position);

		//Read the depth/stencil store operator.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._DepthStencilLoadOperator, sizeof(AttachmentStoreOperator), &stream_archive_position);

		//Read the color load operator.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._ColorLoadOperator, sizeof(AttachmentLoadOperator), &stream_archive_position);

		//Read the color store operator.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._ColorStoreOperator, sizeof(AttachmentStoreOperator), &stream_archive_position);

		//Read whether or not the depth test is enabled.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._DepthTestEnabled, sizeof(bool), &stream_archive_position);

		//Read whether or not the stencil test is enabled.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._StencilTestEnabled, sizeof(bool), &stream_archive_position);

		//Read whether or not depth write is enabled.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._DepthWriteEnabled, sizeof(bool), &stream_archive_position);

		//Read the depth compare operator.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._DepthCompareOperator, sizeof(CompareOperator), &stream_archive_position);

		//Read the stencil compare operator.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._StencilCompareOperator, sizeof(CompareOperator), &stream_archive_position);

		//Read the stencil fail operator.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._StencilFailOperator, sizeof(StencilOperator), &stream_archive_position);

		//Read the stencil pass operator.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._StencilPassOperator, sizeof(StencilOperator), &stream_archive_position);

		//Read the stencil depth fail operator.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._StencilDepthFailOperator, sizeof(StencilOperator), &stream_archive_position);

		//Read the stencil reference mask.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._StencilReferenceMask, sizeof(uint32), &stream_archive_position);

		//Read the stencil compare mask.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._StencilCompareMask, sizeof(uint32), &stream_archive_position);

		//Read the stencil write mask.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._StencilWriteMask, sizeof(uint32), &stream_archive_position);

		//Read the render resolution.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._RenderResolution, sizeof(HashString), &stream_archive_position);

		//Read the depth buffer.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._DepthBuffer, sizeof(HashString), &stream_archive_position);

		//Read whether or not blend is enabled.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._BlendEnabled, sizeof(bool), &stream_archive_position);

		//Read the blend color source factor.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._BlendColorSourceFactor, sizeof(BlendFactor), &stream_archive_position);

		//Read the blend color destination factor.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._BlendColorDestinationFactor, sizeof(BlendFactor), &stream_archive_position);

		//Read the blend color operator.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._BlendColorOperator, sizeof(BlendOperator), &stream_archive_position);

		//Read the blend alpha source factor.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._BlendAlphaSourceFactor, sizeof(BlendFactor), &stream_archive_position);

		//Read the blend alpha destination factor.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._BlendAlphaDestinationFactor, sizeof(BlendFactor), &stream_archive_position);

		//Read the blend alpha operator.
		load_data->_StreamArchive->Read(&load_data->_Asset->_GraphicsData._BlendAlphaOperator, sizeof(BlendOperator), &stream_archive_position);
	}
}

/*
*	Retrieves all lines from the given file.
*/
void RenderPipelineAssetCompiler::RetrieveLines(const char *const RESTRICT file_path, DynamicArray<DynamicString> *const RESTRICT lines) NOEXCEPT
{
	std::ifstream file{ file_path };

	std::string line;

	while (std::getline(file, line))
	{
		lines->Emplace(line.c_str());
	}

	file.close();
}

/*
*	Strips comments out of the given lines.
*/
void RenderPipelineAssetCompiler::StripComments(DynamicArray<DynamicString> *const RESTRICT lines) NOEXCEPT
{
	for (uint64 i{ 0 }; i < lines->Size();)
	{
		if (TextParsingUtilities::OnlyComment(lines->At(i).Data(), lines->At(i).Length()))
		{
			lines->EraseAt<true>(i);
		}

		else
		{
			++i;
		}
	}
}

/*
*	Searches for the given file, first in the engine directory, then the game directory.
*/
NO_DISCARD bool RenderPipelineAssetCompiler::SearchForFile
(
	const char *const RESTRICT engine_directory_path,
	const char *const RESTRICT game_directory_path,
	char *const RESTRICT file_path
) NOEXCEPT
{
	//Keep a buffer around.
	char buffer[MAXIMUM_FILE_PATH_LENGTH];

	//Check if the file exists in the engine directory path.
	sprintf_s(buffer, "%s\\%s", engine_directory_path, file_path);

	if (File::Exists(buffer))
	{
		sprintf_s(file_path, MAXIMUM_FILE_PATH_LENGTH, "%s", buffer);
		return true;
	}

	//Check if the file exists in the game directory path.
	sprintf_s(buffer, "%s\\%s", game_directory_path, file_path);

	if (File::Exists(buffer))
	{
		sprintf_s(file_path, MAXIMUM_FILE_PATH_LENGTH, "%s", buffer);
		return true;
	}

	//File could not be found!
	return false;
}

/*
*	Resolves an include.
*/
void RenderPipelineAssetCompiler::ResolveInclude
(
	const DynamicString &current_line,
	const char *const RESTRICT extension,
	const char *const RESTRICT sub_folder,
	DynamicArray<DynamicString> *const RESTRICT lines,
	const uint64 current_line_index,
	const char *const RESTRICT replacement_line
) NOEXCEPT
{
	//Retrieve the argument.
	DynamicString argument;

	const uint64 number_of_arguments_parsed
	{
		TextParsingUtilities::ParseFunctionArguments
		(
			current_line.Data(),
			current_line.Length(),
			&argument
		)
	};
	ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments!");

	//Retrieve the file path.
	char file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(file_path, "%s.%s", argument.Data(), extension);

	//Search for the file in both the engine and game directories.
	char engine_directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(engine_directory_path, "%s\\%s", ENGINE_RENDERING_PATH, sub_folder);

	char game_directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(game_directory_path, "%s\\%s", GAME_RENDERING_PATH, sub_folder);

	const bool found_file
	{
		SearchForFile
		(
			engine_directory_path,
			game_directory_path,
			file_path
		)
	};
	ASSERT(found_file, "Couldn't find file!");

	//Retrieve the include lines.
	DynamicArray<DynamicString> include_lines;

	if (replacement_line)
	{
		include_lines.Emplace(replacement_line);
	}

	RetrieveLines(file_path, &include_lines);

	//Erase the line, don't need it anymore.
	lines->EraseAt<true>(current_line_index);

	//Remember the original size.
	const uint64 original_size{ lines->Size() };

	//Make space for the new lines.
	lines->Resize<true>(lines->Size() + include_lines.Size());

	//Move the original lines.
	for (int64 original_line_index{ static_cast<int64>(original_size) - 1 }; original_line_index >= static_cast<int64>(current_line_index); --original_line_index)
	{
		lines->At(original_line_index + include_lines.Size()) = lines->At(original_line_index);
	}

	//Copy in the include lines.
	for (uint64 include_index{ 0 }; include_index < include_lines.Size(); ++include_index)
	{
		lines->At(current_line_index + include_index) = include_lines.At(include_index);
	}
}

/*
*	Resolves includes.
*/
void RenderPipelineAssetCompiler::ResolveIncludes(DynamicArray<DynamicString> *const RESTRICT lines) NOEXCEPT
{
	//Iterate over all lines.
	for (uint64 current_line_index{ 0 }; current_line_index < lines->Size();)
	{
		//Cache the current line.
		const DynamicString &current_line{ lines->At(current_line_index) };

		//Resolve common vertex shader includes.
		if (current_line.Find("IncludeCommonVertexShader("))
		{
			ResolveInclude
			(
				current_line,
				"common_shader",
				"Common Shaders",
				lines,
				current_line_index,
				"Vertex"
			);
		}

		//Resolve common ray closest hit includes.
		else if (current_line.Find("IncludeCommonRayClosestHitShader("))
		{
			ResolveInclude
			(
				current_line,
				"common_shader",
				"Common Shaders",
				lines,
				current_line_index,
				"RayClosestHit"
			);
		}

		//Resolve shader function library includes.
		else if (current_line.Find("IncludeShaderFunctionLibrary("))
		{
			ResolveInclude
			(
				current_line,
				"shader_function_library",
				"Shader Function Libraries",
				lines,
				current_line_index
			);
		}

		//Resolve storage buffer includes.
		else if (current_line.Find("IncludeStorageBuffer("))
		{
			ResolveInclude
			(
				current_line,
				"storage_buffer_definition",
				"Storage Buffer Definitions",
				lines,
				current_line_index
			);
		}

		//Resolve uniform buffer includes.
		else if (current_line.Find("IncludeUniformBuffer("))
		{
			ResolveInclude
			(
				current_line,
				"uniform_buffer_definition",
				"Uniform Buffer Definitions",
				lines,
				current_line_index
			);
		}

		//Otherwise, increment the current line index.
		else
		{
			++current_line_index;
		}
	}
}

/*
*	Consumes settings.
*/
void RenderPipelineAssetCompiler::ConsumeSettings(RenderPipelineAsset *const RESTRICT asset, ExtraData *const RESTRICT extra_data, DynamicArray<DynamicString> *const RESTRICT lines) NOEXCEPT
{
	//Keep track of the current shader stage.
	ShaderStage current_shader_stage{ static_cast<ShaderStage>(0) };

	//Remember the current ray tracing indices.
	uint32 current_ray_any_hit_index{ UINT32_MAXIMUM };
	uint32 current_ray_closest_hit_index{ UINT32_MAXIMUM };
	uint32 current_ray_miss_index{ UINT32_MAXIMUM };

	//Iterate over the lines, and either consume them or add them back (or both).
	for (uint64 line_index{ 0 }; line_index < lines->Size();)
	{
		//Cache the line.
		const DynamicString &line{ lines->At(line_index) };

		//Update the current shader stage.
		for (const Pair<ShaderStage, const char *const RESTRICT> &shader_stage_to_string_lookup : SHADER_STAGE_TO_STRING_LOOKUP)
		{
			if (line == shader_stage_to_string_lookup._Second)
			{
				current_shader_stage = shader_stage_to_string_lookup._First;

				switch (current_shader_stage)
				{
					case ShaderStage::RAY_ANY_HIT:
					{
						++current_ray_any_hit_index;

						break;
					}

					case ShaderStage::RAY_CLOSEST_HIT:
					{
						++current_ray_closest_hit_index;

						break;
					}

					case ShaderStage::RAY_MISS:
					{
						++current_ray_miss_index;

						break;
					}
				}

				break;
			}
		}

		//Is this a blend alpha destination factor declaration?
		if (line.Find("BlendAlphaDestinationFactor("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'BlendAlphaDestinationFactor()'!");

			//Set the value.
			asset->_GraphicsData._BlendAlphaDestinationFactor = ParseBlendFactor(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a blend alpha source factor declaration?
		else if (line.Find("BlendAlphaSourceFactor("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'BlendAlphaSourceFactor()'!");

			//Set the value.
			asset->_GraphicsData._BlendAlphaSourceFactor = ParseBlendFactor(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a blend alpha operator declaration?
		else if (line.Find("BlendAlphaOperator("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'BlendAlphaOperator()'!");

			//Set the value.
			asset->_GraphicsData._BlendAlphaOperator = ParseBlendOperator(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a blend color destination factor declaration?
		else if (line.Find("BlendColorDestinationFactor("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'BlendColorDestinationFactor()'!");

			//Set the value.
			asset->_GraphicsData._BlendColorDestinationFactor = ParseBlendFactor(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a blend color source factor declaration?
		else if (line.Find("BlendColorSourceFactor("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'BlendColorSourceFactor()'!");

			//Set the value.
			asset->_GraphicsData._BlendColorSourceFactor = ParseBlendFactor(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a blend color operator declaration?
		else if (line.Find("BlendColorOperator("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'BlendColorOperator()'!");

			//Set the value.
			asset->_GraphicsData._BlendColorOperator = ParseBlendOperator(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a blend enable declaration?
		else if (line.Find("BlendEnable()"))
		{
			//Set the value.
			asset->_GraphicsData._BlendEnabled = true;

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a compute local size definition?
		else if (line.Find("ComputeLocalSize("))
		{
			//Parse the arguments.
			StaticArray<DynamicString, 3> arguments;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					arguments.Data()
				)
			};
			ASSERT(number_of_arguments_parsed <= 3, "Invalid number of arguments for 'ComputeLocalSize()'!");

			//Fill in the extra data.
			if (number_of_arguments_parsed >= 1)
			{
				extra_data->_ComputeLocalSize._Width = std::stoul(arguments[0].Data());
			}

			else
			{
				extra_data->_ComputeLocalSize._Width = 1;
			}

			if (number_of_arguments_parsed >= 2)
			{
				extra_data->_ComputeLocalSize._Height = std::stoul(arguments[1].Data());
			}

			else
			{
				extra_data->_ComputeLocalSize._Height = 1;
			}

			if (number_of_arguments_parsed >= 3)
			{
				extra_data->_ComputeLocalSize._Depth = std::stoul(arguments[2].Data());
			}

			else
			{
				extra_data->_ComputeLocalSize._Depth = 1;
			}

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a color load operator definition?
		else if (line.Find("ColorLoadOperator("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'ColorLoadOperator()'!");

			//Set the value.
			asset->_GraphicsData._ColorLoadOperator = ParseAttachmentLoadOperator(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a color store operator definition?
		else if (line.Find("ColorStoreOperator("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'ColorStoreOperator()'!");

			//Set the value.
			asset->_GraphicsData._ColorStoreOperator = ParseAttachmentStoreOperator(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a cull mode definition?
		else if (line.Find("CullMode("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'CullMode()'!");

			//Set the value.
			if (argument == "NONE")
			{
				asset->_GraphicsData._CullMode = CullMode::NONE;
			}

			else if (argument == "BACK")
			{
				asset->_GraphicsData._CullMode = CullMode::BACK;
			}

			else if (argument == "FRONT")
			{
				asset->_GraphicsData._CullMode = CullMode::FRONT;
			}

			else if (argument == "FRONT_AND_BACK")
			{
				asset->_GraphicsData._CullMode = CullMode::FRONT_AND_BACK;
			}

			else
			{
				ASSERT(false, "Unknown argument for 'CullMode()': %s", argument.Data());
			}

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a depth compare operator definition?
		else if (line.Find("DepthCompareOperator("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'DepthCompareOperator()'!");

			//Set the value.
			asset->_GraphicsData._DepthCompareOperator = ParseCompareOperator(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a depth/stencil load operator definition?
		else if (line.Find("DepthStencilLoadOperator("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'DepthStencilLoadOperator()'!");

			//Set the value.
			asset->_GraphicsData._DepthStencilLoadOperator = ParseAttachmentLoadOperator(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a depth/stencil store operator definition?
		else if (line.Find("DepthStencilStoreOperator("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'DepthStencilStoreOperator()'!");

			//Set the value.
			asset->_GraphicsData._DepthStencilStoreOperator = ParseAttachmentStoreOperator(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a depth test enable declaration?
		else if (line.Find("DepthTestEnable()"))
		{
			//Set the value.
			asset->_GraphicsData._DepthTestEnabled = true;

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a depth write enable declaration?
		else if (line.Find("DepthWriteEnable()"))
		{
			//Set the value.
			asset->_GraphicsData._DepthWriteEnabled = true;

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a (fragment) input parameter declaration?
		else if (line.Find("InputParameter("))
		{
			//Parse the argument.
			StaticArray<DynamicString, 2> arguments;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					arguments.Data()
				)
			};
			ASSERT(number_of_arguments_parsed == 2, "Invalid number of arguments for 'InputParameter()'!");

			//Cache the input parameters array. This can be for either vertex or fragment shaders.
			DynamicArray<ExtraData::InputParameter> *RESTRICT input_parameters{ nullptr };

			switch (current_shader_stage)
			{
				case ShaderStage::FRAGMENT:
				{
					input_parameters = &extra_data->_FragmentInputParameters;

					break;
				}

				case ShaderStage::VERTEX:
				{
					input_parameters = &extra_data->_VertexInputParameters;

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			//Set the values.
			input_parameters->Emplace();
			ExtraData::InputParameter &value{ input_parameters->Back() };

			value._Type = arguments[0];
			value._Name = arguments[1];

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this an output depth buffer declaration?
		else if (line.Find("OutputDepthBuffer("))
		{
			//Parse the argument.
			StaticArray<DynamicString, 1> arguments;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					arguments.Data()
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'OutputDepthBuffer()'!");

			//Set the value.
			asset->_GraphicsData._DepthBuffer = HashString(arguments[0].Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a (vertex) output parameter declaration?
		else if (line.Find("OutputParameter("))
		{
			//Parse the argument.
			StaticArray<DynamicString, 2> arguments;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					arguments.Data()
				)
			};
			ASSERT(number_of_arguments_parsed == 2, "Invalid number of arguments for 'OutputParameter()'!");

			//Set the values.
			extra_data->_VertexOutputParameters.Emplace();
			ExtraData::OutputParameter &value{ extra_data->_VertexOutputParameters.Back() };

			value._Type = arguments[0];
			value._Name = arguments[1];

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a payload definition?
		else if (line.Find("Payload("))
		{
			//Parse the arguments.
			StaticArray<DynamicString, 3> arguments;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					arguments.Data()
				)
			};
			ASSERT(number_of_arguments_parsed <= 3, "Invalid number of arguments for 'Payload()'!");

			//Fill in depending on the current shader stage.
			switch (current_shader_stage)
			{
				case ShaderStage::RAY_ANY_HIT:
				{
					if (current_ray_any_hit_index >= extra_data->_RayAnyHitPayloads.Size())
					{
						extra_data->_RayAnyHitPayloads.Emplace();
					}

					extra_data->_RayAnyHitPayloads[current_ray_any_hit_index].Emplace();
					extra_data->_RayAnyHitPayloads[current_ray_any_hit_index].Back()._Index = std::stoul(arguments[0].Data());
					extra_data->_RayAnyHitPayloads[current_ray_any_hit_index].Back()._Type = arguments[1];
					extra_data->_RayAnyHitPayloads[current_ray_any_hit_index].Back()._Name = arguments[2];

					break;
				}

				case ShaderStage::RAY_CLOSEST_HIT:
				{
					if (current_ray_closest_hit_index >= extra_data->_RayClosestHitPayloads.Size())
					{
						extra_data->_RayClosestHitPayloads.Emplace();
					}

					extra_data->_RayClosestHitPayloads[current_ray_closest_hit_index].Emplace();
					extra_data->_RayClosestHitPayloads[current_ray_closest_hit_index].Back()._Index = std::stoul(arguments[0].Data());
					extra_data->_RayClosestHitPayloads[current_ray_closest_hit_index].Back()._Type = arguments[1];
					extra_data->_RayClosestHitPayloads[current_ray_closest_hit_index].Back()._Name = arguments[2];

					break;
				}

				case ShaderStage::RAY_GENERATION:
				{
					extra_data->_RayGenerationPayloads.Emplace();
					extra_data->_RayGenerationPayloads.Back()._Index = std::stoul(arguments[0].Data());
					extra_data->_RayGenerationPayloads.Back()._Type = arguments[1];
					extra_data->_RayGenerationPayloads.Back()._Name = arguments[2];

					break;
				}

				case ShaderStage::RAY_MISS:
				{
					if (current_ray_miss_index >= extra_data->_RayMissPayloads.Size())
					{
						extra_data->_RayMissPayloads.Emplace();
					}

					extra_data->_RayMissPayloads[current_ray_miss_index].Emplace();
					extra_data->_RayMissPayloads[current_ray_miss_index].Back()._Index = std::stoul(arguments[0].Data());
					extra_data->_RayMissPayloads[current_ray_miss_index].Back()._Type = arguments[1];
					extra_data->_RayMissPayloads[current_ray_miss_index].Back()._Name = arguments[2];

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a polygon mode definition?
		else if (line.Find("PolygonMode("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'PolygonMode()'!");

			//Set the value.
			if (argument == "FILL")
			{
				asset->_GraphicsData._PolygonMode = PolygonMode::FILL;
			}

			else if (argument == "LINE")
			{
				asset->_GraphicsData._PolygonMode = PolygonMode::LINE;
			}

			else if (argument == "POINT")
			{
				asset->_GraphicsData._PolygonMode = PolygonMode::POINT;
			}

			else
			{
				ASSERT(false, "Unknown argument for 'PolygonMode()': %s", argument.Data());
			}

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a push constant data declaration?
		else if (line.Find("PushConstantData("))
		{
			//Parse the argument.
			StaticArray<DynamicString, 2> arguments;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					arguments.Data()
				)
			};
			ASSERT(number_of_arguments_parsed == 2, "Invalid number of arguments for 'PushConstantData()'!");

			//Set the values.
			extra_data->_PushConstantDataValues.Emplace();
			ExtraData::PushConstantDataValue &value{ extra_data->_PushConstantDataValues.Back() };

			value._Type = arguments[0];
			value._Name = arguments[1];

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a render resolution definition?
		else if (line.Find("RenderResolution("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'RenderResolution()'!");

			//Set the value.
			asset->_GraphicsData._RenderResolution = HashString(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a stencil compare mask definition?
		else if (line.Find("StencilCompareMask("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'StencilCompareMask()'!");

			//Set the value.
			asset->_GraphicsData._StencilCompareMask = std::stoul(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a stencil compare operator definition?
		else if (line.Find("StencilCompareOperator("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'StencilCompareOperator()'!");

			//Set the value.
			asset->_GraphicsData._StencilCompareOperator = ParseCompareOperator(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a stencil depth fail operator definition?
		else if (line.Find("StencilDepthFailOperator("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'StencilPassOperator()'!");

			//Set the value.
			asset->_GraphicsData._StencilDepthFailOperator = ParseStencilOperator(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a stencil fail operator definition?
		else if (line.Find("StencilFailOperator("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'StencilFailOperator()'!");

			//Set the value.
			asset->_GraphicsData._StencilFailOperator = ParseStencilOperator(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a stencil pass operator definition?
		else if (line.Find("StencilPassOperator("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'StencilPassOperator()'!");

			//Set the value.
			asset->_GraphicsData._StencilPassOperator = ParseStencilOperator(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a stencil reference mask definition?
		else if (line.Find("StencilReferenceMask("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'StencilReferenceMask()'!");

			//Set the value.
			asset->_GraphicsData._StencilReferenceMask = std::stoul(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a stencil test enable declaration?
		else if (line.Find("StencilTestEnable()"))
		{
			//Set the value.
			asset->_GraphicsData._StencilTestEnabled = true;

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a stencil write mask definition?
		else if (line.Find("StencilWriteMask("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'StencilWriteMask()'!");

			//Set the value.
			asset->_GraphicsData._StencilWriteMask = std::stoul(argument.Data());

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this an input stream subscription definition?
		else if (line.Find("SubscribeToInputStream("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'SubscribeToInputStream()'!");

			//Set the value.
			asset->_CommonData._InputStreamSubscriptions.Emplace(HashString(argument.Data()));

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Is this a topology definition?
		else if (line.Find("Topology("))
		{
			//Parse the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					line.Data(),
					line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'Topology()'!");

			//Set the value.
			if (argument == "LINE_LIST")
			{
				asset->_GraphicsData._Topology = Topology::LineList;
			}

			else if (argument == "LINE_LIST_WITH_ADJACENCY")
			{
				asset->_GraphicsData._Topology = Topology::LineListWithAdjacency;
			}

			else if (argument == "LINE_STRIP")
			{
				asset->_GraphicsData._Topology = Topology::LineStrip;
			}

			else if (argument == "LINE_STRIP_WITH_ADJACENCY")
			{
				asset->_GraphicsData._Topology = Topology::LineStripWithAdjacency;
			}

			else if (argument == "PATCH_LIST")
			{
				asset->_GraphicsData._Topology = Topology::PatchList;
			}

			else if (argument == "POINT_LIST")
			{
				asset->_GraphicsData._Topology = Topology::PointList;
			}

			else if (argument == "TRIANGLE_FAN")
			{
				asset->_GraphicsData._Topology = Topology::TriangleFan;
			}

			else if (argument == "TRIANGLE_LIST")
			{
				asset->_GraphicsData._Topology = Topology::TriangleList;
			}

			else if (argument == "TRIANGLE_LIST_WITH_ADJACENCY")
			{
				asset->_GraphicsData._Topology = Topology::TriangleListWithAdjacency;
			}

			else if (argument == "TRIANGLE_STRIP")
			{
				asset->_GraphicsData._Topology = Topology::TriangleStrip;
			}

			else if (argument == "TRIANGLE_STRIP_WITH_ADJACENCY")
			{
				asset->_GraphicsData._Topology = Topology::TriangleStripWithAdjacency;
			}

			else
			{
				ASSERT(false, "Unknown argument for 'Topology()': %s", argument.Data());
			}

			//Remove this line.
			lines->EraseAt<true>(line_index);
		}

		//Otherwise, just increment.
		else
		{
			++line_index;
		}
	}
}

/*
*	Splits shader stages.
*/
void RenderPipelineAssetCompiler::SplitShaderStages(const DynamicArray<DynamicString> &lines, DynamicArray<ShaderStageLines> *const RESTRICT shader_stages) NOEXCEPT
{
	//Keep track of the common lines.
	DynamicArray<DynamicString> common_lines;

	//Iterate through the lines.
	bool has_started_parsing_first_shader{ false };
	bool is_parsing_shader{ false };
	uint32 current_parantheses_depth{ 0 };
	ShaderStageLines *RESTRICT current_shader_stage{ nullptr };

	DynamicString current_ray_tracing_hit_group;

	for (uint64 current_line_index{ 0 }; current_line_index < lines.Size(); ++current_line_index)
	{
		//Cache the current line.
		const DynamicString &current_line{ lines[current_line_index] };

		//Is this a hit group declaration?
		if (current_line.Find("HitGroup("))
		{
			//Retrieve the argument.
			DynamicString argument;

			const uint64 number_of_arguments_parsed
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					current_line.Data(),
					current_line.Length(),
					&argument
				)
			};
			ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments for 'HitGroup()'!");

			//Set the current ray tracing hit group
			current_ray_tracing_hit_group = argument;
		}

		//If we're currently parsing a shader, add the lines.
		if (is_parsing_shader)
		{
			//Add the line.
			current_shader_stage->_Lines.Emplace(current_line);

			//Update the current parantheses depth.
			for (uint64 i{ 0 }; i < current_line.Length(); ++i)
			{
				if (current_line[i] == '{')
				{
					++current_parantheses_depth;
				}

				else if (current_line[i] == '}')
				{
					ASSERT(current_parantheses_depth > 0, "Invalid parentheses depth!");
					--current_parantheses_depth;
				}
			}

			//If we reached the end, stop parsing.
			if (current_parantheses_depth == 0)
			{
				is_parsing_shader = false;
				current_shader_stage = nullptr;
			}
		}

		else
		{
			//Check if this is potentially the start of a shader.
			for (const Pair<ShaderStage, const char *const RESTRICT> &shader_stage_to_string_lookup : SHADER_STAGE_TO_STRING_LOOKUP)
			{
				if (current_line == shader_stage_to_string_lookup._Second)
				{
					has_started_parsing_first_shader = true;
					is_parsing_shader = true;

					shader_stages->Emplace();
					current_shader_stage = &shader_stages->Back();

					current_shader_stage->_ShaderStage = shader_stage_to_string_lookup._First;

					for (const DynamicString &common_line : common_lines)
					{
						current_shader_stage->_Lines.Emplace(common_line);
					}

					//Add the entry point marker.
					current_shader_stage->_Lines.Emplace("__ENTRYPOINT");

					//If this is a shader that's supposed to be part of a hit group, add the current hit ray tracing hit group to the lines.
					if (current_shader_stage->_ShaderStage == ShaderStage::RAY_ANY_HIT
						|| current_shader_stage->_ShaderStage == ShaderStage::RAY_CLOSEST_HIT)
					{
						current_shader_stage->_RayTracingHitGroup = current_ray_tracing_hit_group;
					}

					break;
				}
			}

			//If we haven't started parsing the first shader, add to the common lines.
			if (!has_started_parsing_first_shader)
			{
				common_lines.Emplace(current_line);
			}
		}
	}
}

/*
*	Compiles GLSL shaders.
*/
void RenderPipelineAssetCompiler::CompileGLSLShaders
(
	const CompileData &compile_data,
	const ExtraData &extra_data,
	const DynamicArray<ShaderStageLines> &shader_stages
) NOEXCEPT
{
	//Define constants.
	constexpr auto AddRayTracingData
	{
		[](DynamicArray<DynamicString> *const RESTRICT glsl_lines, const char *const RESTRICT hit_group = nullptr)
		{
			//TODO: This should not be hardcoded.
			constexpr const char *const RESTRICT HIT_GROUPS[]
			{
				"OpaqueModels",
				"MaskedModels"
			};

			char buffer[256];

			uint32 current_binding{ 0 };

			sprintf_s(buffer, "layout (set = 2, binding = %u) uniform accelerationStructureNV TOP_LEVEL_ACCELERATION_STRUCTURE;", current_binding++);
			glsl_lines->Emplace(buffer);

			for (const char *const RESTRICT _hit_group : HIT_GROUPS)
			{
				sprintf_s(buffer, "layout (set = 2, binding = %u) buffer %s_VERTEX_DATA_BUFFER { vec4 %s_VERTEX_DATA[]; } %s_VERTEX_BUFFERS[];", current_binding++, _hit_group, _hit_group, _hit_group);
				glsl_lines->Emplace(buffer);

				sprintf_s(buffer, "layout (set = 2, binding = %u) buffer %s_INDEX_DATA_BUFFER { uint %s_INDEX_DATA[]; } %s_INDEX_BUFFERS[];", current_binding++, _hit_group, _hit_group, _hit_group);
				glsl_lines->Emplace(buffer);

				sprintf_s(buffer, "layout (set = 2, binding = %u) buffer %s_MATERIAL_BUFFER { layout (offset = 0) uvec4[] %s_MATERIAL_INDICES; };", current_binding++, _hit_group, _hit_group);
				glsl_lines->Emplace(buffer);
			}

			glsl_lines->Emplace("");

			if (hit_group)
			{
				glsl_lines->Emplace("hitAttributeNV vec3 HIT_ATTRIBUTE;");
				glsl_lines->Emplace("");

				glsl_lines->Emplace("struct HitVertexInformation");
				glsl_lines->Emplace("{");
				glsl_lines->Emplace("\tvec3 _Position;");
				glsl_lines->Emplace("\tvec3 _Normal;");
				glsl_lines->Emplace("\tvec3 _Tangent;");
				glsl_lines->Emplace("\tvec2 _TextureCoordinate;");
				glsl_lines->Emplace("};");
				glsl_lines->Emplace("");

				glsl_lines->Emplace("HitVertexInformation GetHitVertexInformation()");
				glsl_lines->Emplace("{");
				glsl_lines->Emplace("\tHitVertexInformation hit_vertex_information;");
				glsl_lines->Emplace("\t");

				for (uint32 vertex_index{ 0 }; vertex_index < 3; ++vertex_index)
				{
					sprintf_s(buffer, "\tuint vertex_index_%u = %s_INDEX_BUFFERS[gl_InstanceCustomIndexNV].%s_INDEX_DATA[gl_PrimitiveID * 3 + %u];", vertex_index, hit_group, hit_group, vertex_index);
					glsl_lines->Emplace(buffer);

					for (uint32 vertex_data_index{ 0 }; vertex_data_index < 3; ++vertex_data_index)
					{
						sprintf_s(buffer, "\tvec4 vertex_data_%u_%u = %s_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].%s_VERTEX_DATA[3 * vertex_index_%u + %u];", vertex_index, vertex_data_index, hit_group, hit_group, vertex_index, vertex_data_index);
						glsl_lines->Emplace(buffer);
					}
				}

				glsl_lines->Emplace("\t");

				glsl_lines->Emplace("\tvec3 barycentric_coordinates = vec3(1.0f - HIT_ATTRIBUTE.x - HIT_ATTRIBUTE.y, HIT_ATTRIBUTE.x, HIT_ATTRIBUTE.y);");
				glsl_lines->Emplace("\t");

				glsl_lines->Emplace("\thit_vertex_information._Position = ");

				for (uint32 vertex_index{ 0 }; vertex_index < 3; ++vertex_index)
				{
					sprintf_s(buffer, "\tvec3(vertex_data_%u_0.x, vertex_data_%u_0.y, vertex_data_%u_0.z) * barycentric_coordinates[%u]", vertex_index, vertex_index, vertex_index, vertex_index);
					glsl_lines->Emplace(buffer);

					if (vertex_index < 2)
					{
						glsl_lines->Emplace("\t+");
					}

					else
					{
						glsl_lines->Emplace("\t;");
					}
				}

				glsl_lines->Emplace("\t");

				glsl_lines->Emplace("\thit_vertex_information._Position = gl_ObjectToWorldNV * vec4(hit_vertex_information._Normal, 1.0f);");
				glsl_lines->Emplace("\t");

				glsl_lines->Emplace("\thit_vertex_information._Normal = ");

				for (uint32 vertex_index{ 0 }; vertex_index < 3; ++vertex_index)
				{
					sprintf_s(buffer, "\tvec3(vertex_data_%u_0.w, vertex_data_%u_1.x, vertex_data_%u_1.y) * barycentric_coordinates[%u]", vertex_index, vertex_index, vertex_index, vertex_index);
					glsl_lines->Emplace(buffer);

					if (vertex_index < 2)
					{
						glsl_lines->Emplace("\t+");
					}

					else
					{
						glsl_lines->Emplace("\t;");
					}
				}

				glsl_lines->Emplace("\t");

				glsl_lines->Emplace("\thit_vertex_information._Normal = normalize(gl_ObjectToWorldNV * vec4(hit_vertex_information._Normal, 0.0f));");
				glsl_lines->Emplace("\t");

				glsl_lines->Emplace("\thit_vertex_information._Tangent = ");

				for (uint32 vertex_index{ 0 }; vertex_index < 3; ++vertex_index)
				{
					sprintf_s(buffer, "\tvec3(vertex_data_%u_1.z, vertex_data_%u_1.w, vertex_data_%u_2.x) * barycentric_coordinates[%u]", vertex_index, vertex_index, vertex_index, vertex_index);
					glsl_lines->Emplace(buffer);

					if (vertex_index < 2)
					{
						glsl_lines->Emplace("\t+");
					}

					else
					{
						glsl_lines->Emplace("\t;");
					}
				}

				glsl_lines->Emplace("\t");

				glsl_lines->Emplace("\thit_vertex_information._Tangent = normalize(gl_ObjectToWorldNV * vec4(hit_vertex_information._Tangent, 0.0f));");
				glsl_lines->Emplace("\t");

				glsl_lines->Emplace("\thit_vertex_information._TextureCoordinate = ");

				for (uint32 vertex_index{ 0 }; vertex_index < 3; ++vertex_index)
				{
					sprintf_s(buffer, "\tvec2(vertex_data_%u_2.y, vertex_data_%u_2.z) * barycentric_coordinates[%u]", vertex_index, vertex_index, vertex_index);
					glsl_lines->Emplace(buffer);

					if (vertex_index < 2)
					{
						glsl_lines->Emplace("\t+");
					}

					else
					{
						glsl_lines->Emplace("\t;");
					}
				}

				glsl_lines->Emplace("\t");

				glsl_lines->Emplace("\treturn hit_vertex_information;");
				glsl_lines->Emplace("}");
				glsl_lines->Emplace("");

				glsl_lines->Emplace("uint GetHitMaterialIndex()");
				glsl_lines->Emplace("{");

				sprintf_s(buffer, "\treturn %s_MATERIAL_INDICES[gl_InstanceCustomIndexNV / 4][gl_InstanceCustomIndexNV & 3];", hit_group);
				glsl_lines->Emplace(buffer);

				glsl_lines->Emplace("}");
				glsl_lines->Emplace("");
			}
		}
	};
	constexpr auto AddRayTracingPayloads
	{
		[](const DynamicArray<ExtraData::RayTracingPayload> &payloads, DynamicArray<DynamicString> *const RESTRICT glsl_lines)
		{
			if (!payloads.Empty())
			{
				for (const ExtraData::RayTracingPayload &payload : payloads)
				{
					char buffer[64];
					sprintf_s(buffer, "layout (location = %u) rayPayloadNV %s %s;", payload._Index, payload._Type.Data(), payload._Name.Data());

					glsl_lines->Emplace(buffer);
				}

				glsl_lines->Emplace("");
			}
		}
	};

	//Remember the current ray tracing indices.
	uint32 current_ray_any_hit_index{ UINT32_MAXIMUM };
	uint32 current_ray_closest_hit_index{ UINT32_MAXIMUM };
	uint32 current_ray_miss_index{ UINT32_MAXIMUM };

	//Compile all shader stages.
	for (const ShaderStageLines &shader_stage : shader_stages)
	{
		//Increment the current ray tracing indices.
		switch (shader_stage._ShaderStage)
		{
			case ShaderStage::RAY_ANY_HIT:
			{
				++current_ray_any_hit_index;

				break;
			}

			case ShaderStage::RAY_CLOSEST_HIT:
			{
				++current_ray_closest_hit_index;

				break;
			}

			case ShaderStage::RAY_MISS:
			{
				++current_ray_miss_index;

				break;
			}
		}

		//Create a separate set of the GLSL-ified lines.
		DynamicArray<DynamicString> glsl_lines;

		//Insert the version.
		glsl_lines.Emplace("#version 460");
		glsl_lines.Emplace("");

		//Add global extensions.
		glsl_lines.Emplace("#extension GL_EXT_nonuniform_qualifier : require");
		glsl_lines.Emplace("");

		//Insert shader stage specific extensions.
		switch (shader_stage._ShaderStage)
		{
			case ShaderStage::RAY_ANY_HIT:
			{
				glsl_lines.Emplace("#extension GL_NV_ray_tracing : require");
				glsl_lines.Emplace("");

				break;
			}

			case ShaderStage::RAY_CLOSEST_HIT:
			{
				glsl_lines.Emplace("#extension GL_NV_ray_tracing : require");
				glsl_lines.Emplace("");

				break;
			}

			case ShaderStage::RAY_GENERATION:
			{
				glsl_lines.Emplace("#extension GL_NV_ray_tracing : require");
				glsl_lines.Emplace("");

				break;
			}

			case ShaderStage::RAY_MISS:
			{
				glsl_lines.Emplace("#extension GL_NV_ray_tracing : require");
				glsl_lines.Emplace("");

				break;
			}
		}

		//Iterate over the shader stage lines, GLSL-ifying each line.
		uint32 current_binding_index{ 0 };
		uint32 location_index{ 0 };

		for (uint64 current_line_index{ 0 }; current_line_index < shader_stage._Lines.Size(); ++current_line_index)
		{
			//Cache the current line.
			DynamicString current_line{ shader_stage._Lines[current_line_index] };

			//////////////////////////////
			// MULTI LINE MODIFICATIONS //
			//////////////////////////////

			//Handle storage buffer declarations.
			if (current_line.Find("StorageBuffer("))
			{
				//Parse the buffer name.
				DynamicString buffer_name;

				const uint64 number_of_arguments_parsed
				{
					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.Data(),
						current_line.Length(),
						&buffer_name
					)
				};
				ASSERT(number_of_arguments_parsed == 1, "Wrong number of arguments for 'StorageBuffer()' declaration!");

				//Insert the header.
				char header[128];
				sprintf_s(header, "layout (std430, set = 1, binding = %u) buffer %s", current_binding_index++, buffer_name.Data());

				glsl_lines.Emplace(header);

				//Iterate over the rest of the lines.
				uint64 current_offset{ 0 };

				while (true)
				{
					//Increment the current line index.
					++current_line_index;

					//Cache the current sub line.
					const DynamicString &current_sub_line{ shader_stage._Lines[current_line_index] };

					//If the line is only whitespace, ignore it.
					if (TextParsingUtilities::OnlyWhitespace(current_sub_line.Data(), current_sub_line.Length()))
					{
						continue;
					}

					//If this is the opening bracket, add it.
					if (current_sub_line == "{")
					{
						glsl_lines.Emplace(current_sub_line);
					}

					//If this is the ending bracket, add it and quit.
					else if (current_sub_line == "};")
					{
						glsl_lines.Emplace(current_sub_line);

						break;
					}

					//Otherwise, parse the type and name and output the GLSL-ified line.
					else
					{
						uint64 type_begin{ 0 };

						for (uint64 i{ 0 }; i < current_sub_line.Length(); ++i)
						{
							if (TextParsingUtilities::IsWhitespace(current_sub_line[i]))
							{
								++type_begin;
							}

							else
							{
								break;
							}
						}

						uint64 type_end{ type_begin };

						for (uint64 i{ type_begin }; i < current_sub_line.Length(); ++i)
						{
							if (!TextParsingUtilities::IsWhitespace(current_sub_line[i]))
							{
								++type_end;
							}

							else
							{
								break;
							}
						}

						const uint64 type_length{ type_end - type_begin };

						char type[32];

						for (uint64 i{ 0 }; i < type_length; ++i)
						{
							type[i] = current_sub_line[type_begin + i];
						}

						type[type_length] = '\0';

						const uint64 name_begin{ type_end + 1 };

						char name[64];

						for (uint64 i{ 0 };; ++i)
						{
							if ((name_begin + i) >= current_sub_line.Length() || current_sub_line[(name_begin + i)] == ';')
							{
								name[i] = '\0';

								break;
							}

							else
							{
								name[i] = current_sub_line[(name_begin + i)];
							}
						}

						char glsl_line[128];
						sprintf_s(glsl_line, "\tlayout (offset = %llu) %s %s;", current_offset, type, name);

						glsl_lines.Emplace(glsl_line);

						//Update the current offset.
						{
							//Figure out if this is an array.
							uint64 array_count{ 0 };

							{
								const char *const RESTRICT array_start_position{ current_sub_line.Find("[") };

								if (array_start_position)
								{
									char array_count_buffer[8];

									for (uint64 i{ 0 };; ++i)
									{
										if (array_start_position[1 + i] == ']' || array_start_position[1 + i] == '\0')
										{
											array_count_buffer[i] = '\0';

											break;
										}

										else
										{
											array_count_buffer[i] = array_start_position[1 + i];
										}
									}

									if (array_count_buffer[0] != '\0')
									{
										array_count = std::stoull(array_count_buffer);
									}
								}
							}

							//Update the current offset. Assume 16 bytes for each array element if this is an array.
							if (array_count > 0)
							{
								current_offset += 16 * array_count;
							}

							else
							{
								//Offsets are always 16 for storage buffers.
								current_offset += 16;
							}
						}
					}
				}

				continue;
			}

			//Handle 'TraceRay()' calls.
			if (current_line.Find("TraceRay("))
			{
				//Parse the arguments
				StaticArray<DynamicString, 6> arguments;

				const uint64 number_of_arguments_parsed
				{
					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.Data(),
						current_line.Length(),
						arguments.Data()
					)
				};
				ASSERT(number_of_arguments_parsed == 6, "Wrong number of arguments for 'TraceRay()'!");

				//Output the lines.
				char buffer[64];

				glsl_lines.Emplace("traceNV");
				glsl_lines.Emplace("(");
				glsl_lines.Emplace("\tTOP_LEVEL_ACCELERATION_STRUCTURE, /*topLevel*/");

				sprintf_s(buffer, "\t%s, /*rayFlags*/", arguments[0].Data());
				glsl_lines.Emplace(buffer);

				glsl_lines.Emplace("\t0xff, /*cullMask*/");
				glsl_lines.Emplace("\t0, /*sbtRecordOffset*/");
				glsl_lines.Emplace("\t0, /*sbtRecordStride*/");

				sprintf_s(buffer, "\t%s, /*missIndex*/", arguments[1].Data());
				glsl_lines.Emplace(buffer);

				sprintf_s(buffer, "\t%s, /*origin*/", arguments[2].Data());
				glsl_lines.Emplace(buffer);

				glsl_lines.Emplace("\tFLOAT32_EPSILON * 128.0f, /*Tmin*/");

				sprintf_s(buffer, "\t%s, /*direction*/", arguments[3].Data());
				glsl_lines.Emplace(buffer);

				sprintf_s(buffer, "\t%s, /*Tmax*/", arguments[4].Data());
				glsl_lines.Emplace(buffer);

				sprintf_s(buffer, "\t%s /*payload*/", arguments[5].Data());
				glsl_lines.Emplace(buffer);

				glsl_lines.Emplace(");");

				continue;
			}

			//Handle uniform buffer declarations.
			if (current_line.Find("UniformBuffer("))
			{
				//Parse the buffer name.
				DynamicString buffer_name;

				const uint64 number_of_arguments_parsed
				{
					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.Data(),
						current_line.Length(),
						&buffer_name
					)
				};
				ASSERT(number_of_arguments_parsed == 1, "Wrong number of arguments for 'UniformBuffer()' declaration!");

				//Insert the header.
				char header[128];
				sprintf_s(header, "layout (std140, set = 1, binding = %u) uniform %s", current_binding_index++, buffer_name.Data());

				glsl_lines.Emplace(header);

				//Iterate over the rest of the lines.
				uint64 current_offset{ 0 };

				while (true)
				{
					//Increment the current line index.
					++current_line_index;

					//Cache the current sub line.
					const DynamicString &current_sub_line{ shader_stage._Lines[current_line_index] };

					//If the line is only whitespace, ignore it.
					if (TextParsingUtilities::OnlyWhitespace(current_sub_line.Data(), current_sub_line.Length()))
					{
						continue;
					}

					//If this is the opening bracket, add it.
					if (current_sub_line == "{")
					{
						glsl_lines.Emplace(current_sub_line);
					}

					//If this is the ending bracket, add it and quit.
					else if (current_sub_line == "};")
					{
						glsl_lines.Emplace(current_sub_line);

						break;
					}

					//Otherwise, parse the type and name and output the GLSL-ified line.
					else
					{
						uint64 type_begin{ 0 };

						for (uint64 i{ 0 }; i < current_sub_line.Length(); ++i)
						{
							if (TextParsingUtilities::IsWhitespace(current_sub_line[i]))
							{
								++type_begin;
							}

							else
							{
								break;
							}
						}

						uint64 type_end{ type_begin };

						for (uint64 i{ type_begin }; i < current_sub_line.Length(); ++i)
						{
							if (!TextParsingUtilities::IsWhitespace(current_sub_line[i]))
							{
								++type_end;
							}

							else
							{
								break;
							}
						}

						const uint64 type_length{ type_end - type_begin };

						char type[8];

						for (uint64 i{ 0 }; i < type_length; ++i)
						{
							type[i] = current_sub_line[type_begin + i];
						}

						type[type_length] = '\0';

						const uint64 name_begin{ type_end + 1 };

						char name[64];

						for (uint64 i{ 0 };; ++i)
						{
							if ((name_begin + i) >= current_sub_line.Length() || current_sub_line[(name_begin + i)] == ';')
							{
								name[i] = '\0';

								break;
							}

							else
							{
								name[i] = current_sub_line[(name_begin + i)];
							}
						}

						char glsl_line[128];
						sprintf_s(glsl_line, "\tlayout (offset = %llu) %s %s;", current_offset, type, name);

						glsl_lines.Emplace(glsl_line);

						//Update the current offset.
						{
							//Figure out if this is an array.
							uint64 array_count{ 0 };

							{
								const char *const RESTRICT array_start_position{ current_sub_line.Find("[") };

								if (array_start_position)
								{
									char array_count_buffer[8];

									for (uint64 i{ 0 };; ++i)
									{
										if (array_start_position[1 + i] == ']' || array_start_position[1 + i] == '\0')
										{
											array_count_buffer[i] = '\0';

											break;
										}

										else
										{
											array_count_buffer[i] = array_start_position[1 + i];
										}
									}

									array_count = std::stoull(array_count_buffer);
								}
							}

							//Update the current offset. Assume 16 bytes for each array element if this is an array.
							if (array_count > 0)
							{
								current_offset += 16 * array_count;
							}

							else
							{
								current_offset += GLSLCompilation::GetByteOffsetForType(type);
							}
						}
					}
				}

				continue;
			}

			///////////////////////////////
			// SINGLE LINE MODIFICATIONS //
			///////////////////////////////

			//Handle the entrypoint.
			if (current_line.Find("__ENTRYPOINT"))
			{
				//Add the push constant data.
				if (!extra_data._PushConstantDataValues.Empty())
				{
					glsl_lines.Emplace("layout (push_constant) uniform PushConstantData");
					glsl_lines.Emplace("{");

					uint64 current_offset{ 0 };

					for (const ExtraData::PushConstantDataValue &value : extra_data._PushConstantDataValues)
					{
						char buffer[64];
						sprintf_s(buffer, "\tlayout (offset = %llu) %s %s;", current_offset, value._Type.Data(), value._Name.Data());

						glsl_lines.Emplace(buffer);

						current_offset += GLSLCompilation::GetByteOffsetForType(value._Type.Data());
					}

					glsl_lines.Emplace("};");
					glsl_lines.Emplace("");
				}

				//This is a good time to add shader stage specific stuff.
				switch (shader_stage._ShaderStage)
				{
					case ShaderStage::COMPUTE:
					{
						//Add the compute local size.
						{
							char buffer[128];
							sprintf_s(buffer, "layout (local_size_x = %u, local_size_y = %u, local_size_z = %u) in;", extra_data._ComputeLocalSize._Width, extra_data._ComputeLocalSize._Height, extra_data._ComputeLocalSize._Depth);
						
							glsl_lines.Emplace(buffer);
							glsl_lines.Emplace("");
						}

						//Add the "ComputeWorkGroupSize()" function.
						{
							char buffer[128];

							sprintf_s(buffer, "%s", "uvec3 ComputeWorkGroupSize()");
							glsl_lines.Emplace(buffer);

							sprintf_s(buffer, "%s", "{");
							glsl_lines.Emplace(buffer);

							sprintf_s(buffer, "\treturn uvec3(%u, %u, %u);", extra_data._ComputeLocalSize._Width, extra_data._ComputeLocalSize._Height, extra_data._ComputeLocalSize._Depth);
							glsl_lines.Emplace(buffer);

							sprintf_s(buffer, "%s", "}");
							glsl_lines.Emplace(buffer);

							glsl_lines.Emplace("");
						}

						//Add the "ComputeDimensions()" function.
						{
							char buffer[128];

							sprintf_s(buffer, "%s", "uvec3 ComputeDimensions()");
							glsl_lines.Emplace(buffer);

							sprintf_s(buffer, "%s", "{");
							glsl_lines.Emplace(buffer);

							sprintf_s(buffer, "%s", "\treturn ComputeWorkGroupSize() * gl_NumWorkGroups;");
							glsl_lines.Emplace(buffer);

							sprintf_s(buffer, "%s", "}");
							glsl_lines.Emplace(buffer);

							glsl_lines.Emplace("");
						}

						break;
					}

					case ShaderStage::FRAGMENT:
					{
						//Add the input parameters.
						if (!extra_data._FragmentInputParameters.Empty())
						{
							for (const ExtraData::InputParameter &parameter : extra_data._FragmentInputParameters)
							{
								char buffer[64];
								sprintf_s(buffer, "layout (location = %u) %s in %s %s;", location_index, parameter._Type == "uint" ? "flat" : "", parameter._Type.Data(), parameter._Name.Data());

								glsl_lines.Emplace(buffer);

								location_index += GLSLCompilation::GetLocationOffsetForType(parameter._Type.Data());
							}

							glsl_lines.Emplace("");
						}

						break;
					}

					case ShaderStage::RAY_ANY_HIT:
					{
						//Add the ray tracing data.
						AddRayTracingData(&glsl_lines, shader_stage._RayTracingHitGroup.Data());

						//Add the ray generation payloads.
						if (current_ray_any_hit_index < extra_data._RayAnyHitPayloads.Size() && !extra_data._RayAnyHitPayloads[current_ray_any_hit_index].Empty())
						{
							AddRayTracingPayloads(extra_data._RayAnyHitPayloads[current_ray_any_hit_index], &glsl_lines);
						}

						break;
					}

					case ShaderStage::RAY_CLOSEST_HIT:
					{
						//Add the ray tracing data.
						AddRayTracingData(&glsl_lines, shader_stage._RayTracingHitGroup.Data());

						//Add the ray generation payloads.
						AddRayTracingPayloads(extra_data._RayClosestHitPayloads[current_ray_closest_hit_index], &glsl_lines);

						break;
					}

					case ShaderStage::RAY_GENERATION:
					{
						//Add the ray tracing data.
						AddRayTracingData(&glsl_lines);

						//Add the ray generation payloads.
						AddRayTracingPayloads(extra_data._RayGenerationPayloads, &glsl_lines);

						break;
					}

					case ShaderStage::RAY_MISS:
					{
						//Add the ray tracing data.
						AddRayTracingData(&glsl_lines);

						//Add the ray generation payloads.
						AddRayTracingPayloads(extra_data._RayMissPayloads[current_ray_miss_index], &glsl_lines);

						break;
					}

					case ShaderStage::VERTEX:
					{
						//Add the input parameters.
						if (!extra_data._VertexInputParameters.Empty())
						{
							for (const ExtraData::InputParameter &parameter : extra_data._VertexInputParameters)
							{
								char buffer[64];
								sprintf_s(buffer, "layout (location = %u) in %s %s;", location_index, parameter._Type.Data(), parameter._Name.Data());

								glsl_lines.Emplace(buffer);

								location_index += GLSLCompilation::GetLocationOffsetForType(parameter._Type.Data());
							}

							glsl_lines.Emplace("");
						}

						//Add the output parameters.
						if (!extra_data._VertexOutputParameters.Empty())
						{
							for (const ExtraData::OutputParameter &parameter : extra_data._VertexOutputParameters)
							{
								char buffer[64];
								sprintf_s(buffer, "layout (location = %u) out %s %s;", location_index, parameter._Type.Data(), parameter._Name.Data());

								glsl_lines.Emplace(buffer);

								location_index += GLSLCompilation::GetLocationOffsetForType(parameter._Type.Data());
							}

							glsl_lines.Emplace("");
						}

						break;
					}
				}

				//Modify the line.
				current_line = "void main()";
			}

			//Replace "COMPUTE_GLOBAL_ID" with "gl_GlobalInvocationID".
			if (current_line.Find("COMPUTE_GLOBAL_ID"))
			{
				std::string _current_line{ current_line.Data() };

				size_t position{ _current_line.find("COMPUTE_GLOBAL_ID") };

				while (position != std::string::npos)
				{
					_current_line.replace(position, strlen("COMPUTE_GLOBAL_ID"), "gl_GlobalInvocationID");
					position = _current_line.find("COMPUTE_GLOBAL_ID");
				}

				current_line = _current_line.c_str();
			}

			//Handle compute render targets.
			if (current_line.Find("ComputeRenderTarget("))
			{
				//Parse the arguments.
				StaticArray<DynamicString, 2> arguments;

				const uint64 number_of_arguments_parsed
				{
					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.Data(),
						current_line.Length(),
						arguments.Data()
					)
				};
				ASSERT(number_of_arguments_parsed == 2, "Wrong number of arguments for 'ComputeRenderTarget()' declaration!");

				//Figure out the format string.
				const char *RESTRICT format_string;

				if (arguments[1] == "R_UINT8")
				{
					format_string = "r8";
				}

				else if (arguments[1] == "RGBA_UINT8")
				{
					format_string = "rgba8";
				}

				else if (arguments[1] == "RGBA_FLOAT32")
				{
					format_string = "rgba32f";
				}

				else
				{
					ASSERT(false, "Unknown compute render target format!");
				}

				//Construct the GLSL line and add it.
				char glsl_line[128];
				sprintf_s(glsl_line, "layout (set = 1, binding = %u, %s) uniform image2D %s;", current_binding_index++, format_string, arguments[0].Data());

				current_line = glsl_line;
			}

			//Handle external textures.
			if (current_line.Find("ExternalTexture("))
			{
				//Parse the arguments.
				StaticArray<DynamicString, 1> arguments;

				const uint64 number_of_arguments_parsed
				{
					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.Data(),
						current_line.Length(),
						arguments.Data()
					)
				};
				ASSERT(number_of_arguments_parsed == 1, "Wrong number of arguments for 'ExternalTexture()' declaration!");

				//Construct the GLSL line and add it.
				char glsl_line[128];
				sprintf_s(glsl_line, "layout (set = 1, binding = %u) uniform sampler2D %s;", current_binding_index++, arguments[0].Data());

				current_line = glsl_line;
			}

			//Replace "FRAGMENT_COORDINATE" with "gl_FragCoord".
			if (current_line.Find("FRAGMENT_COORDINATE"))
			{
				std::string _current_line{ current_line.Data() };

				size_t position{ _current_line.find("FRAGMENT_COORDINATE") };

				while (position != std::string::npos)
				{
					_current_line.replace(position, strlen("FRAGMENT_COORDINATE"), "gl_FragCoord");
					position = _current_line.find("FRAGMENT_COORDINATE");
				}

				current_line = _current_line.c_str();
			}

			//Replace "FRAGMENT_FRONT_FACING" with "gl_FrontFacing".
			if (current_line.Find("FRAGMENT_FRONT_FACING"))
			{
				std::string _current_line{ current_line.Data() };

				size_t position{ _current_line.find("FRAGMENT_FRONT_FACING") };

				while (position != std::string::npos)
				{
					_current_line.replace(position, strlen("FRAGMENT_FRONT_FACING"), "gl_FrontFacing");
					position = _current_line.find("FRAGMENT_FRONT_FACING");
				}

				current_line = _current_line.c_str();
			}

			//Replace "IgnoreHit()" with "ignoreIntersectionNV()".
			if (current_line.Find("IgnoreHit()"))
			{
				std::string _current_line{ current_line.Data() };

				size_t position{ _current_line.find("IgnoreHit()") };

				while (position != std::string::npos)
				{
					_current_line.replace(position, strlen("IgnoreHit()"), "ignoreIntersectionNV()");
					position = _current_line.find("IgnoreHit()");
				}

				current_line = _current_line.c_str();
			}

			//Replace "ImageLoad" with "imageLoad".
			if (current_line.Find("ImageLoad"))
			{
				std::string _current_line{ current_line.Data() };

				size_t position{ _current_line.find("ImageLoad") };

				while (position != std::string::npos)
				{
					_current_line.replace(position, strlen("ImageLoad"), "imageLoad");
					position = _current_line.find("ImageLoad");
				}

				current_line = _current_line.c_str();
			}

			//Replace "ImageStore" with "imageStore".
			if (current_line.Find("ImageStore"))
			{
				std::string _current_line{ current_line.Data() };

				size_t position{ _current_line.find("ImageStore") };

				while (position != std::string::npos)
				{
					_current_line.replace(position, strlen("ImageStore"), "imageStore");
					position = _current_line.find("ImageStore");
				}

				current_line = _current_line.c_str();
			}

			//Handle input render targets.
			if (current_line.Find("InputRenderTarget("))
			{
				//Parse the arguments.
				StaticArray<DynamicString, 4> arguments;

				const uint64 number_of_arguments_parsed
				{
					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.Data(),
						current_line.Length(),
						arguments.Data()
					)
				};
				ASSERT(number_of_arguments_parsed == 4, "Wrong number of arguments for 'InputRenderTarget()' declaration!");

				//Construct the GLSL line and add it.
				char glsl_line[128];
				sprintf_s(glsl_line, "layout (set = 1, binding = %u) uniform sampler2D %s;", current_binding_index++, arguments[0].Data());

				current_line = glsl_line;
			}

			//Replace "INSTANCE_INDEX" with "gl_InstanceIndex".
			if (current_line.Find("INSTANCE_INDEX"))
			{
				std::string _current_line{ current_line.Data() };

				size_t position{ _current_line.find("INSTANCE_INDEX") };

				while (position != std::string::npos)
				{
					_current_line.replace(position, strlen("INSTANCE_INDEX"), "gl_InstanceIndex");
					position = _current_line.find("INSTANCE_INDEX");
				}

				current_line = _current_line.c_str();
			}

			//Handle fragment outputs.
			if (current_line.Find("OutputFragment("))
			{
				//Parse the arguments.
				StaticArray<DynamicString, 2> arguments;

				const uint64 number_of_arguments_parsed
				{
					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.Data(),
						current_line.Length(),
						arguments.Data()
					)
				};
				ASSERT(number_of_arguments_parsed == 2, "Wrong number of arguments for 'OutputFragment()' declaration!");

				//Construct the GLSL line and add it.
				char glsl_line[128];
				sprintf_s(glsl_line, "\t%s =  %s;", arguments[0].Data(), arguments[1].Data());

				current_line = glsl_line;
			}

			//Handle compute outoput render targets.
			if (current_line.Find("OutputRenderTarget("))
			{
				//This is only valid for fragment shaders.
				if (shader_stage._ShaderStage == ShaderStage::FRAGMENT)
				{
					//Parse the arguments.
					StaticArray<DynamicString, 1> arguments;

					const uint64 number_of_arguments_parsed
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.Data(),
							current_line.Length(),
							arguments.Data()
						)
					};
					ASSERT(number_of_arguments_parsed == 1, "Wrong number of arguments for 'OutputRenderTarget()' declaration!");

					//Construct the GLSL line and add it.
					char glsl_line[128];
					sprintf_s(glsl_line, "layout (location = %u) out vec4 %s;", location_index++, arguments[0].Data());

					current_line = glsl_line;
				}

				else
				{
					current_line = "";
				}
			}

			//Handle vertex position outputs.
			if (current_line.Find("OutputVertexPosition("))
			{
				//Parse the arguments.
				StaticArray<DynamicString, 1> arguments;

				const uint64 number_of_arguments_parsed
				{
					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.Data(),
						current_line.Length(),
						arguments.Data()
					)
				};
				ASSERT(number_of_arguments_parsed == 1, "Wrong number of arguments for 'OutputVertexPosition()' declaration!");

				//Construct the GLSL line and add it.
				char glsl_line[128];
				sprintf_s(glsl_line, "\tgl_Position =  %s;", arguments[0].Data());

				current_line = glsl_line;
			}

			//Replace "RAY_HIT_DISTANCE" with "gl_HitTNV".
			if (current_line.Find("RAY_HIT_DISTANCE"))
			{
				std::string _current_line{ current_line.Data() };

				size_t position{ _current_line.find("RAY_HIT_DISTANCE") };

				while (position != std::string::npos)
				{
					_current_line.replace(position, strlen("RAY_HIT_DISTANCE"), "gl_HitTNV");
					position = _current_line.find("RAY_HIT_DISTANCE");
				}

				current_line = _current_line.c_str();
			}

			//Replace "RAY_TRACING_ID" with "gl_LaunchIDNV".
			if (current_line.Find("RAY_TRACING_ID"))
			{
				std::string _current_line{ current_line.Data() };

				size_t position{ _current_line.find("RAY_TRACING_ID") };

				while (position != std::string::npos)
				{
					_current_line.replace(position, strlen("RAY_TRACING_ID"), "gl_LaunchIDNV");
					position = _current_line.find("RAY_TRACING_ID");
				}

				current_line = _current_line.c_str();
			}

			//Replace "RAY_TRACING_SIZE" with "gl_LaunchSizeNV".
			if (current_line.Find("RAY_TRACING_SIZE"))
			{
				std::string _current_line{ current_line.Data() };

				size_t position{ _current_line.find("RAY_TRACING_SIZE") };

				while (position != std::string::npos)
				{
					_current_line.replace(position, strlen("RAY_TRACING_SIZE"), "gl_LaunchSizeNV");
					position = _current_line.find("RAY_TRACING_SIZE");
				}

				current_line = _current_line.c_str();
			}

			//Replace "RAY_TRACING_FLAG_SKIP_CLOSEST_HIT" with "gl_RayFlagsSkipClosestHitShaderNV".
			if (current_line.Find("RAY_TRACING_FLAG_SKIP_CLOSEST_HIT"))
			{
				std::string _current_line{ current_line.Data() };

				size_t position{ _current_line.find("RAY_TRACING_FLAG_SKIP_CLOSEST_HIT") };

				while (position != std::string::npos)
				{
					_current_line.replace(position, strlen("RAY_TRACING_FLAG_SKIP_CLOSEST_HIT"), "gl_RayFlagsSkipClosestHitShaderNV");
					position = _current_line.find("RAY_TRACING_FLAG_SKIP_CLOSEST_HIT");
				}

				current_line = _current_line.c_str();
			}

			//Replace "RAY_TRACING_FLAG_TERMINATE_ON_FIRST_HIT" with "gl_RayFlagsTerminateOnFirstHitNV".
			if (current_line.Find("RAY_TRACING_FLAG_TERMINATE_ON_FIRST_HIT"))
			{
				std::string _current_line{ current_line.Data() };

				size_t position{ _current_line.find("RAY_TRACING_FLAG_TERMINATE_ON_FIRST_HIT") };

				while (position != std::string::npos)
				{
					_current_line.replace(position, strlen("RAY_TRACING_FLAG_TERMINATE_ON_FIRST_HIT"), "gl_RayFlagsTerminateOnFirstHitNV");
					position = _current_line.find("RAY_TRACING_FLAG_TERMINATE_ON_FIRST_HIT");
				}

				current_line = _current_line.c_str();
			}

			//Handle samplers.
			if (current_line.Find("Sampler("))
			{
				//Parse the arguments.
				StaticArray<DynamicString, 5> arguments;

				const uint64 number_of_arguments_parsed
				{
					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.Data(),
						current_line.Length(),
						arguments.Data()
					)
				};
				ASSERT(number_of_arguments_parsed == 5, "Wrong number of arguments for 'Sampler()' declaration!");

				//Construct the GLSL line and add it.
				char glsl_line[128];
				sprintf_s(glsl_line, "layout (set = 1, binding = %u) uniform sampler %s;", current_binding_index++, arguments[0].Data());

				current_line = glsl_line;
			}

			//Replace "VERTEX_INDEX" with "gl_VertexIndex".
			if (current_line.Find("VERTEX_INDEX"))
			{
				std::string _current_line{ current_line.Data() };

				size_t position{ _current_line.find("VERTEX_INDEX") };

				while (position != std::string::npos)
				{
					_current_line.replace(position, strlen("VERTEX_INDEX"), "gl_VertexIndex");
					position = _current_line.find("VERTEX_INDEX");
				}

				current_line = _current_line.c_str();
			}

			//Replace "WORLD_RAY_DIRECTION" with "gl_WorldRayDirectionNV".
			if (current_line.Find("WORLD_RAY_DIRECTION"))
			{
				std::string _current_line{ current_line.Data() };

				size_t position{ _current_line.find("WORLD_RAY_DIRECTION") };

				while (position != std::string::npos)
				{
					_current_line.replace(position, strlen("WORLD_RAY_DIRECTION"), "gl_WorldRayDirectionNV");
					position = _current_line.find("WORLD_RAY_DIRECTION");
				}

				current_line = _current_line.c_str();
			}

			//Replace "WORLD_RAY_ORIGIN" with "gl_WorldRayOriginNV".
			if (current_line.Find("WORLD_RAY_ORIGIN"))
			{
				std::string _current_line{ current_line.Data() };

				size_t position{ _current_line.find("WORLD_RAY_ORIGIN") };

				while (position != std::string::npos)
				{
					_current_line.replace(position, strlen("WORLD_RAY_ORIGIN"), "gl_WorldRayOriginNV");
					position = _current_line.find("WORLD_RAY_ORIGIN");
				}

				current_line = _current_line.c_str();
			}

			//Add the line.
			glsl_lines.Emplace(current_line);
		}

		//For debug purposes, write out the GLSL shader to file for inspection.
		char file_path[MAXIMUM_FILE_PATH_LENGTH];

		{
			//Create the directory where we host the GLSL shaders.
			std::filesystem::create_directories("RenderPipelineAssetCompiler GLSL Shaders");

			//Figure out the file name.
			std::string file_name{ compile_data._FilePath.Data() };
			file_name = file_name.substr(file_name.find_last_of('\\') + 1);
			file_name = file_name.substr(0, file_name.find_last_of('.'));

			//Figure out the file path.
			sprintf_s(file_path, "%s\\%s %s.glsl", "RenderPipelineAssetCompiler GLSL Shaders", file_name.c_str(), ShaderStageString(shader_stage._ShaderStage));

			//Open the file.
			std::ofstream file{ file_path };

			//Write the lines.
			for (uint64 current_line_index{ 0 }; current_line_index < glsl_lines.Size(); ++current_line_index)
			{
				file << glsl_lines[current_line_index].Data();

				if (current_line_index != glsl_lines.LastIndex())
				{
					file << std::endl;
				}
			}

			//Close the file.
			file.close();
		}

		//Compile the shader!
		DynamicArray<byte> compiled_shader_data;

		{
			GLSLCompilation::CompileParameters parameters;

			parameters._ShaderStage = shader_stage._ShaderStage;
			parameters._InputLines = &glsl_lines;
			parameters._InputFilePath = file_path;
			parameters._OutputData = &compiled_shader_data;

			GLSLCompilation::Compile(parameters);
		}
	}
}