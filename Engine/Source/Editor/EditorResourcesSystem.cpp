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

	//Add the button for creating a material resource.
	if (ImGui::Button("Create Material Resource"))
	{
		//Reset the create material resource data.
		_CreateMaterialResourceData.~CreateMaterialResourceData();
		new (&_CreateMaterialResourceData) CreateMaterialResourceData();

		//Set the current create resource mode.
		_CurrentCreateResourceMode = CreateResourceMode::MATERIAL;
	}

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

		case CreateResourceMode::MATERIAL:
		{
			AddCreateMaterialResourceWindow();

			break;
		}

		case CreateResourceMode::MODEL:
		{
			AddCreateModelResourceWindow();

			break;
		}

		case CreateResourceMode::TEXTURE_2D:
		{
			AddCreateTexture2DResourceWindow();

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

/*
*	Adds the create material resource window.
*/
void EditorResourcesSystem::AddCreateMaterialResourceWindow() NOEXCEPT
{
	//Add the "Create Material Resource" window.
	ImGui::Begin("Create Material Resource", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowPos(ImVec2(256.0f, 256.0f));
	ImGui::SetWindowSize(ImVec2(512.0f, 512.0f));

	//If the user has already selected a file path, display it.
	if (_CreateMaterialResourceData._OutputFilePath.Data())
	{
		ImGui::Text("Output File Path:");
		ImGui::Text(_CreateMaterialResourceData._OutputFilePath.Data());
	}

	//Add the button to set the output file path/identifier.
	if (ImGui::Button("Select Output File Path"))
	{
		File::BrowseForFile(true, &_CreateMaterialResourceData._OutputFilePath);
	}

	//Add the button for switching the type of material.
	ImGui::Text("Material Type:");

	switch (_CreateMaterialResourceData._Type)
	{
		case MaterialResource::Type::MASKED:
		{
			if (ImGui::Button("Masked"))
			{
				_CreateMaterialResourceData._Type = MaterialResource::Type::OPAQUE;
			}

			break;
		}

		case MaterialResource::Type::OPAQUE:
		{
			if (ImGui::Button("Opaque"))
			{
				_CreateMaterialResourceData._Type = MaterialResource::Type::TRANSLUCENT;
			}

			break;
		}

		case MaterialResource::Type::TRANSLUCENT:
		{
			if (ImGui::Button("Translucent"))
			{
				_CreateMaterialResourceData._Type = MaterialResource::Type::MASKED;
			}

			break;
		}
	}

	//Add the widgets for the albedo/thickness component.
	switch (_CreateMaterialResourceData._AlbedoThicknessType)
	{
		case MaterialResource::MaterialResourceComponent::Type::COLOR:
		{
			if (ImGui::Button("Albedo/Thickness Component Type: Color"))
			{
				_CreateMaterialResourceData._AlbedoThicknessType = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
			}

			else
			{
				ImGui::ColorEdit4("Albedo/Thickness Color", &_CreateMaterialResourceData._AlbedoThicknessColor[0]);
			}

			break;
		}

		case MaterialResource::MaterialResourceComponent::Type::TEXTURE:
		{
			if (ImGui::Button("Albedo/Thickness Component Type: Texture"))
			{
				_CreateMaterialResourceData._AlbedoThicknessType = MaterialResource::MaterialResourceComponent::Type::COLOR;
			}

			else
			{
				ImGui::Text("Albedo/Thickness Texture: %s", _CreateMaterialResourceData._AlbedoThicknessTexture2DResource ? _CreateMaterialResourceData._AlbedoThicknessTexture2DResource->_Header._ResourceName.Data() : "Not selected");
				
				ImGui::BeginChild("Choose new Albedo/Thickness texture:", ImVec2(512.0f, 128.0f), true);

				const HashTable<HashString, Texture2DResource *RESTRICT> &all_texture_2D_resources{ ResourceSystem::Instance->GetAllTexture2DResources() };

				for (const Texture2DResource *const RESTRICT texture_2D_resource : all_texture_2D_resources.ValueIterator())
				{
					if (ImGui::Button(texture_2D_resource->_Header._ResourceName.Data()))
					{
						_CreateMaterialResourceData._AlbedoThicknessTexture2DResource = ResourceSystem::Instance->GetTexture2DResource(texture_2D_resource->_Header._ResourceIdentifier);

						break;
					}
				}

				ImGui::EndChild();
			}

			break;
		}
	}

	//Add the widgets for the normal map/displacement component.
	switch (_CreateMaterialResourceData._NormalMapDisplacementType)
	{
		case MaterialResource::MaterialResourceComponent::Type::COLOR:
		{
			if (ImGui::Button("Normal Map/Displacement Component Type: Color"))
			{
				_CreateMaterialResourceData._NormalMapDisplacementType = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
			}

			else
			{
				ImGui::ColorEdit4("Normal Map/Displacement Color", &_CreateMaterialResourceData._NormalMapDisplacementColor[0]);
			}

			break;
		}

		case MaterialResource::MaterialResourceComponent::Type::TEXTURE:
		{
			if (ImGui::Button("Normal Map/Displacement Component Type: Texture"))
			{
				_CreateMaterialResourceData._NormalMapDisplacementType = MaterialResource::MaterialResourceComponent::Type::COLOR;
			}

			else
			{
				ImGui::Text("Normal Map/Displacement Texture: %s", _CreateMaterialResourceData._NormalMapDisplacementTexture2DResource ? _CreateMaterialResourceData._NormalMapDisplacementTexture2DResource->_Header._ResourceName.Data() : "Not selected");

				ImGui::BeginChild("Choose new Normal Map/Displacements texture:", ImVec2(512.0f, 128.0f), true);

				const HashTable<HashString, Texture2DResource *RESTRICT> &all_texture_2D_resources{ ResourceSystem::Instance->GetAllTexture2DResources() };

				for (const Texture2DResource *const RESTRICT texture_2D_resource : all_texture_2D_resources.ValueIterator())
				{
					if (ImGui::Button(texture_2D_resource->_Header._ResourceName.Data()))
					{
						_CreateMaterialResourceData._NormalMapDisplacementTexture2DResource = ResourceSystem::Instance->GetTexture2DResource(texture_2D_resource->_Header._ResourceIdentifier);

						break;
					}
				}

				ImGui::EndChild();
			}

			break;
		}
	}

	//Add the widgets for the material properties component.
	switch (_CreateMaterialResourceData._MaterialPropertiesType)
	{
		case MaterialResource::MaterialResourceComponent::Type::COLOR:
		{
			if (ImGui::Button("Material Properties Component Type: Color"))
			{
				_CreateMaterialResourceData._MaterialPropertiesType = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
			}

			else
			{
				ImGui::ColorEdit4("Material Properties Color", &_CreateMaterialResourceData._MaterialPropertiesColor[0]);
			}

			break;
		}

		case MaterialResource::MaterialResourceComponent::Type::TEXTURE:
		{
			if (ImGui::Button("Material Properties Component Type: Texture"))
			{
				_CreateMaterialResourceData._MaterialPropertiesType = MaterialResource::MaterialResourceComponent::Type::COLOR;
			}

			else
			{
				ImGui::Text("Material Properties Texture: %s", _CreateMaterialResourceData._MaterialPropertiesTexture2DResource ? _CreateMaterialResourceData._MaterialPropertiesTexture2DResource->_Header._ResourceName.Data() : "Not selected");

				ImGui::BeginChild("Choose new Material Properties texture:", ImVec2(512.0f, 128.0f), true);

				const HashTable<HashString, Texture2DResource *RESTRICT> &all_texture_2D_resources{ ResourceSystem::Instance->GetAllTexture2DResources() };

				for (const Texture2DResource *const RESTRICT texture_2D_resource : all_texture_2D_resources.ValueIterator())
				{
					if (ImGui::Button(texture_2D_resource->_Header._ResourceName.Data()))
					{
						_CreateMaterialResourceData._MaterialPropertiesTexture2DResource = ResourceSystem::Instance->GetTexture2DResource(texture_2D_resource->_Header._ResourceIdentifier);

						break;
					}
				}

				ImGui::EndChild();
			}

			break;
		}
	}

	//Add the widgets for the opacity component.
	switch (_CreateMaterialResourceData._OpacityType)
	{
		case MaterialResource::MaterialResourceComponent::Type::COLOR:
		{
			if (ImGui::Button("Opacity Component Type: Color"))
			{
				_CreateMaterialResourceData._OpacityType = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
			}

			else
			{
				ImGui::ColorEdit4("Opacity Color", &_CreateMaterialResourceData._OpacityColor[0]);
			}

			break;
		}

		case MaterialResource::MaterialResourceComponent::Type::TEXTURE:
		{
			if (ImGui::Button("Opacity Component Type: Texture"))
			{
				_CreateMaterialResourceData._OpacityType = MaterialResource::MaterialResourceComponent::Type::COLOR;
			}

			else
			{
				ImGui::Text("Opacity Texture: %s", _CreateMaterialResourceData._OpacityTexture2DResource ? _CreateMaterialResourceData._OpacityTexture2DResource->_Header._ResourceName.Data() : "Not selected");

				ImGui::BeginChild("Choose new Opacity texture:", ImVec2(512.0f, 128.0f), true);

				const HashTable<HashString, Texture2DResource *RESTRICT> &all_texture_2D_resources{ ResourceSystem::Instance->GetAllTexture2DResources() };

				for (const Texture2DResource *const RESTRICT texture_2D_resource : all_texture_2D_resources.ValueIterator())
				{
					if (ImGui::Button(texture_2D_resource->_Header._ResourceName.Data()))
					{
						_CreateMaterialResourceData._OpacityTexture2DResource = ResourceSystem::Instance->GetTexture2DResource(texture_2D_resource->_Header._ResourceIdentifier);

						break;
					}
				}

				ImGui::EndChild();
			}

			break;
		}
	}

	//Add the widget for the emissive multiplier.
	ImGui::DragFloat("Emissive Multiplier", &_CreateMaterialResourceData._EmissiveMultiplier, 0.1f, 0.0f);

	//Add the widget for defining whether or not the material is double-sided.
	ImGui::Checkbox("Double Sided", &_CreateMaterialResourceData._DoubleSided);

	//Add some padding before the "Create Material Resource" button.
	ImGui::Text("");

	//Add the create button.
	if (ImGui::Button("Create Material Resource"))
	{
		//Retrieve the identifier.
		DynamicString identifier;

		for (int64 i{ static_cast<int64>(_CreateMaterialResourceData._OutputFilePath.Length()) - 1 }; i >= 0; --i)
		{
			if (_CreateMaterialResourceData._OutputFilePath[i] == '\\')
			{
				identifier = &_CreateMaterialResourceData._OutputFilePath[i + 1];

				break;
			}
		}

		//Build the material.
		{
			MaterialBuildParameters parameters;

			parameters._Output = _CreateMaterialResourceData._OutputFilePath.Data();
			parameters._ID = identifier.Data();
			parameters._Type = _CreateMaterialResourceData._Type;

			parameters._AlbedoThicknessComponent._Type = _CreateMaterialResourceData._AlbedoThicknessType;

			switch (_CreateMaterialResourceData._AlbedoThicknessType)
			{
				case MaterialResource::MaterialResourceComponent::Type::COLOR:
				{
					parameters._AlbedoThicknessComponent._Color = _CreateMaterialResourceData._AlbedoThicknessColor;

					break;
				}

				case MaterialResource::MaterialResourceComponent::Type::TEXTURE:
				{
					parameters._AlbedoThicknessComponent._TextureResourceIdentifier = _CreateMaterialResourceData._AlbedoThicknessTexture2DResource->_Header._ResourceIdentifier;

					break;
				}
			}

			parameters._NormalMapDisplacementComponent._Type = _CreateMaterialResourceData._NormalMapDisplacementType;

			switch (_CreateMaterialResourceData._NormalMapDisplacementType)
			{
				case MaterialResource::MaterialResourceComponent::Type::COLOR:
				{
					parameters._NormalMapDisplacementComponent._Color = _CreateMaterialResourceData._NormalMapDisplacementColor;

					break;
				}

				case MaterialResource::MaterialResourceComponent::Type::TEXTURE:
				{
					parameters._NormalMapDisplacementComponent._TextureResourceIdentifier = _CreateMaterialResourceData._NormalMapDisplacementTexture2DResource->_Header._ResourceIdentifier;

					break;
				}
			}

			parameters._MaterialPropertiesComponent._Type = _CreateMaterialResourceData._MaterialPropertiesType;

			switch (_CreateMaterialResourceData._MaterialPropertiesType)
			{
				case MaterialResource::MaterialResourceComponent::Type::COLOR:
				{
					parameters._MaterialPropertiesComponent._Color = _CreateMaterialResourceData._MaterialPropertiesColor;

					break;
				}

				case MaterialResource::MaterialResourceComponent::Type::TEXTURE:
				{
					parameters._MaterialPropertiesComponent._TextureResourceIdentifier = _CreateMaterialResourceData._MaterialPropertiesTexture2DResource->_Header._ResourceIdentifier;

					break;
				}
			}

			parameters._OpacityComponent._Type = _CreateMaterialResourceData._OpacityType;

			switch (_CreateMaterialResourceData._OpacityType)
			{
				case MaterialResource::MaterialResourceComponent::Type::COLOR:
				{
					parameters._OpacityComponent._Color = _CreateMaterialResourceData._OpacityColor;

					break;
				}

				case MaterialResource::MaterialResourceComponent::Type::TEXTURE:
				{
					parameters._OpacityComponent._TextureResourceIdentifier = _CreateMaterialResourceData._OpacityTexture2DResource->_Header._ResourceIdentifier;

					break;
				}
			}

			parameters._EmissiveMultiplier = _CreateMaterialResourceData._EmissiveMultiplier;
			parameters._DoubleSided = _CreateMaterialResourceData._DoubleSided;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildMaterial(parameters);
		}

		//Now load the material.
		_CreateMaterialResourceData._OutputFilePath += ".cr";
		ResourceSystem::Instance->LoadResource(_CreateMaterialResourceData._OutputFilePath.Data());

		//No longer creating a resource.
		_CurrentCreateResourceMode = CreateResourceMode::NONE;
	}

	ImGui::End();
}

/*
*	Adds the create model resource window.
*/
void EditorResourcesSystem::AddCreateModelResourceWindow() NOEXCEPT
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

			parameters._Transformation = MatrixConstants::IDENTITY;
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
}

/*
*	Adds the create texture 2D resource window.
*/
void EditorResourcesSystem::AddCreateTexture2DResourceWindow() NOEXCEPT
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
		File::BrowseForFile(false, &_CreateTexture2DResourceData._File1FilePath);
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
		File::BrowseForFile(false, &_CreateTexture2DResourceData._File2FilePath);
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
		File::BrowseForFile(false, &_CreateTexture2DResourceData._File3FilePath);
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
		File::BrowseForFile(false, &_CreateTexture2DResourceData._File4FilePath);
	}

	//Add the widget to configure the defaults.
	ImGui::DragFloat4("Default", &_CreateTexture2DResourceData._Default[0]);

	//Add widgets for all channel mappings.
	for (uint8 i{ 0 }; i < 4; ++i)
	{
		char buffer[64];

		switch (_CreateTexture2DResourceData._ChannelMappings[i]._File)
		{
			case Texture2DBuildParameters::File::FILE_1:
			{
				sprintf_s(buffer, "Channel Mapping %u File: File 1", i + 1);

				if (ImGui::Button(buffer))
				{
					_CreateTexture2DResourceData._ChannelMappings[i]._File = Texture2DBuildParameters::File::FILE_2;
				}

				break;
			}

			case Texture2DBuildParameters::File::FILE_2:
			{
				sprintf_s(buffer, "Channel Mapping %u File: File 2", i + 1);

				if (ImGui::Button(buffer))
				{
					_CreateTexture2DResourceData._ChannelMappings[i]._File = Texture2DBuildParameters::File::FILE_3;
				}

				break;
			}

			case Texture2DBuildParameters::File::FILE_3:
			{
				sprintf_s(buffer, "Channel Mapping %u File: File 3", i + 1);

				if (ImGui::Button(buffer))
				{
					_CreateTexture2DResourceData._ChannelMappings[i]._File = Texture2DBuildParameters::File::FILE_4;
				}

				break;
			}

			case Texture2DBuildParameters::File::FILE_4:
			{
				sprintf_s(buffer, "Channel Mapping %u File: File 4", i + 1);

				if (ImGui::Button(buffer))
				{
					_CreateTexture2DResourceData._ChannelMappings[i]._File = Texture2DBuildParameters::File::DEFAULT;
				}

				break;
			}

			case Texture2DBuildParameters::File::DEFAULT:
			{
				sprintf_s(buffer, "Channel Mapping %u File: Default", i + 1);

				if (ImGui::Button(buffer))
				{
					_CreateTexture2DResourceData._ChannelMappings[i]._File = Texture2DBuildParameters::File::FILE_1;
				}

				break;
			}
		}

		switch (_CreateTexture2DResourceData._ChannelMappings[i]._Channel)
		{
			case Texture2DBuildParameters::Channel::RED:
			{
				sprintf_s(buffer, "Channel Mapping %u Channel: Red", i + 1);

				if (ImGui::Button(buffer))
				{
					_CreateTexture2DResourceData._ChannelMappings[i]._Channel = Texture2DBuildParameters::Channel::GREEN;
				}

				break;
			}

			case Texture2DBuildParameters::Channel::GREEN:
			{
				sprintf_s(buffer, "Channel Mapping %u Channel: Green", i + 1);

				if (ImGui::Button(buffer))
				{
					_CreateTexture2DResourceData._ChannelMappings[i]._Channel = Texture2DBuildParameters::Channel::BLUE;
				}

				break;
			}

			case Texture2DBuildParameters::Channel::BLUE:
			{
				sprintf_s(buffer, "Channel Mapping %u Channel: Blue", i + 1);

				if (ImGui::Button(buffer))
				{
					_CreateTexture2DResourceData._ChannelMappings[i]._Channel = Texture2DBuildParameters::Channel::ALPHA;
				}

				break;
			}

			case Texture2DBuildParameters::Channel::ALPHA:
			{
				sprintf_s(buffer, "Channel Mapping %u Channel: Alpha", i + 1);

				if (ImGui::Button(buffer))
				{
					_CreateTexture2DResourceData._ChannelMappings[i]._Channel = Texture2DBuildParameters::Channel::RED;
				}

				break;
			}
		}
	}

	//Add the checkbox on whether or not to apply gamma correction.
	ImGui::Checkbox("Apply Gamma Correction", &_CreateTexture2DResourceData._ApplyGammaCorrection);

	//Add the button to select the base mipmap level.
	ImGui::SetNextItemWidth(64.0f);
	ImGui::DragInt("Base Mipmap Level", &_CreateTexture2DResourceData._BaseMipmapLevel, 1.0f, 0, 16);

	//Add the button to select the number of mipmap levels.
	ImGui::SetNextItemWidth(64.0f);
	ImGui::DragInt("Number Of Mipmaps Level", &_CreateTexture2DResourceData._NumberOfMipmapLevels, 1.0f, 0, 16);

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
			parameters._Default = _CreateTexture2DResourceData._Default;
			parameters._ChannelMappings[0] = _CreateTexture2DResourceData._ChannelMappings[0];
			parameters._ChannelMappings[1] = _CreateTexture2DResourceData._ChannelMappings[1];
			parameters._ChannelMappings[2] = _CreateTexture2DResourceData._ChannelMappings[2];
			parameters._ChannelMappings[3] = _CreateTexture2DResourceData._ChannelMappings[3];
			parameters._ApplyGammaCorrection = _CreateTexture2DResourceData._ApplyGammaCorrection;
			parameters._TransformFunction = nullptr;
			parameters._BaseMipmapLevel = static_cast<uint8>(_CreateTexture2DResourceData._BaseMipmapLevel);
			parameters._MipmapLevels = static_cast<uint8>(_CreateTexture2DResourceData._NumberOfMipmapLevels);

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
		}

		//Now load the texture 2D.
		_CreateTexture2DResourceData._OutputFilePath += ".cr";
		ResourceSystem::Instance->LoadResource(_CreateTexture2DResourceData._OutputFilePath.Data());

		//No longer creating a resource.
		_CurrentCreateResourceMode = CreateResourceMode::NONE;
	}

	ImGui::End();
}
#endif