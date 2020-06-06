#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorResourcesSystem.h>

//File.
#include <File/Core/FileCore.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/ResourceSystem.h>

//Third party.
#include <ThirdParty/imgui.h>

/*
*	Updates the editor resources system.
*/
void EditorResourcesSystem::Update() NOEXCEPT
{
	//Is the current contextual window RESOURCES?
	if (CatalystEditorSystem::Instance->GetCurrentContextualWindow() != CatalystEditorSystem::ContextualWindow::RESOURCES)
	{
		_CurrentCreateResourceMode = CreateResourceMode::NONE;

		return;
	}

	//Add the entities window.
	ImGui::Begin("Resources", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowPos(ImVec2(0.0f, 256.0f));
	ImGui::SetWindowSize(ImVec2(256.0f, 512.0f));

	//Add the button for creating a model resource.
	if (ImGui::Button("Create Model Resource"))
	{
		//Reset the create model resource data.
		_CreateModelResourceData.~CreateModelResourceData();
		new (&_CreateModelResourceData) CreateModelResourceData();

		//Set the current create resource mode.
		_CurrentCreateResourceMode = CreateResourceMode::MODEL;
	}

	//Add the button for creating a texture 2D resource.
	if (ImGui::Button("Create Texture 2D Resource"))
	{
		//Reset the create model resource data.
		_CreateTexture2DResourceData.~CreateTexture2DResourceData();
		new (&_CreateTexture2DResourceData) CreateTexture2DResourceData();

		//Set the current create resource mode.
		_CurrentCreateResourceMode = CreateResourceMode::TEXTURE_2D;
	}

	switch (_CurrentCreateResourceMode)
	{
		case CreateResourceMode::NONE:
		{
			//HWEL. Nothing to do here.

			break;
		}

		case CreateResourceMode::MODEL:
		{
			//Add the "Create Model Resource" window.
			ImGui::Begin("Create Model Resource", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			ImGui::SetWindowPos(ImVec2(256.0f, 256.0f));
			ImGui::SetWindowSize(ImVec2(512.0f, 512.0f));

			//If the user has already selected a file path, display it.
			if (_CreateModelResourceData._OutputFilePath.Data())
			{
				ImGui::Text("Output File Path:");
				ImGui::Text(_CreateModelResourceData._OutputFilePath.Data());
			}

			//Add the button to set the output file path/identifier.
			if (ImGui::Button("Select Output File Path"))
			{
				File::BrowseForFile(true, &_CreateModelResourceData._OutputFilePath);
			}

			//If the user has already added level of details, display them.
			uint64 level_of_detail_counter{ 0 };

			for (uint64 i{ 0 }, size{ _CreateModelResourceData._LevelOfDetailFilepaths.Size() }; i < size; ++i)
			{
				ImGui::Text("Level Of Detail #%llu File Path:", level_of_detail_counter + 1);
				ImGui::Text(_CreateModelResourceData._LevelOfDetailFilepaths[i].Data());

				++level_of_detail_counter;
			}

			//Add the button for adding level of detail file paths.
			if (ImGui::Button("Add Level Of Detail File Path"))
			{
				DynamicString chosen_file;

				if (File::BrowseForFile(false, &chosen_file))
				{
					_CreateModelResourceData._LevelOfDetailFilepaths.Emplace(chosen_file);
				}
			}

			//Add some padding before the "Create Model Resource" button.
			ImGui::Text("");

			//Add the create button.
			if (ImGui::Button("Create Model Resource"))
			{
				//Retrieve the identifier.
				DynamicString identifier;

				for (int64 i{ static_cast<int64>(_CreateModelResourceData._OutputFilePath.Length()) - 1 }; i >= 0; --i)
				{
					if (_CreateModelResourceData._OutputFilePath[i] == '\\')
					{
						identifier = &_CreateModelResourceData._OutputFilePath[i + 1];

						break;
					}
				}

				//Build the model.
				{
					ModelBuildParameters parameters;

					parameters._Output = _CreateModelResourceData._OutputFilePath.Data();
					parameters._ID = identifier.Data();

					for (const DynamicString &level_of_detail_file_path : _CreateModelResourceData._LevelOfDetailFilepaths)
					{
						parameters._LevelOfDetails.Emplace(level_of_detail_file_path.Data());
					}

					parameters._Transformation = Matrix4x4(VectorConstants::ZERO, Vector3<float32>(-CatalystBaseMathConstants::HALF_PI, 0.0f, 0.0f), VectorConstants::ONE);
					parameters._TextureCoordinateMultiplier = 1.0f;
					parameters._TexturCoordinateRotation = 0.0f;

					ResourceSystem::Instance->GetResourceBuildingSystem()->BuildModel(parameters);
				}

				//Now load the model.
				_CreateModelResourceData._OutputFilePath += ".cr";
				ResourceSystem::Instance->LoadResource(_CreateModelResourceData._OutputFilePath.Data());

				//No longer creating a resource.
				_CurrentCreateResourceMode = CreateResourceMode::NONE;
			}

			ImGui::End();

			break;
		}

		case CreateResourceMode::TEXTURE_2D:
		{
			//Add the "Create Texture 2D Resource" window.
			ImGui::Begin("Create Texture 2D Resource", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			ImGui::SetWindowPos(ImVec2(256.0f, 256.0f));
			ImGui::SetWindowSize(ImVec2(512.0f, 512.0f));

			//If the user has already selected a file path, display it.
			if (_CreateTexture2DResourceData._OutputFilePath.Data())
			{
				ImGui::Text("Output File Path:");
				ImGui::Text(_CreateTexture2DResourceData._OutputFilePath.Data());
			}

			//Add the button to set the output file path/identifier.
			if (ImGui::Button("Select Output File Path"))
			{
				File::BrowseForFile(true, &_CreateTexture2DResourceData._OutputFilePath);
			}

			//If the user has already selected a file path for file 1, display it.
			if (_CreateTexture2DResourceData._File1FilePath.Data())
			{
				ImGui::Text("File 1 File Path:");
				ImGui::Text(_CreateTexture2DResourceData._File1FilePath.Data());
			}

			//Add the button to set the file 1 file path.
			if (ImGui::Button("Select File 1 File Path"))
			{
				File::BrowseForFile(true, &_CreateTexture2DResourceData._File1FilePath);
			}

			//If the user has already selected a file path for file 2, display it.
			if (_CreateTexture2DResourceData._File2FilePath.Data())
			{
				ImGui::Text("File 2 File Path:");
				ImGui::Text(_CreateTexture2DResourceData._File2FilePath.Data());
			}

			//Add the button to set the file 2 file path.
			if (ImGui::Button("Select File 2 File Path"))
			{
				File::BrowseForFile(true, &_CreateTexture2DResourceData._File2FilePath);
			}

			//If the user has already selected a file path for file 3, display it.
			if (_CreateTexture2DResourceData._File3FilePath.Data())
			{
				ImGui::Text("File 3 File Path:");
				ImGui::Text(_CreateTexture2DResourceData._File3FilePath.Data());
			}

			//Add the button to set the file 3 file path.
			if (ImGui::Button("Select File 3 File Path"))
			{
				File::BrowseForFile(true, &_CreateTexture2DResourceData._File3FilePath);
			}

			//If the user has already selected a file path for file 4, display it.
			if (_CreateTexture2DResourceData._File4FilePath.Data())
			{
				ImGui::Text("File 4 File Path:");
				ImGui::Text(_CreateTexture2DResourceData._File4FilePath.Data());
			}

			//Add the button to set the file 4 file path.
			if (ImGui::Button("Select File 4 File Path"))
			{
				File::BrowseForFile(true, &_CreateTexture2DResourceData._File4FilePath);
			}

			//Add the widget to configure the defaults.
			ImGui::DragFloat4("Default", _CreateTexture2DResourceData._Default.Data());

			//Add some padding before the "Create Model Resource" button.
			ImGui::Text("");

			//Add the create button.
			if (ImGui::Button("Create Texture 2D Resource"))
			{
				//Retrieve the identifier.
				DynamicString identifier;

				for (int64 i{ static_cast<int64>(_CreateTexture2DResourceData._OutputFilePath.Length()) - 1 }; i >= 0; --i)
				{
					if (_CreateTexture2DResourceData._OutputFilePath[i] == '\\')
					{
						identifier = &_CreateTexture2DResourceData._OutputFilePath[i + 1];

						break;
					}
				}

				//Build the texture 2D.
				{
					Texture2DBuildParameters parameters;

					parameters._Output = _CreateTexture2DResourceData._OutputFilePath.Data();
					parameters._ID = identifier.Data();
					parameters._DefaultWidth = 0;
					parameters._DefaultHeight = 0;
					parameters._File1 = _CreateTexture2DResourceData._File1FilePath.Data();
					parameters._File2 = _CreateTexture2DResourceData._File2FilePath.Data();
					parameters._File3 = _CreateTexture2DResourceData._File3FilePath.Data();
					parameters._File4 = _CreateTexture2DResourceData._File4FilePath.Data();
					parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
					parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
					parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
					parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
					parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
					parameters._ApplyGammaCorrection = true;
					parameters._TransformFunction = nullptr;
					parameters._BaseMipmapLevel = 1;
					parameters._MipmapLevels = 7;

					ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
				}

				//Now load the model.
				_CreateTexture2DResourceData._OutputFilePath += ".cr";
				ResourceSystem::Instance->LoadResource(_CreateTexture2DResourceData._OutputFilePath.Data());

				//No longer creating a resource.
				_CurrentCreateResourceMode = CreateResourceMode::NONE;
			}

			ImGui::End();

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	ImGui::End();
}
#endif