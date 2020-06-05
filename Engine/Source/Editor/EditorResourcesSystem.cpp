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
	ImGui::SetWindowPos(ImVec2(8.0f, 8.0f + 256.0f + 8.0f));
	ImGui::SetWindowSize(ImVec2(256.0f, 512.0f));

	//Add the button for creating a model resource.
	if (ImGui::Button("Create Model Resource"))
	{
		_CurrentCreateResourceMode = CreateResourceMode::MODEL;
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
			//Add the entities window.
			ImGui::Begin("Create Model Resource", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			ImGui::SetWindowPos(ImVec2(8.0f + 256.0f + 8.0f, 8.0f + 256.0f + 8.0f));
			ImGui::SetWindowSize(ImVec2(512.0f, 512.0f));

			//If the user has already selected a file path, display it.
			if (_CreateModelResourceData._OutputFilePath.Data())
			{
				ImGui::Text("File Path:");
				ImGui::Text(_CreateModelResourceData._OutputFilePath.Data());
			}

			//Add the button to set the output file path/identifier.
			if (ImGui::Button("Select File Path"))
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
			if (ImGui::Button("Add Level Of Detail"))
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

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	ImGui::End();
}
#endif