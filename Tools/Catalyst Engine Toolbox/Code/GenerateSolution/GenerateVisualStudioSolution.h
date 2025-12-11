#pragma once

//Core.
#include <Core/Core.h>

//STD.
#include <string>
#include <vector>

class GenerateVisualStudioSolution
{

public:

	/*
	*	Parameters class definition.
	*/
	class Parameters final
	{

	public:

		/*
		*	Configuration class definition.
		*/
		class Configuration final
		{

		public:

			//Enumeration covering all types.
			enum class Type : uint8
			{
				//.exe builds
				APPLICATION,

				//.lib builds
				STATIC_LIBRARY,

				//.dll builds
				DYNAMIC_LIBRARY
			};

			//The name.
			const char *_Name;

			//The type.
			Type _Type;

			//The target file extension.
			const char *_TargetFileExtension;

			//Denotes whether or not to use debug libraries.
			bool _UseDebugLibraries;

			//The runtime library.
			const char *_RuntimeLibrary;

			//The subsystem.
			const char *_SubSystem;

			//The preprocessor definitions.
			std::vector<std::string> _PreprocessorDefinitions;

			//The library includes.
			std::vector<std::string> _LibraryIncludes;

			//The pre build event.
			std::string _PreBuildEvent;

		};

		/*
		*	Platform class definition.
		*/
		class Platform final
		{

		public:

			//The name.
			const char *_Name;

		};

		/*
		*	File class definition.
		*/
		class File final
		{

		public:

			//The path.
			std::string _Path;

			//Denotes whether or not to enable warnings for this file.
			bool _EnableWarnings;

		};

		/*
		*	Directory class definition.
		*/
		class Directory final
		{

		public:

			//The name.
			std::string _Name;

			//The directories.
			std::vector<Directory> _Directories;

			//The files.
			std::vector<File> _Files;

		};

		//The directory path.
		const char *_DirectoryPath;

		//The project name.
		const char *_ProjectName;

		//The configurations.
		std::vector<Configuration> _Configurations;

		//The platforms.
		std::vector<Platform> _Platforms;

		//The code include paths.
		std::vector<std::string> _CodeIncludePaths;

		//The library include paths.
		std::vector<std::string> _LibraryIncludePaths;

		//The directories.
		std::vector<Directory> _Directories;

	};

	/*
	*	Generates!
	*/
	static void Generate(const Parameters &parameters);

private:

	/*
	*	Generates the project file.
	*/
	static void GenerateProjectFile(const Parameters &parameters, const std::string &project_guid);

	/*
	*	Generates the filters file.
	*/
	static void GenerateFiltersFile(const Parameters &parameters);

	/*
	*	Generates the solution file.
	*/
	static void GenerateSolutionFile(const Parameters &parameters, const std::string &solution_guid, const std::string &project_guid);

	/*
	*   Adds compile files for the given directory.
	*/
	static void AddCompileFiles(std::ofstream &file, const Parameters::Directory &directory);

	/*
	*	Adds filter files.
	*/
	static void AddFilterFiles(std::ofstream &file, const Parameters::Directory &directory, const std::string &filter_string);

	/*
	*	Adds filter identifiers.
	*/
	static void AddFilterIdentifiers(std::ofstream &file, const Parameters::Directory &directory, const std::string &filter_string);

};