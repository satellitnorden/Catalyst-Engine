#pragma once

//Core.
#include <Core/Core.h>

//Project settings.
#include <ProjectSettings/ProjectSettings.h>

class GenerateAudioPlugin final
{

public:

	/*
	*	Returns the code directory for the given audio plugin.
	*/
	static std::string CodeDirectory(const ProjectSettings::AudioPlugin &audio_plugin)
	{
		return "../AudioPlugins/" + audio_plugin._NameNoSpaces + "/Code";
	}

	/*
	*	Returns the content directory for the given audio plugin.
	*/
	static std::string ContentDirectory(const ProjectSettings::AudioPlugin &audio_plugin)
	{
		return "../AudioPlugins/" + audio_plugin._NameNoSpaces + "/Content";
	}

	/*
	*	Returns the CMake directory for the given audio plugin.
	*/
	static std::string CMakeDirectory(const ProjectSettings::AudioPlugin &audio_plugin)
	{
		return audio_plugin._NameNoSpaces + "/CMake";
	}

	/*
	*	Returns the visual studio solution directory for the given audio plugin.
	*/
	static std::string VisualStudioSolutionDirectory(const ProjectSettings::AudioPlugin &audio_plugin)
	{
		return audio_plugin._NameNoSpaces + "/Visual Studio";
	}

	/*
	*	Returns the visual studio code workspace directory for the given audio plugin.
	*/
	static std::string VisualStudioCodeWorkspaceDirectory(const ProjectSettings::AudioPlugin &audio_plugin)
	{
		return audio_plugin._NameNoSpaces + "/Visual Studio Code";
	}

	/*
	*	Returns if the given audio plugin does MIDI in.
	*/
	static bool DoesMIDIIn(const ProjectSettings::AudioPlugin &audio_plugin)
	{
		return audio_plugin._MainCategory == "INSTRUMENT";
	}

	/*
	*	Generates the files for the given audio plugin.
	*/
	static void GenerateFiles(const ProjectSettings &project_settings, const ProjectSettings::AudioPlugin &audio_plugin);

};