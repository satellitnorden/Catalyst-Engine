#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>

//Content.
#include <Content/Core/AssetCompiler.h>

class ALIGN(8) ScriptAssetCompiler final : public AssetCompiler
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ScriptAssetCompiler);

	/*
	*	Default constructor.
	*/
	ScriptAssetCompiler() NOEXCEPT;

	/*
	*	Runs before compilation in the specified domain has finished.
	*/
	void PreCompile(const CompilationDomain compilation_domain) NOEXCEPT override;

	/*
	*	Compiles a single asset with the given compile context.
	*/
	void Compile(const CompileContext &compile_context) NOEXCEPT override;

	/*
	*	Runs after compilation in the specified domain has finished.
	*/
	void PostCompile(const CompilationDomain compilation_domain) NOEXCEPT override;

	/*
	*	Adds an extra include.
	*/
	void AddExtraInclude(const char *const RESTRICT extra_include) NOEXCEPT;

private:

	/*
	*	Script data class definition.
	*/
	class ScriptData final
	{

	public:

		//The file path.
		DynamicString _FilePath;

		//The name.
		DynamicString _Name;

		//The flags.
		uint32 _Flags;

		//The required data size.
		uint64 _RequiredDataSize;

	};

	//Denotes if scripts need to be compile.
	bool _CompileScripts;

	//The script data.
	DynamicArray<ScriptData> _ScriptData;

	//The extra includes.
	DynamicArray<DynamicString> _ExtraIncludes;

	/*
	*	Generates the source file.
	*/
	void GenerateSourceFile(const CompilationDomain compilation_domain) NOEXCEPT;

	/*
	*	Generates the header file.
	*/
	void GenerateHeaderFile(const CompilationDomain compilation_domain) NOEXCEPT;

};