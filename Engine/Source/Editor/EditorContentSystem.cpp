#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorContentSystem.h>

//File.
#include <File/Core/File.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/ImGuiSystem.h>

//STL.
#include <fstream>

/*
*	Initializes the editor content system.
*/
void EditorContentSystem::Initialize() NOEXCEPT
{
	//Register the top bar callback.
	CatalystEditorSystem::Instance->RegisterTopBarMenuCallback
	(
		[]()
		{
			CatalystEditorSystem::Instance->GetEditorContentSystem()->TopBarCallback();
		}
	);
}

/*
*	The top bar callback.
*/
void EditorContentSystem::TopBarCallback() NOEXCEPT
{
	if (ImGui::BeginMenu("Content"))
	{
		if (ImGui::MenuItem("Create Model"))
		{
			SetUpCreateModel();
		}

		ImGui::EndMenu();
	}
}

/*
*	Sets up create model.
*/
void EditorContentSystem::SetUpCreateModel() NOEXCEPT
{
	//Register for the floating editor window.
	ImGuiSystem::Instance->RegisterEditorWindow
	(
		ImGuiSystem::EditorWindow::FLOATING,
		[](const Vector2<float32> minimum, const Vector2<float32> maximum)
		{
			return CatalystEditorSystem::Instance->GetEditorContentSystem()->WindowCallback(minimum, maximum);
		}
	);
}

/*
*	The window callback.
*/
NO_DISCARD bool EditorContentSystem::WindowCallback(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT
{
	//Begin the window.
	ImGuiSystem::BeginWindowParameters begin_window_parameters;

	begin_window_parameters._Name = "Create Model";
	begin_window_parameters._Minimum = minimum;
	begin_window_parameters._Maximum = maximum;
	begin_window_parameters._EnableResize = true;
	begin_window_parameters._EnableMove = true;
	begin_window_parameters._EnableMenuBar = false;
	begin_window_parameters._Closable = true;

	if (!ImGuiSystem::Instance->BeginWindow(begin_window_parameters))
	{
		//End the window.
		ImGui::End();

		return false;
	}

	//Add the widget for the input folder.
	char input_folder_buffer[MAXIMUM_FILE_PATH_LENGTH];

	ImGui::Text("Input Folder:");
	ImGui::SameLine();

	if (_CreateModelState._InputFolder)
	{
		sprintf_s(input_folder_buffer, "%s", _CreateModelState._InputFolder.Data());
	}

	else
	{
		Memory::Set(input_folder_buffer, 0, MAXIMUM_FILE_PATH_LENGTH);
	}

	ImGui::InputTextWithHint("##INPUT_FOLDER", "Choose Input Folder", input_folder_buffer, MAXIMUM_FILE_PATH_LENGTH);
	ImGui::SameLine();

	ImGui::PushID("##INPUT_FOLDER_SELECTOR");
	if (ImGui::Button("..."))
	{
		if (File::BrowseForFolder(&_CreateModelState._InputFolder))
		{
			CreateModelGatherFiles(_CreateModelState._InputFolder.Data());
			CreateModelEstimateFiles();
		}
	}
	ImGui::PopID();

	//Add selectors.
	if (_CreateModelState._InputFolder)
	{
		//Add a selectors for the level of detail models.
		if (ImGui::Button("Add Level Of Detail"))
		{
			_CreateModelState._LevelOfDetailModelFiles.Emplace();
		}

		for (uint64 i{ 0 }; i < _CreateModelState._LevelOfDetailModelFiles.Size();)
		{
			char text_buffer[32];
			sprintf_s(text_buffer, "Level Of Detail %llu:", i + 1);

			ImGui::Text(text_buffer);
			ImGui::SameLine();

			char id_buffer[32];
			sprintf_s(id_buffer, "##LEVEL_OF_DETAIL_%llu", i + 1);

			ImGui::PushID(id_buffer);

			if (ImGui::BeginCombo("##LEVEL_OF_DETAIL_SELECTOR", _CreateModelState._LevelOfDetailModelFiles[i] ? _CreateModelState._LevelOfDetailModelFiles[i].Data() : "None"))
			{
				for (const DynamicString &model_file : _CreateModelState._ModelFiles)
				{
					if (ImGui::Selectable(model_file.Data(), _CreateModelState._LevelOfDetailModelFiles[i] == model_file))
					{
						_CreateModelState._LevelOfDetailModelFiles[i] = model_file;
					}
				}

				ImGui::EndCombo();
			}

			ImGui::SameLine();

			if (ImGui::Button("Delete"))
			{
				_CreateModelState._LevelOfDetailModelFiles.EraseAt<true>(i);
			}

			else
			{
				++i;
			}

			ImGui::PopID();
		}

		//Add a selector for the base model.
		ImGui::Text("Collision Model:");
		ImGui::SameLine();

		if (ImGui::BeginCombo("##COLLISION_MODEL_SELECTOR", _CreateModelState._CollisionModelFile ? _CreateModelState._CollisionModelFile.Data() : "None"))
		{
			for (const DynamicString &model_file : _CreateModelState._ModelFiles)
			{
				if (ImGui::Selectable(model_file.Data(), _CreateModelState._CollisionModelFile && _CreateModelState._CollisionModelFile == model_file))
				{
					_CreateModelState._CollisionModelFile = model_file;
				}
			}

			ImGui::EndCombo();
		}

		//Add a selector for the albedo texture.
		ImGui::Text("Albedo Texture:");
		ImGui::SameLine();

		if (ImGui::BeginCombo("##ALBEDO_TEXTURE_SELECTOR", _CreateModelState._AlbedoTextureFile ? _CreateModelState._AlbedoTextureFile.Data() : "None"))
		{
			for (const DynamicString &texture_file : _CreateModelState._TextureFiles)
			{
				if (ImGui::Selectable(texture_file.Data(), _CreateModelState._AlbedoTextureFile && _CreateModelState._AlbedoTextureFile == texture_file))
				{
					_CreateModelState._AlbedoTextureFile = texture_file;
				}
			}

			ImGui::EndCombo();
		}

		//Add a selector for the normal map texture.
		ImGui::Text("Normal Map Texture:");
		ImGui::SameLine();

		if (ImGui::BeginCombo("##NORMAL_MAP_TEXTURE_SELECTOR", _CreateModelState._NormalMapTextureFile ? _CreateModelState._NormalMapTextureFile.Data() : "None"))
		{
			for (const DynamicString &texture_file : _CreateModelState._TextureFiles)
			{
				if (ImGui::Selectable(texture_file.Data(), _CreateModelState._NormalMapTextureFile == texture_file))
				{
					_CreateModelState._NormalMapTextureFile = texture_file;
				}
			}

			ImGui::EndCombo();
		}

		//Add a selector for the displacement texture.
		ImGui::Text("Displacement Texture:");
		ImGui::SameLine();

		if (ImGui::BeginCombo("##DISPLACEMENT_TEXTURE_SELECTOR", _CreateModelState._DisplacementTextureFile ? _CreateModelState._DisplacementTextureFile.Data() : "None"))
		{
			for (const DynamicString &texture_file : _CreateModelState._TextureFiles)
			{
				if (ImGui::Selectable(texture_file.Data(), _CreateModelState._DisplacementTextureFile && _CreateModelState._DisplacementTextureFile == texture_file))
				{
					_CreateModelState._DisplacementTextureFile = texture_file;
				}
			}

			ImGui::EndCombo();
		}

		//Add a selector for the roughness texture.
		ImGui::Text("Roughness Texture:");
		ImGui::SameLine();

		if (ImGui::BeginCombo("##ROUGHNESS_TEXTURE_SELECTOR", _CreateModelState._RoughnessTextureFile ? _CreateModelState._RoughnessTextureFile.Data() : "None"))
		{
			for (const DynamicString &texture_file : _CreateModelState._TextureFiles)
			{
				if (ImGui::Selectable(texture_file.Data(), _CreateModelState._RoughnessTextureFile == texture_file))
				{
					_CreateModelState._RoughnessTextureFile = texture_file;
				}
			}

			ImGui::EndCombo();
		}

		//Add a selector for the metallic texture.
		ImGui::Text("Metallic Texture:");
		ImGui::SameLine();

		if (ImGui::BeginCombo("##METALLIC_TEXTURE_SELECTOR", _CreateModelState._MetallicTextureFile ? _CreateModelState._MetallicTextureFile.Data() : "None"))
		{
			for (const DynamicString &texture_file : _CreateModelState._TextureFiles)
			{
				if (ImGui::Selectable(texture_file.Data(), _CreateModelState._MetallicTextureFile && _CreateModelState._MetallicTextureFile == texture_file))
				{
					_CreateModelState._MetallicTextureFile = texture_file;
				}
			}

			ImGui::EndCombo();
		}

		//Add a selector for the ambient occlusion texture.
		ImGui::Text("Ambient Occlusion Texture:");
		ImGui::SameLine();

		if (ImGui::BeginCombo("##AMBIENT_OCCLUSION_TEXTURE_SELECTOR", _CreateModelState._AmbientOcclusionTextureFile ? _CreateModelState._AmbientOcclusionTextureFile.Data() : "None"))
		{
			for (const DynamicString &texture_file : _CreateModelState._TextureFiles)
			{
				if (ImGui::Selectable(texture_file.Data(), _CreateModelState._AmbientOcclusionTextureFile && _CreateModelState._AmbientOcclusionTextureFile == texture_file))
				{
					_CreateModelState._AmbientOcclusionTextureFile = texture_file;
				}
			}

			ImGui::EndCombo();
		}
	}

	//Add the widget for the output folder.
	char output_folder_buffer[MAXIMUM_FILE_PATH_LENGTH];

	ImGui::Text("Output Folder:");
	ImGui::SameLine();

	if (_CreateModelState._OutputFolder)
	{
		sprintf_s(output_folder_buffer, "%s", _CreateModelState._OutputFolder.Data());
	}

	else
	{
		Memory::Set(output_folder_buffer, 0, MAXIMUM_FILE_PATH_LENGTH);
	}

	ImGui::InputTextWithHint("##OUTPUT_FOLDER", "Choose Output Folder", output_folder_buffer, MAXIMUM_FILE_PATH_LENGTH);
	ImGui::SameLine();

	ImGui::PushID("##OUTPUT_FOLDER_SELECTOR");
	if (ImGui::Button("..."))
	{
		File::BrowseForFolder(&_CreateModelState._OutputFolder);
	}
	ImGui::PopID();

	//Add the "Compile" button.
	if (_CreateModelState._InputFolder && _CreateModelState._OutputFolder)
	{
		if (ImGui::Button("Compile!"))
		{
			CreateModelCompile();
		}
	}

	//End the window.
	ImGui::End();

	//It should stay open, for now.
	return true;
}

/*
*	Gathers files for create model.
*/
void EditorContentSystem::CreateModelGatherFiles(const char *const RESTRICT directory_path) NOEXCEPT
{
	//Iterate over all files in the folder.
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//Cache the entry path.
		const std::string entry_path{ entry.path().string() };

		//Gather recursively if this is a directory.
		if (entry.is_directory())
		{
			CreateModelGatherFiles(entry_path.c_str());

			continue;
		}

		//Retrieve the file extension.
		File::Extension file_extension{ File::GetExtension(entry_path.c_str()) };

		switch (file_extension)
		{
			case File::Extension::FBX:
			{
				_CreateModelState._ModelFiles.Emplace(entry_path.c_str());

				break;
			}

			case File::Extension::PNG:
			case File::Extension::JPG:
			{
				_CreateModelState._TextureFiles.Emplace(entry_path.c_str());

				break;
			}
		}
	}
}

/*
*	Estimates the files for create model.
*/
void EditorContentSystem::CreateModelEstimateFiles() NOEXCEPT
{
	//Estimate the level detail model files. For now, just add every one.
	_CreateModelState._LevelOfDetailModelFiles = _CreateModelState._ModelFiles;

	//Estimate the albedo texture file.
	for (const DynamicString &texture_file : _CreateModelState._TextureFiles)
	{
		if (texture_file.Find("Albedo")
			|| texture_file.Find("Color"))
		{
			_CreateModelState._AlbedoTextureFile = texture_file;

			break;
		}
	}

	//Estimate the normal map texture file.
	for (const DynamicString &texture_file : _CreateModelState._TextureFiles)
	{
		if (texture_file.Find("Normal"))
		{
			_CreateModelState._NormalMapTextureFile = texture_file;

			break;
		}
	}

	//Estimate the displacement texture file.
	for (const DynamicString &texture_file : _CreateModelState._TextureFiles)
	{
		if (texture_file.Find("Cavity")
			|| texture_file.Find("Displacement"))
		{
			_CreateModelState._DisplacementTextureFile = texture_file;

			break;
		}
	}

	//Estimate the roughness texture file.
	for (const DynamicString &texture_file : _CreateModelState._TextureFiles)
	{
		if (texture_file.Find("Roughness"))
		{
			_CreateModelState._RoughnessTextureFile = texture_file;

			break;
		}
	}

	//Estimate the metallic texture file.
	for (const DynamicString &texture_file : _CreateModelState._TextureFiles)
	{
		if (texture_file.Find("Metalness"))
		{
			_CreateModelState._MetallicTextureFile = texture_file;

			break;
		}
	}

	//Estimate the ambient occlusion texture file.
	for (const DynamicString &texture_file : _CreateModelState._TextureFiles)
	{
		if (texture_file.Find("AO")
			|| texture_file.Find("AmbientOcclusion"))
		{
			_CreateModelState._AmbientOcclusionTextureFile = texture_file;

			break;
		}
	}
}

/*
*	Compiles for create model.
*/
void EditorContentSystem::CreateModelCompile() NOEXCEPT
{
	//Figure out the name.
	std::string name;

	{
		const std::string input_folder{ _CreateModelState._InputFolder.Data() };
		const size_t position{ input_folder.find_last_of("\\") };

		name = input_folder.substr(position + 1);
	}

	//Write the model asset file.
	{
		char buffer[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(buffer, "%s\\%s.Model", _CreateModelState._OutputFolder.Data(), name.c_str());

		std::ofstream file{ buffer };

		for (const DynamicString &level_of_detail_model_file : _CreateModelState._LevelOfDetailModelFiles)
		{
			file << "LevelOfDetail(" << level_of_detail_model_file.Data() << ");" << std::endl;
		}

		if (_CreateModelState._CollisionModelFile)
		{
			file << "Collision(" << _CreateModelState._CollisionModelFile.Data() << ");" << std::endl;
		}

		file.close();
	}

	//Write the material asset file.
	{
		char buffer[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(buffer, "%s\\%s.Material", _CreateModelState._OutputFolder.Data(), name.c_str());

		std::ofstream file{ buffer };

		file << "Type(OPAQUE);" << std::endl;
		file << "AlbedoThicknessTexture(" << name.c_str() << "_AlbedoThickness);" << std::endl;
		file << "NormalMapDisplacementTexture(" << name.c_str() << "_NormalMapDisplacement);" << std::endl;
		file << "MaterialPropertiesTexture(" << name.c_str() << "_MaterialProperties);" << std::endl;

		file.close();
	}

	//Write the albedo/thickness asset file.
	{
		char buffer[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(buffer, "%s\\%s_AlbedoThickness.Texture2D", _CreateModelState._OutputFolder.Data(), name.c_str());

		std::ofstream file{ buffer };

		if (_CreateModelState._AlbedoTextureFile)
		{
			file << "File1(" << _CreateModelState._AlbedoTextureFile.Data() << ");" << std::endl;
			file << "ChannelMapping(RED, FILE_1, RED);" << std::endl;
			file << "ChannelMapping(GREEN, FILE_1, GREEN);" << std::endl;
			file << "ChannelMapping(BLUE, FILE_1, BLUE);" << std::endl;
		}

		else
		{
			file << "ChannelMapping(RED, DEFAULT, RED);" << std::endl;
			file << "ChannelMapping(GREEN, DEFAULT, GREEN);" << std::endl;
			file << "ChannelMapping(BLUE, DEFAULT, BLUE);" << std::endl;
		}

		file << "ChannelMapping(ALPHA, DEFAULT, ALPHA);" << std::endl;

		file << "Default(0.0, 0.0, 0.0, 1.0);" << std::endl;
		file << "ApplyGammaCorrection();" << std::endl;
		file << "BaseMipLevel(" << _CreateModelState._AlbedoThicknessBaseMipLevel << ");" << std::endl;
		file << "Compression(BC7, true);";

		file.close();
	}

	//Write the normal map/displacement asset file.
	{
		char buffer[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(buffer, "%s\\%s_NormalMapDisplacement.Texture2D", _CreateModelState._OutputFolder.Data(), name.c_str());

		std::ofstream file{ buffer };

		if (_CreateModelState._NormalMapTextureFile)
		{
			file << "File1(" << _CreateModelState._NormalMapTextureFile.Data() << ");" << std::endl;
			file << "ChannelMapping(RED, FILE_1, RED);" << std::endl;
			file << "ChannelMapping(GREEN, FILE_1, GREEN);" << std::endl;
			file << "ChannelMapping(BLUE, FILE_1, BLUE);" << std::endl;
		}

		else
		{
			file << "ChannelMapping(RED, DEFAULT, RED);" << std::endl;
			file << "ChannelMapping(GREEN, DEFAULT, GREEN);" << std::endl;
			file << "ChannelMapping(BLUE, DEFAULT, BLUE);" << std::endl;
		}

		if (_CreateModelState._DisplacementTextureFile)
		{
			file << "File4(" << _CreateModelState._DisplacementTextureFile.Data() << ");" << std::endl;
			file << "ChannelMapping(ALPHA, FILE_4, RED);" << std::endl;
		}

		else
		{
			file << "ChannelMapping(ALPHA, DEFAULT, ALPHA);" << std::endl;
		}

		file << "Default(0.5, 0.5, 1.0, 0.5);" << std::endl;
		file << "BaseMipLevel(" << _CreateModelState._NormalMapDisplacementBaseMipLevel << ");" << std::endl;
		file << "Compression(BC7, false);";

		file.close();
	}

	//Write the material properties asset file.
	{
		char buffer[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(buffer, "%s\\%s_MaterialProperties.Texture2D", _CreateModelState._OutputFolder.Data(), name.c_str());

		std::ofstream file{ buffer };

		if (_CreateModelState._RoughnessTextureFile)
		{
			file << "File1(" << _CreateModelState._RoughnessTextureFile.Data() << ");" << std::endl;
			file << "ChannelMapping(RED, FILE_1, RED);" << std::endl;
		}

		else
		{
			file << "ChannelMapping(RED, DEFAULT, RED);" << std::endl;
		}

		if (_CreateModelState._MetallicTextureFile)
		{
			file << "File2(" << _CreateModelState._MetallicTextureFile.Data() << ");" << std::endl;
			file << "ChannelMapping(GREEN, FILE_2, RED);" << std::endl;
		}

		else
		{
			file << "ChannelMapping(GREEN, DEFAULT, GREEN);" << std::endl;
		}

		if (_CreateModelState._AmbientOcclusionTextureFile)
		{
			file << "File3(" << _CreateModelState._AmbientOcclusionTextureFile.Data() << ");" << std::endl;
			file << "ChannelMapping(BLUE, FILE_3, RED);" << std::endl;
		}

		else
		{
			file << "ChannelMapping(BLUE, DEFAULT, BLUE);" << std::endl;
		}
		
		file << "ChannelMapping(ALPHA, DEFAULT, ALPHA);" << std::endl;

		file << "Default(1.0, 0.0, 1.0, 0.0);" << std::endl;
		file << "BaseMipLevel(" << _CreateModelState._MaterialPropertiesBaseMipLevel << ");" << std::endl;
		file << "Compression(BC7, false);";

		file.close();
	}
}
#endif