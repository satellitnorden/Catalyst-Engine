//Header file.
#include <GenerateProjects/GenerateAudioPlugin.h>

//Core.
#include <Core/Utilities.h>

//STD.
#include <filesystem>
#include <fstream>

/*
*	Generates the files for the given audio plugin.
*/
void GenerateAudioPlugin::GenerateFiles(const ProjectSettings &project_settings, const ProjectSettings::AudioPlugin &audio_plugin)
{
	//Ensure the directories exist.
	std::filesystem::create_directories(CodeDirectory(audio_plugin));

	//Generate the "AudioPluginGlue.Generated.h" file.
	{
		char buffer[256];
		PRINT_BUFFER(buffer, "%s/AudioPluginGlue.Generated.h", CodeDirectory(audio_plugin).c_str());

		std::ofstream file{ buffer };

		file << "#pragma once" << std::endl;
		file << std::endl;

		file << "//Core." << std::endl;
		file << "#include <Core/Essential/CatalystEssential.h>" << std::endl;
		file << std::endl;

		file << "//Rendering." << std::endl;
		file << "#include <Rendering/Native/Resolution.h>" << std::endl;
		file << std::endl;

		file << "//Audio plugin." << std::endl;
		file << "#include <" << audio_plugin._ClassName.c_str() << ".h>" << std::endl;
		file << std::endl;

		file << "namespace AudioPluginGlue" << std::endl;
		file << "{" << std::endl;
		file << std::endl;

		file << "\t/*" << std::endl;
		file << "\t*\tReturns the default directory of the plugin." << std::endl;
		file << "\t*/" << std::endl;
		file << "\tFORCE_INLINE constexpr const char *const RESTRICT GetDefaultDirectory() NOEXCEPT" << std::endl;
		file << "\t{" << std::endl;
		file << "\t\treturn \"" << audio_plugin._DefaultDirectory.c_str() << "\";" << std::endl;
		file << "\t}" << std::endl;
		file << std::endl;

		file << "\t/*" << std::endl;
		file << "\t*\tReturns the identifier of the plugin." << std::endl;
		file << "\t*/" << std::endl;
		file << "\tFORCE_INLINE constexpr const char *const RESTRICT GetIdentifier() NOEXCEPT" << std::endl;
		file << "\t{" << std::endl;
		file << "\t\treturn \"com." << project_settings._DeveloperNameNoSpaces.c_str() << "." << audio_plugin._NameNoSpaces.c_str() << "\";" << std::endl;
		file << "\t}" << std::endl;
		file << std::endl;

		file << "\t/*" << std::endl;
		file << "\t*\tReturns the name of the plugin." << std::endl;
		file << "\t*/" << std::endl;
		file << "\tFORCE_INLINE constexpr const char *const RESTRICT GetName() NOEXCEPT" << std::endl;
		file << "\t{" << std::endl;
		file << "\t\treturn \"" << audio_plugin._Name.c_str() << "\";" << std::endl;
		file << "\t}" << std::endl;
		file << std::endl;

		file << "\t/*" << std::endl;
		file << "\t*\tReturns the vendor of the plugin." << std::endl;
		file << "\t*/" << std::endl;
		file << "\tFORCE_INLINE constexpr const char *const RESTRICT GetVendor() NOEXCEPT" << std::endl;
		file << "\t{" << std::endl;
		file << "\t\treturn \"" << project_settings._DeveloperNameNoSpaces.c_str() << "\";" << std::endl;
		file << "\t}" << std::endl;
		file << std::endl;

		file << "\t/*" << std::endl;
		file << "\t*\tReturns the main category of the plugin." << std::endl;
		file << "\t*/" << std::endl;
		file << "\tFORCE_INLINE constexpr Audio::Plugin::MainCategory GetMainCategory() NOEXCEPT" << std::endl;
		file << "\t{" << std::endl;
		file << "\t\treturn Audio::Plugin::MainCategory::" << audio_plugin._MainCategory.c_str() << ";" << std::endl;
		file << "\t}" << std::endl;
		file << std::endl;

		file << "\t/*" << std::endl;
		file << "\t*\tReturns the sub category of the plugin." << std::endl;
		file << "\t*/" << std::endl;
		file << "\tFORCE_INLINE constexpr Audio::Plugin::SubCategory GetSubCategory() NOEXCEPT" << std::endl;
		file << "\t{" << std::endl;
		file << "\t\treturn Audio::Plugin::SubCategory::" << audio_plugin._SubCategory.c_str() << ";" << std::endl;
		file << "\t}" << std::endl;
		file << std::endl;

		file << "\t/*" << std::endl;
		file << "\t*\tReturns the resolution of the plugin." << std::endl;
		file << "\t*/" << std::endl;
		file << "\tFORCE_INLINE constexpr Resolution GetResolution() NOEXCEPT" << std::endl;
		file << "\t{" << std::endl;
		file << "\t\treturn Resolution(" << audio_plugin._Resolution._Width << ", " << audio_plugin._Resolution._Height << ");" << std::endl;
		file << "\t}" << std::endl;
		file << std::endl;

		file << "\t/*" << std::endl;
		file << "\t*\tCreates an instance of the plugin." << std::endl;
		file << "\t*/" << std::endl;
		file << "\tFORCE_INLINE Audio::Plugin *const RESTRICT Create() NOEXCEPT" << std::endl;
		file << "\t{" << std::endl;
		file << "\t\treturn new " << audio_plugin._ClassName.c_str() << "();" << std::endl;
		file << "\t}" << std::endl;
		file << std::endl;

		file << "\t/*" << std::endl;
		file << "\t*\tDestroys an instance of the plugin." << std::endl;
		file << "\t*/" << std::endl;
		file << "\tFORCE_INLINE void Destroy(Audio::Plugin *const RESTRICT plugin) NOEXCEPT" << std::endl;
		file << "\t{" << std::endl;
		file << "\t\tdelete plugin;" << std::endl;
		file << "\t}" << std::endl;
		file << std::endl;

		file << "}";

		file.close();
	}

	//Generate the "StandaloneMain.Generated.cpp" file.
	{
		char buffer[256];
		PRINT_BUFFER(buffer, "%s/StandaloneMain.Generated.cpp", CodeDirectory(audio_plugin).c_str());

		std::ofstream file{ buffer };

		file << "#if defined(AUDIO_PLUGIN_STANDALONE)" << std::endl;

		file << "//Core." << std::endl;
		file << "#include <Core/Essential/CatalystEssential.h>" << std::endl;
		file << std::endl;

		file << "//Audio." << std::endl;
		file << "#include <Audio/Effects/Utilities/PluginHandler.h>" << std::endl;
		file << "#include <Audio/Plugins/Editor/PluginEditor.h>" << std::endl;
		file << std::endl;

		file << "//Platform." << std::endl;
		file << "#include <Platform/Windows/CatalystPlatformWindows.h>" << std::endl;
		file << std::endl;

		file << "//Systems." << std::endl;
		file << "#include <Systems/CatalystEngineSystem.h>" << std::endl;
		file << "#include <Systems/AudioSystem.h>" << std::endl;
		file << std::endl;

		file << "//" << audio_plugin._Name.c_str() << "." << std::endl;
		file << "#include <AudioPluginGlue.Generated.h>" << std::endl;
		file << "#include <" << audio_plugin._NameNoSpaces.c_str() << "StandaloneMain.h>" << std::endl;
		file << "#include <" << audio_plugin._NameNoSpaces.c_str() << "Plugin.h>" << std::endl;
		file << std::endl;

		file << "//The plugin." << std::endl;
		file << audio_plugin._NameNoSpaces.c_str() << "Plugin PLUGIN;" << std::endl;
		file << std::endl;

		file << "//The plugin editor." << std::endl;
		file << "Audio::PluginEditor PLUGIN_EDITOR;" << std::endl;
		file << std::endl;

		file << "/*" << std::endl;
		file << "*\tCreates the Catalyst project configuration." << std::endl;
		file << "*/" << std::endl;
		file << "void CreateCatalystProjectConfiguration(CatalystProjectConfiguration *const RESTRICT configuration) NOEXCEPT" << std::endl;
		file << "{" << std::endl;

		file << "\tconfiguration->_GeneralConfiguration._ProjectName = \"" << audio_plugin._Name.c_str() << "\";" << std::endl;
		file << std::endl;

		file << "\tconfiguration->_ConcurrencyConfiguration._NumberOfTaskExecutors = " << audio_plugin._NameNoSpaces.c_str() << "NumberOfThreads();" << std::endl;
		file << std::endl;

		file << "\tif (Audio::PluginEditor::USE_INTERNAL_RENDERING)" << std::endl;
		file << "\t{" << std::endl;
		file << "\t\tconfiguration->_RenderingConfiguration._InitialRenderingPath = RenderingPath::NONE;" << std::endl;
		file << "\t}" << std::endl;
		file << std::endl;

		file << "\telse" << std::endl;
		file << "\t{" << std::endl;
		file << "\t\tconfiguration->_RenderingConfiguration._InitialRenderingPath = RenderingPath::DEFAULT;" << std::endl;
		file << "\t}" << std::endl;
		file << std::endl;
		file << "\tconfiguration->_RenderingConfiguration._InitialFullScreen = false;" << std::endl;
		file << "\tconfiguration->_RenderingConfiguration._Resolution = AudioPluginGlue::GetResolution();" << std::endl;

		file << "}" << std::endl;
		file << std::endl;

		file << "MAIN_FUNCTION" << std::endl;
		file << "(" << std::endl;

		file << "\t//Call the pre-initialize standalone function." << std::endl;
		file << "\t" << audio_plugin._NameNoSpaces.c_str() << "PreInitialize();" << std::endl;
		file << std::endl;

		file << "\t//Create the Catalyst project configuration." << std::endl;
		file << "\tCatalystProjectConfiguration configuration;" << std::endl;
		file << "\tCreateCatalystProjectConfiguration(&configuration);" << std::endl;
		file << std::endl;

		file << "\t//Initialize the engine system." << std::endl;
		file << "\tCatalystEngineSystem::Instance->Initialize(configuration);" << std::endl;
		file << std::endl;

		file << "\t//Call the post-initialize standalone function." << std::endl;
		file << "\t" << audio_plugin._NameNoSpaces.c_str() << "PostInitialize();" << std::endl;
		file << std::endl;

		file << "\t//Initialize the plugin editor." << std::endl;
		file << "\tPLUGIN_EDITOR.Initialize(&PLUGIN);" << std::endl;
		file << "\tPLUGIN_EDITOR.CreateEditor(CatalystPlatformWindows::_MainWindow);" << std::endl;
		file << std::endl;

		file << "\t//Add the plugin handler." << std::endl;
		file << "\tPluginHandler *const RESTRICT plugin_handler{ AudioSystem::Instance->CreateAudioEffect<PluginHandler>(&PLUGIN) };" << std::endl;
		file << "\tAudioSystem::Instance->AddMasterAudioEffect(plugin_handler);" << std::endl;
		file << std::endl;

		file << "\t//Main game loop." << std::endl;
		file << "\twhile (CatalystEngineSystem::Instance->Update());" << std::endl;
		file << std::endl;

		file << "\t//Terminate the plugin editor." << std::endl;
		file << "\tPLUGIN_EDITOR.DestroyEditor();" << std::endl;
		file << std::endl;

		file << "\t//Terminate the engine system." << std::endl;
		file << "\tCatalystEngineSystem::Instance->Terminate();" << std::endl;

		file << ")" << std::endl;

		file << "#endif";

		file.close();
	}

	//Generate the audio plugin header file.
	{
		char buffer[256];
		PRINT_BUFFER(buffer, "%s/%s.h", CodeDirectory(audio_plugin).c_str(), audio_plugin._ClassName.c_str());

		if (!audio_plugin._Locked)
		{
			std::ofstream file{ buffer };

			file << "#pragma once" << std::endl;
			file << std::endl;

			file << "//Core." << std::endl;
			file << "#include <Core/Essential/CatalystEssential.h>" << std::endl;
			file << std::endl;

			file << "//Audio." << std::endl;
			file << "#include <Audio/Plugins/Core/Plugin.h>" << std::endl;
			file << std::endl;

			file << "//Forward declarations." << std::endl;
			file << "class " << audio_plugin._NameNoSpaces.c_str() << "UIScene;" << std::endl;
			file << std::endl;

			file << "class " << audio_plugin._ClassName.c_str() << " final : public Audio::Plugin" << std::endl;
			file << "{" << std::endl;

			file << std::endl;
			file << "public:" << std::endl;
			file << std::endl;

			file << "\t/*" << std::endl;
			file << "\t*\tDefault constructor." << std::endl;
			file << "\t*/" << std::endl;
			file << "\t" << audio_plugin._NameNoSpaces.c_str() << "Plugin() NOEXCEPT;" << std::endl;
			file << std::endl;

			file << "\t/*" << std::endl;
			file << "\t*\tReturns the main category." << std::endl;
			file << "\t*/" << std::endl;
			file << "\tFORCE_INLINE NO_DISCARD Audio::Plugin::MainCategory GetMainCategory() const NOEXCEPT override" << std::endl;
			file << "\t{" << std::endl;
			file << "\t\treturn Audio::Plugin::MainCategory::" << audio_plugin._MainCategory.c_str() << ";" << std::endl;
			file << "\t}" << std::endl;
			file << std::endl;

			file << "\t/*" << std::endl;
			file << "\t*\tReturns the sub category." << std::endl;
			file << "\t*/" << std::endl;
			file << "\tFORCE_INLINE NO_DISCARD Audio::Plugin::SubCategory GetSubCategory() const NOEXCEPT override" << std::endl;
			file << "\t{" << std::endl;
			file << "\t\treturn Audio::Plugin::SubCategory::" << audio_plugin._SubCategory.c_str() << ";" << std::endl;
			file << "\t}" << std::endl;
			file << std::endl;

			file << "\t/*" << std::endl;
			file << "\t*\tReturns the resolution." << std::endl;
			file << "\t*/" << std::endl;
			file << "\tFORCE_INLINE NO_DISCARD Resolution GetResolution() const NOEXCEPT override" << std::endl;
			file << "\t{" << std::endl;
			file << "\t\treturn Resolution(" << audio_plugin._Resolution._Width << ", " << audio_plugin._Resolution._Height << ");" << std::endl;
			file << "\t}" << std::endl;
			file << std::endl;

			file << "\t/*" << std::endl;
			file << "\t*\tInitializes this plugin." << std::endl;
			file << "\t*/" << std::endl;
			file << "\tvoid Initialize(const InitializeContext &context) NOEXCEPT override;" << std::endl;
			file << std::endl;

			file << "\t/*" << std::endl;
			file << "\t*\tPost-initializes this plugin." << std::endl;
			file << "\t*/" << std::endl;
			file << "\tvoid PostInitialize(const InitializeContext &context) NOEXCEPT override;" << std::endl;
			file << std::endl;

			file << "\t/*" << std::endl;
			file << "\t*\tCallback for this plugin to process the given buffer." << std::endl;
			file << "\t*/" << std::endl;
			file << "\tvoid Process" << std::endl;
			file << "\t(" << std::endl;
			file << "\t\tconst AudioProcessContext &context," << std::endl;
			file << "\t\tconst DynamicArray<DynamicArray<float32>> &inputs," << std::endl;
			file << "\t\tDynamicArray<DynamicArray<float32>> *const RESTRICT outputs," << std::endl;
			file << "\t\tconst uint8 number_of_channels," << std::endl;
			file << "\t\tconst uint32 number_of_samples" << std::endl;
			file << "\t) NOEXCEPT override;" << std::endl;
			file << std::endl;

			if (DoesMIDIIn(audio_plugin))
			{
				file << "\t/*" << std::endl;
				file << "\t*\tCallback for this plugin when it receives MIDI messages." << std::endl;
				file << "\t*/" << std::endl;
				file << "\tvoid OnMIDIMessages(const MIDIMessage *const RESTRICT messages, const uint64 number_of_messages) NOEXCEPT override;" << std::endl;
				file << std::endl;
			}

			file << "private:" << std::endl;
			file << std::endl;

			file << "\t//The UI scene." << std::endl;
			file << "\t" << audio_plugin._NameNoSpaces.c_str() << "UIScene *RESTRICT _UIScene;" << std::endl;
			file << std::endl;

			file << "};";

			file.close();
		}
	}

	//Generate the audio plugin CPP file.
	{
		char buffer[256];
		PRINT_BUFFER(buffer, "%s/%s.cpp", CodeDirectory(audio_plugin).c_str(), audio_plugin._ClassName.c_str());

		if (!audio_plugin._Locked)
		{
			std::ofstream file{ buffer };

			file << "//Header file." << std::endl;
			file << "#include <" << audio_plugin._ClassName.c_str() << ".h>" << std::endl;
			file << std::endl;

			file << "//Systems." << std::endl;
			file << "#include <Systems/UISystem.h>" << std::endl;
			file << std::endl;

			file << "//" << audio_plugin._Name.c_str() << "." << std::endl;
			file << "#include <" << audio_plugin._NameNoSpaces.c_str() << "UIScene.h>" << std::endl;
			file << std::endl;

			file << "/*" << std::endl;
			file << "*\tDefault constructor." << std::endl;
			file << "*/" << std::endl;
			file << "" << audio_plugin._NameNoSpaces.c_str() << "Plugin::" << audio_plugin._NameNoSpaces.c_str() << "Plugin() NOEXCEPT" << std::endl;
			file << "{" << std::endl;
			file << std::endl;
			file << "}" << std::endl;
			file << std::endl;

			file << "/*" << std::endl;
			file << "*\tInitializes this plugin." << std::endl;
			file << "*/" << std::endl;
			file << "void " << audio_plugin._ClassName.c_str() << "::Initialize(const InitializeContext &context) NOEXCEPT" << std::endl;
			file << "{" << std::endl;
			file << std::endl;
			file << "}" << std::endl;
			file << std::endl;

			file << "/*" << std::endl;
			file << "*\tPost-initializes this plugin." << std::endl;
			file << "*/" << std::endl;
			file << "void " << audio_plugin._ClassName.c_str() << "::PostInitialize(const InitializeContext &context) NOEXCEPT" << std::endl;
			file << "{" << std::endl;
			file << "\t//Add the UI scene." << std::endl;
			file << "\t_UIScene = context._UISystem->AddScene<" << audio_plugin._NameNoSpaces.c_str() << "UIScene>();" << std::endl;
			file << "\t_UIScene->SetPlugin(this);" << std::endl;
			file << "}" << std::endl;
			file << std::endl;

			file << "/*" << std::endl;
			file << "*\tCallback for this plugin to process the given buffer." << std::endl;
			file << "*/" << std::endl;
			file << "void " << audio_plugin._ClassName.c_str() << "::Process" << std::endl;
			file << "(" << std::endl;
			file << "\tconst AudioProcessContext &context," << std::endl;
			file << "\tconst DynamicArray<DynamicArray<float32>> &inputs," << std::endl;
			file << "\tDynamicArray<DynamicArray<float32>> *const RESTRICT outputs," << std::endl;
			file << "\tconst uint8 number_of_channels," << std::endl;
			file << "\tconst uint32 number_of_samples" << std::endl;
			file << ") NOEXCEPT" << std::endl;
			file << "{" << std::endl;
			file << std::endl;
			file << "}" << std::endl;
			file << std::endl;

			if (DoesMIDIIn(audio_plugin))
			{
				file << "/*" << std::endl;
				file << "*\tCallback for this plugin when it receives MIDI messages." << std::endl;
				file << "*/" << std::endl;
				file << "void " << audio_plugin._ClassName.c_str() << "::OnMIDIMessages(const MIDIMessage *const RESTRICT messages, const uint64 number_of_messages) NOEXCEPT" << std::endl;
				file << "{" << std::endl;
				file << std::endl;
				file << "}";
			}

			file.close();
		}
	}

	//Generate the audio plugin standalone main header file.
	{
		char buffer[256];
		PRINT_BUFFER(buffer, "%s/%sStandaloneMain.h", CodeDirectory(audio_plugin).c_str(), audio_plugin._NameNoSpaces.c_str());

		if (!audio_plugin._Locked)
		{
			std::ofstream file{ buffer };

			file << "#pragma once" << std::endl;
			file << std::endl;

			file << "//Core." << std::endl;
			file << "#include <Core/Essential/CatalystEssential.h>" << std::endl;
			file << std::endl;

			file << "/*" << std::endl;
			file << "*\tReturns the number of threads for the standalone application." << std::endl;
			file << "*/" << std::endl;
			file << "NO_DISCARD uint32 " << audio_plugin._NameNoSpaces.c_str() << "NumberOfThreads() NOEXCEPT;" << std::endl;
			file << std::endl;

			file << "/*" << std::endl;
			file << "*\tPre-initializes the " << audio_plugin._Name.c_str() << " plugin." << std::endl;
			file << "*/" << std::endl;
			file << "void " << audio_plugin._NameNoSpaces.c_str() << "PreInitialize() NOEXCEPT;" << std::endl;
			file << std::endl;

			file << "/*" << std::endl;
			file << "*\tPost-initializes the " << audio_plugin._Name.c_str() << " plugin." << std::endl;
			file << "*/" << std::endl;
			file << "void " << audio_plugin._NameNoSpaces.c_str() << "PostInitialize() NOEXCEPT;";

			file.close();
		}
	}

	//Generate the audio plugin standalone main source file.
	{
		char buffer[256];
		PRINT_BUFFER(buffer, "%s/%sStandaloneMain.cpp", CodeDirectory(audio_plugin).c_str(), audio_plugin._NameNoSpaces.c_str());

		if (!audio_plugin._Locked)
		{
			std::ofstream file{ buffer };

			file << "//Header file." << std::endl;
			file << "#include <" << audio_plugin._NameNoSpaces << "StandaloneMain.h>" << std::endl;
			file << std::endl;

			file << "#if !defined(CATALYST_CONFIGURATION_FINAL)" << std::endl;
			file << "//Systems." << std::endl;
			file << "#include <Systems/ContentSystem.h>" << std::endl;
			file << "#endif" << std::endl;
			file << std::endl;

			file << "/*" << std::endl;
			file << "*\tReturns the number of threads for the standalone application." << std::endl;
			file << "*/" << std::endl;
			file << "NO_DISCARD uint32 " << audio_plugin._NameNoSpaces.c_str() << "NumberOfThreads() NOEXCEPT" << std::endl;
			file << "{" << std::endl;
			file << "\t return 0;" << std::endl;
			file << "}" << std::endl;
			file << std::endl;

			file << "/*" << std::endl;
			file << "*\tPre-initializes the " << audio_plugin._Name.c_str() << " plugin." << std::endl;
			file << "*/" << std::endl;
			file << "void " << audio_plugin._NameNoSpaces.c_str() << "PreInitialize() NOEXCEPT" << std::endl;
			file << "{" << std::endl;
			file << std::endl;
			file << "}" << std::endl;
			file << std::endl;

			file << "/*" << std::endl;
			file << "*\tPost-initializes the " << audio_plugin._Name.c_str() << " plugin." << std::endl;
			file << "*/" << std::endl;
			file << "void " << audio_plugin._NameNoSpaces.c_str() << "PostInitialize() NOEXCEPT" << std::endl;
			file << "{" << std::endl;
			file << "#if !defined(CATALYST_CONFIGURATION_FINAL)" << std::endl;
			file << "\t//Compile content." << std::endl;
			file << "\tContentSystem::Instance->Compile(\"..//..//..//AudioPlugins//" << audio_plugin._NameNoSpaces.c_str() << "//Content\");" << std::endl;
			file << "#endif" << std::endl;
			file << "}";

			file.close();
		}
	}

	//Generate the audio plugin UI scene header file.
	{
		char buffer[256];
		PRINT_BUFFER(buffer, "%s/%sUIScene.h", CodeDirectory(audio_plugin).c_str(), audio_plugin._NameNoSpaces.c_str());

		if (!audio_plugin._Locked)
		{
			std::ofstream file{ buffer };

			file << "#pragma once" << std::endl;
			file << std::endl;

			file << "//Core." << std::endl;
			file << "#include <Core/Essential/CatalystEssential.h>" << std::endl;
			file << std::endl;

			file << "//UI." << std::endl;
			file << "#include <UI/Core/Scene.h>" << std::endl;
			file << std::endl;

			file << "//Forward declarations." << std::endl;
			file << "class " << audio_plugin._ClassName.c_str() << ";" << std::endl;
			file << std::endl;

			file << "class " << audio_plugin._NameNoSpaces.c_str() << "UIScene final : public UI::Scene" << std::endl;
			file << "{" << std::endl;

			file << std::endl;
			file << "public:" << std::endl;
			file << std::endl;

			file << "\t/*" << std::endl;
			file << "\t*\tSets the plugin." << std::endl;
			file << "\t*/" << std::endl;
			file << "\tvoid SetPlugin(" << audio_plugin._ClassName.c_str() << " *const RESTRICT value) NOEXCEPT;" << std::endl;
			file << std::endl;

			file << "\t/*" << std::endl;
			file << "\t*\tBuilds this scene." << std::endl;
			file << "\t*/" << std::endl;
			file << "\tvoid Build(const UI::BuildContext &context) NOEXCEPT override;" << std::endl;
			file << std::endl;

			file << "private:" << std::endl;
			file << std::endl;

			file << "\t//The plugin." << std::endl;
			file << "\t" << audio_plugin._ClassName.c_str() << " *RESTRICT _Plugin;" << std::endl;
			file << std::endl;

			file << "};";

			file.close();
		}
	}

	//Generate the audio plugin UI scene source file.
	{
		char buffer[256];
		PRINT_BUFFER(buffer, "%s/%sUIScene.cpp", CodeDirectory(audio_plugin).c_str(), audio_plugin._NameNoSpaces.c_str());

		if (!audio_plugin._Locked)
		{
			std::ofstream file{ buffer };

			file << "//Header file." << std::endl;
			file << "#include <" << audio_plugin._NameNoSpaces << "UIScene.h>" << std::endl;
			file << std::endl;

			if (DoesMIDIIn(audio_plugin))
			{
				file << "//UI." << std::endl;
				file << "#include <UI/Widgets/PianoRollWidget.h>" << std::endl;
				file << std::endl;
			}

			file << "//" << audio_plugin._Name.c_str() << "." << std::endl;
			file << "#include <" << audio_plugin._ClassName.c_str() << ".h>" << std::endl;
			file << std::endl;

			file << "/*" << std::endl;
			file << "*\tSets the plugin." << std::endl;
			file << "*/" << std::endl;
			file << "void " << audio_plugin._NameNoSpaces.c_str() << "UIScene::SetPlugin(" << audio_plugin._ClassName.c_str() << " *const RESTRICT value) NOEXCEPT" << std::endl;
			file << "{" << std::endl;
			file << "\t//Set the plugin." << std::endl;
			file << "\t_Plugin = value;" << std::endl;
			file << "}" << std::endl;
			file << std::endl;

			file << "/*" << std::endl;
			file << "*\tBuilds this scene.." << std::endl;
			file << "*/" << std::endl;
			file << "void " << audio_plugin._NameNoSpaces.c_str() << "UIScene::Build(const UI::BuildContext &context) NOEXCEPT" << std::endl;
			file << "{" << std::endl;
			file << "\t//Start the container." << std::endl;
			file << "\t{" << std::endl;
			file << "\t\tconstexpr UI::Identifier IDENTIFIER{ \"Container\" };" << std::endl;
			file << "\t\tconstexpr UI::Anchor ANCHOR{ UI::Anchor::CENTER };" << std::endl;
			file << "\t\tconstexpr UI::Container::Layout LAYOUT{ UI::Container::Layout::TOP_TO_BOTTOM };" << std::endl;
			file << "\t\tconst AxisAlignedBoundingBox2D AXIS_ALIGNED_BOUNDING_BOX" << std::endl;
			file << "\t\t{" << std::endl;
			file << "\t\t\tVector2<float32>(0.0f, 0.0f)," << std::endl;
			file << "\t\t\tVector2<float32>(GetReferenceResolution()._X, GetReferenceResolution()._Y)" << std::endl;
			file << "\t\t};" << std::endl;
			file << std::endl;
			file << "\t\tStartContainer(context, UI::Identifier(IDENTIFIER), ANCHOR, LAYOUT, AXIS_ALIGNED_BOUNDING_BOX);" << std::endl;
			file << "\t}" << std::endl;
			file << std::endl;

			file << "\t//Set up the bounding box." << std::endl;
			file << "\tAxisAlignedBoundingBox2D bounding_box" << std::endl;
			file << "\t{" << std::endl;
			file << "\t\tVector2<float32>(0.0f, 0.0f)," << std::endl;
			file << "\t\tVector2<float32>(GetReferenceResolution()._X, GetReferenceResolution()._Y)" << std::endl;
			file << "\t};" << std::endl;
			file << std::endl;

			if (DoesMIDIIn(audio_plugin))
			{
				file << "\t//Add the piano roll." << std::endl;
				file << "\t{" << std::endl;
				file << "\t\t//Set the bounding box." << std::endl;
				file << "\t\tconst AxisAlignedBoundingBox2D piano_roll_bounding_box" << std::endl;
				file << "\t\t{" << std::endl;
				file << "\t\t\tVector2<float32>(0.0f, 0.0f)," << std::endl;
				file << "\t\t\tVector2<float32>(bounding_box.Dimensions()._X, BaseMath::Round<float32>(bounding_box.Dimensions()._Y * 0.1f))" << std::endl;
				file << "\t\t};" << std::endl;
				file << std::endl;
				file << "\t\tSetContainerNextWidgetBoundingBox(piano_roll_bounding_box);" << std::endl;
				file << std::endl;
				file << "\t\t//Add the widget." << std::endl;
				file << "\t\tAddWidget<UI::PianoRollWidget>(context, UI::Identifier(\"Piano Roll\"))" << std::endl;
				file << "\t\t->SetAudioPlugin(_Plugin);" << std::endl;
				file << std::endl;
				file << "\t\t//Shrink the bounding box to account for the piano roll." << std::endl;
				file << "\t\tbounding_box._Minimum._Y += piano_roll_bounding_box.Dimensions()._Y;" << std::endl;
				file << "\t}" << std::endl;
				file << std::endl;
			}

			file << "\t//End the container." << std::endl;
			file << "\tEndContainer(context);" << std::endl;
			file << "}" << std::endl;
			file << std::endl;

			file.close();
		}
	}
}