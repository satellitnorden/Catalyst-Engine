#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//File.
#include <File/Types/AnimatedModelFile.h>
#include <File/Types/ModelFile.h>

class FBXReader final
{

public:

	/*
	*	Reads the animated model file at the given file path. Returns if the read was succesful.
	*/
	static NO_DISCARD bool Read(const char *const RESTRICT file_path, AnimatedModelFile *const RESTRICT animated_model_file) NOEXCEPT;

	/*
	*	Reads the model file at the given file path. Returns if the read was succesful.
	*/
	static NO_DISCARD bool Read(const char *const RESTRICT file_path, ModelFile *const RESTRICT model_file) NOEXCEPT;

	
};