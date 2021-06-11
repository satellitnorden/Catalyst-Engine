#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorResourcesSystem.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>

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

	//Add the button for creating a Quixel material resource.
	if (ImGui::Button("Create Quixel Material Resource"))
	{
		//Reset the create model resource data.
		_CreateQuixelMaterialResourceData.~CreateQuixelMaterialResourceData();
		new (&_CreateQuixelMaterialResourceData) CreateQuixelMaterialResourceData();

		//Set the current create resource mode.
		_CurrentCreateResourceMode = CreateResourceMode::QUIXEL_MATERIAL;
	}

	//Add the button for creating a Quixel model resource.
	if (ImGui::Button("Create Quixel Model Resource"))
	{
		//Reset the create model resource data.
		_CreateQuixelModelResourceData.~CreateQuixelModelResourceData();
		new (&_CreateQuixelModelResourceData) CreateQuixelModelResourceData();

		//Set the current create resource mode.
		_CurrentCreateResourceMode = CreateResourceMode::QUIXEL_MODEL;
	}

	//Add the button for creating a level from .obj and .mtl files resource.
	if (ImGui::Button("Create Level From .obj And .mtl Files Resource"))
	{
		//Reset the create model resource data.
		_CreateLevelResourceFromObjAndMtlData.~CreateLevelResourceFromObjAndMtlData();
		new (&_CreateLevelResourceFromObjAndMtlData) CreateLevelResourceFromObjAndMtlData();

		//Set the current create resource mode.
		_CurrentCreateResourceMode = CreateResourceMode::LEVEL_FROM_OBJ_AND_MTL_FILES;
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

		case CreateResourceMode::QUIXEL_MATERIAL:
		{
			AddCreateQuixelMaterialResourceWindow();

			break;
		}

		case CreateResourceMode::QUIXEL_MODEL:
		{
			AddCreateQuixelModelResourceWindow();

			break;
		}

		case CreateResourceMode::LEVEL_FROM_OBJ_AND_MTL_FILES:
		{
			AddCreateLevelResourceFromObjAndMtlFilesWindow();

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

	//Create the "Blender Transform" checkbox.
	ImGui::Checkbox("Apply Blender Transform", &_CreateModelResourceData._ApplyBlenderTransform);

	//If the user has already selected a collision model file path, display it.
	if (_CreateModelResourceData._CollisionModelFilePath.Data())
	{
		ImGui::Text("Collision Model File Path:");
		ImGui::Text(_CreateModelResourceData._CollisionModelFilePath.Data());
	}

	//Add the button to set the collision model file path.
	if (ImGui::Button("Select Collision Model File Path"))
	{
		File::BrowseForFile(true, &_CreateModelResourceData._CollisionModelFilePath);
	}

	//Add some padding before the "Create Model Resource" button.
	ImGui::Text("");

	//Add the create button.
	if (ImGui::Button("Create Model Resource"))
	{
		//If the user has already written the ".cr" extension, or if the user is aiming to replace an existing file, remove the extension as it's added automatically by the resource building system.
		if (StringUtilities::IsEqual(&_CreateModelResourceData._OutputFilePath[_CreateModelResourceData._OutputFilePath.Length() - 3], ".cr"))
		{
			_CreateModelResourceData._OutputFilePath.SetLength(_CreateModelResourceData._OutputFilePath.Length() - 3);
		}

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
			parameters._ResourceIdentifier = identifier.Data();

			for (const DynamicString &level_of_detail_file_path : _CreateModelResourceData._LevelOfDetailFilepaths)
			{
				parameters._LevelOfDetails.Emplace(level_of_detail_file_path.Data());
			}

			if (_CreateModelResourceData._ApplyBlenderTransform)
			{
				parameters._Transformation = Matrix4x4(VectorConstants::ZERO, EulerAngles(-CatalystBaseMathConstants::HALF_PI, 0.0f, 0.0f), VectorConstants::ONE);
			}

			else
			{
				parameters._Transformation = MatrixConstants::IDENTITY;
			}

			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = _CreateModelResourceData._CollisionModelFilePath.Data();

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

/*
*	Adds the create Quixel material resource window.
*/
void EditorResourcesSystem::AddCreateQuixelMaterialResourceWindow() NOEXCEPT
{
	/*
	*	Create Quixel material resource temporary data class definition.
	*/
	class CreateQuixelMaterialResourceTemporaryData final
	{

	public:

		//The identifier.
		DynamicString _Identifier;

		//The base mipmap level.
		uint8 _BaseMipmapLevel;

		//The number of mipmap levels.
		uint8 _NumberOfMipmapLevels;

		//The albedo texture file path.
		DynamicString _AlbedoTextureFilePath;

		//The normal map texture file path.
		DynamicString _NormalMapTextureFilePath;

		//The displacement texture file path.
		DynamicString _DisplacementTextureFilePath;

		//The roughness texture file path.
		DynamicString _RoughnessTextureFilePath;

		//The metallic texture file path.
		DynamicString _MetallicTextureFilePath;

		//The ambient occlusion texture file path.
		DynamicString _AmbientOcclusionTextureFilePath;

	};

	//Add the "Create Quixel Model Resource" window.
	ImGui::Begin("Create Quixel Material Resource", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowPos(ImVec2(256.0f, 256.0f));
	ImGui::SetWindowSize(ImVec2(512.0f, 512.0f));

	//If the user has already selected a directory path, display it.
	if (_CreateQuixelMaterialResourceData._DirectoryPath.Data())
	{
		ImGui::Text("Directory Path:");
		ImGui::Text(_CreateQuixelMaterialResourceData._DirectoryPath.Data());
	}

	//Add the button to set the directory path.
	if (ImGui::Button("Select Directory Path"))
	{
		File::BrowseForFolder(&_CreateQuixelMaterialResourceData._DirectoryPath);
	}

	//Add some padding before the "Create Quixel Model Resource" button.
	ImGui::Text("");

	//Add the create button.
	if (ImGui::Button("Create Quixel Material Resource"))
	{
		//Browse through all files in the directory path and fill in the temporary data.
		CreateQuixelMaterialResourceTemporaryData temporary_data;

		//Extract the identifier.
		for (int64 i{ static_cast<int64>(_CreateQuixelMaterialResourceData._DirectoryPath.Length()) - 1 }; i >= 0; --i)
		{
			if (_CreateQuixelMaterialResourceData._DirectoryPath[i] == '\\')
			{
				temporary_data._Identifier = &_CreateQuixelMaterialResourceData._DirectoryPath[i + 1];

				break;
			}
		}

		//Iterate over all the files in the directory and load them.
		for (const auto &entry : std::filesystem::directory_iterator(std::string(_CreateQuixelMaterialResourceData._DirectoryPath.Data()) + "\\Raw"))
		{
			ASSERT(!entry.is_directory(), "EditorResourcesSystem::AddCreateQuixelMaterialResourceWindow() failure!");

			//Cache the file path.
			const DynamicString entry_file_path{ entry.path().generic_u8string().c_str() };

			//Cache the file extension.
			const File::Extension file_extension{ File::GetExtension(entry_file_path.Data()) };

			//Is this a texture?
			if (file_extension == File::Extension::JPG)
			{
				//Is this the albedo texture?
				if (entry_file_path.Find("_Albedo."))
				{
					temporary_data._AlbedoTextureFilePath = entry_file_path;

					//Determine the base/number of mipmap level(s).
					if (entry_file_path.Find("_8K_"))
					{
						temporary_data._BaseMipmapLevel = 3;
						temporary_data._NumberOfMipmapLevels = 10;
					}

					else
					{
						temporary_data._BaseMipmapLevel = 2;
						temporary_data._NumberOfMipmapLevels = 9;
					}
				}

				//Is this the normnal map texture?
				if (entry_file_path.Find("_Normal."))
				{
					temporary_data._NormalMapTextureFilePath = entry_file_path;
				}

				//Is this the displacement texture?
				if (entry_file_path.Find("_Displacement."))
				{
					temporary_data._DisplacementTextureFilePath = entry_file_path;
				}

				//Is this the roughness texture?
				if (entry_file_path.Find("_Roughness."))
				{
					temporary_data._RoughnessTextureFilePath = entry_file_path;
				}

				//Is this the metallic texture?
				if (entry_file_path.Find("_Metalness."))
				{
					temporary_data._MetallicTextureFilePath = entry_file_path;
				}

				//Is this the ambient occlusion texture?
				if (entry_file_path.Find("_AO."))
				{
					temporary_data._AmbientOcclusionTextureFilePath = entry_file_path;
				}
			}
		}

		//Create all the directories, if needed.
		{
			char buffer[128];

			sprintf_s(buffer, "..\\..\\..\\Resources\\Intermediate\\Textures");
			File::CreateDirectory(buffer);

			sprintf_s(buffer, "..\\..\\..\\Resources\\Intermediate\\Textures\\%s", temporary_data._Identifier.Data());
			File::CreateDirectory(buffer);

			sprintf_s(buffer, "..\\..\\..\\Resources\\Intermediate\\Materials");
			File::CreateDirectory(buffer);
		}

		//Create the albedo/thickness texture 2D.
		{
			Texture2DBuildParameters parameters;

			char output_file_path_buffer[128];
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Textures\\%s\\%s_AlbedoThickness_Texture2D", temporary_data._Identifier.Data(), temporary_data._Identifier.Data());
			parameters._Output = output_file_path_buffer;

			char identifier_buffer[128];
			sprintf_s(identifier_buffer, "%s_AlbedoThickness_Texture2D", temporary_data._Identifier.Data());

			parameters._ID = identifier_buffer;
			parameters._DefaultWidth = 0;
			parameters._DefaultHeight = 0;
			parameters._File1 = temporary_data._AlbedoTextureFilePath.Data();
			parameters._File2 = nullptr;
			parameters._File3 = nullptr;
			parameters._File4 = nullptr;
			parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f);
			parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
			parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
			parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
			parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::ALPHA);
			parameters._ApplyGammaCorrection = true;
			parameters._TransformFunction = nullptr;
			parameters._BaseMipmapLevel = temporary_data._BaseMipmapLevel;
			parameters._MipmapLevels = temporary_data._NumberOfMipmapLevels;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);

			//Now load the texture 2D.
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Textures\\%s\\%s_AlbedoThickness_Texture2D.cr", temporary_data._Identifier.Data(), temporary_data._Identifier.Data());
			ResourceSystem::Instance->LoadResource(output_file_path_buffer);
		}

		//Create the normal map/displacement texture 2D.
		{
			Texture2DBuildParameters parameters;

			char output_file_path_buffer[128];
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Textures\\%s\\%s_NormalMapDisplacement_Texture2D", temporary_data._Identifier.Data(), temporary_data._Identifier.Data());
			parameters._Output = output_file_path_buffer;

			char identifier_buffer[128];
			sprintf_s(identifier_buffer, "%s_NormalMapDisplacement_Texture2D", temporary_data._Identifier.Data());

			parameters._ID = identifier_buffer;
			parameters._DefaultWidth = 0;
			parameters._DefaultHeight = 0;
			parameters._File1 = temporary_data._NormalMapTextureFilePath.Data();
			parameters._File2 = nullptr;
			parameters._File3 = nullptr;
			parameters._File4 = temporary_data._DisplacementTextureFilePath.Data();
			parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.5f);
			parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
			parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
			parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
			parameters._ChannelMappings[3] = temporary_data._DisplacementTextureFilePath ? Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_4, Texture2DBuildParameters::Channel::RED) : Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::ALPHA);
			parameters._ApplyGammaCorrection = false;
			parameters._TransformFunction = nullptr;
			parameters._BaseMipmapLevel = temporary_data._BaseMipmapLevel;
			parameters._MipmapLevels = temporary_data._NumberOfMipmapLevels;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);

			//Now load the texture 2D.
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Textures\\%s\\%s_NormalMapDisplacement_Texture2D.cr", temporary_data._Identifier.Data(), temporary_data._Identifier.Data());
			ResourceSystem::Instance->LoadResource(output_file_path_buffer);
		}

		//Create the material properties texture 2D.
		{
			Texture2DBuildParameters parameters;

			char output_file_path_buffer[128];
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Textures\\%s\\%s_MaterialProperties_Texture2D", temporary_data._Identifier.Data(), temporary_data._Identifier.Data());
			parameters._Output = output_file_path_buffer;

			char identifier_buffer[128];
			sprintf_s(identifier_buffer, "%s_MaterialProperties_Texture2D", temporary_data._Identifier.Data());

			parameters._ID = identifier_buffer;
			parameters._DefaultWidth = 0;
			parameters._DefaultHeight = 0;
			parameters._File1 = temporary_data._RoughnessTextureFilePath.Data();
			parameters._File2 = temporary_data._MetallicTextureFilePath.Data();
			parameters._File3 = temporary_data._AmbientOcclusionTextureFilePath.Data();
			parameters._File4 = nullptr;
			parameters._Default = Vector4<float32>(1.0f, 0.0f, 1.0f, 0.0f);
			parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
			parameters._ChannelMappings[1] = temporary_data._MetallicTextureFilePath.Data() ? Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_2, Texture2DBuildParameters::Channel::RED) : Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::GREEN);
			parameters._ChannelMappings[2] =  temporary_data._AmbientOcclusionTextureFilePath ? Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_3, Texture2DBuildParameters::Channel::RED) : Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::BLUE);
			parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::RED);
			parameters._ApplyGammaCorrection = false;
			parameters._TransformFunction = nullptr;
			parameters._BaseMipmapLevel = temporary_data._BaseMipmapLevel;
			parameters._MipmapLevels = temporary_data._NumberOfMipmapLevels;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);

			//Now load the texture 2D.
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Textures\\%s\\%s_MaterialProperties_Texture2D.cr", temporary_data._Identifier.Data(), temporary_data._Identifier.Data());
			ResourceSystem::Instance->LoadResource(output_file_path_buffer);
		}

		//Build the material.
		{
			MaterialBuildParameters parameters;

			char output_file_path_buffer[128];
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Materials\\%s_Material", temporary_data._Identifier.Data());
			parameters._Output = output_file_path_buffer;

			char identifier_buffer[128];
			sprintf_s(identifier_buffer, "%s_Material", temporary_data._Identifier.Data());

			parameters._ID = identifier_buffer;
			parameters._Type = MaterialResource::Type::OPAQUE;
			parameters._AlbedoThicknessComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;

			char albedo_thickness_buffer[128];
			sprintf_s(albedo_thickness_buffer, "%s_AlbedoThickness_Texture2D", temporary_data._Identifier.Data());

			parameters._AlbedoThicknessComponent._TextureResourceIdentifier = HashString(albedo_thickness_buffer);
			parameters._NormalMapDisplacementComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;

			char normal_map_displcamenet_buffer[128];
			sprintf_s(normal_map_displcamenet_buffer, "%s_NormalMapDisplacement_Texture2D", temporary_data._Identifier.Data());

			parameters._NormalMapDisplacementComponent._TextureResourceIdentifier = HashString(normal_map_displcamenet_buffer);
			parameters._MaterialPropertiesComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;

			char material_properties_buffer[128];
			sprintf_s(material_properties_buffer, "%s_MaterialProperties_Texture2D", temporary_data._Identifier.Data());

			parameters._MaterialPropertiesComponent._TextureResourceIdentifier = HashString(material_properties_buffer);
			parameters._OpacityComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._OpacityComponent._Color = Color(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
			parameters._EmissiveMultiplier = 0.0f;
			parameters._DoubleSided = false;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildMaterial(parameters);

			//Now load the material.
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Materials\\%s_Material.cr", temporary_data._Identifier.Data());
			ResourceSystem::Instance->LoadResource(output_file_path_buffer);
		}

		//No longer creating a resource.
		_CurrentCreateResourceMode = CreateResourceMode::NONE;
	}

	ImGui::End();
}

/*
*	Adds the create Quixel model resource window.
*/
void EditorResourcesSystem::AddCreateQuixelModelResourceWindow() NOEXCEPT
{
	/*
	*	Create Quixel model resource temporary data class definition.
	*/
	class CreateQuixelModelResourceTemporaryData final
	{

	public:

		//The identifier.
		DynamicString _Identifier;

		//The base mipmap level.
		uint8 _BaseMipmapLevel;

		//The number of mipmap levels.
		uint8 _NumberOfMipmapLevels;

		//The albedo texture file path.
		DynamicString _AlbedoTextureFilePath;

		//The normal map texture file path.
		DynamicString _NormalMapTextureFilePath;

		//The displacement texture file path.
		DynamicString _DisplacementTextureFilePath;

		//The roughness texture file path.
		DynamicString _RoughnessTextureFilePath;

		//The metallic texture file path.
		DynamicString _MetallicTextureFilePath;

		//The ambient occlusion texture file path.
		DynamicString _AmbientOcclusionTextureFilePath;

		//The level of detail file paths.
		DynamicArray<DynamicString> _LevelOfDetailFilePaths;

	};

	//Add the "Create Quixel Model Resource" window.
	ImGui::Begin("Create Quixel Model Resource", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowPos(ImVec2(256.0f, 256.0f));
	ImGui::SetWindowSize(ImVec2(512.0f, 512.0f));

	//If the user has already selected a directory path, display it.
	if (_CreateQuixelModelResourceData._DirectoryPath.Data())
	{
		ImGui::Text("Directory Path:");
		ImGui::Text(_CreateQuixelModelResourceData._DirectoryPath.Data());
	}

	//Add the button to set the directory path.
	if (ImGui::Button("Select Directory Path"))
	{
		File::BrowseForFolder(&_CreateQuixelModelResourceData._DirectoryPath);
	}

	//Add some padding before the "Create Quixel Model Resource" button.
	ImGui::Text("");

	//Add the create button.
	if (ImGui::Button("Create Quixel Model Resource"))
	{
		//Browse through all files in the directory path and fill in the temporary data.
		CreateQuixelModelResourceTemporaryData temporary_data;

		//Extract the identifier.
		for (int64 i{ static_cast<int64>(_CreateQuixelModelResourceData._DirectoryPath.Length()) - 1 }; i >= 0; --i)
		{
			if (_CreateQuixelModelResourceData._DirectoryPath[i] == '\\')
			{
				temporary_data._Identifier = &_CreateQuixelModelResourceData._DirectoryPath[i + 1];

				break;
			}
		}

		//Iterate over all the files in the directory and load them.
		for (const auto &entry : std::filesystem::directory_iterator(std::string(_CreateQuixelModelResourceData._DirectoryPath.Data()) + "\\Raw"))
		{
			ASSERT(!entry.is_directory(), "EditorResourcesSystem::AddCreateQuixelModelResourceWindow() failure!");

			//Cache the file path.
			const DynamicString entry_file_path{ entry.path().generic_u8string().c_str() };

			//Cache the file extension.
			const File::Extension file_extension{ File::GetExtension(entry_file_path.Data()) };

			//Is this a texture?
			if (file_extension == File::Extension::JPG)
			{
				//Is this the albedo texture?
				if (entry_file_path.Find("_Albedo."))
				{
					temporary_data._AlbedoTextureFilePath = entry_file_path;

					//Determine the base/number of mipmap level(s).
					if (entry_file_path.Find("_8K_"))
					{
						temporary_data._BaseMipmapLevel = 3;
						temporary_data._NumberOfMipmapLevels = 10;
					}

					else
					{
						temporary_data._BaseMipmapLevel = 2;
						temporary_data._NumberOfMipmapLevels = 9;
					}
				}

				//Is this the normnal map texture?
				if (entry_file_path.Find("_Normal_"))
				{
					temporary_data._NormalMapTextureFilePath = entry_file_path;
				}

				//Is this the displacement texture?
				if (entry_file_path.Find("_Displacement."))
				{
					temporary_data._DisplacementTextureFilePath = entry_file_path;
				}

				//Is this the roughness texture?
				if (entry_file_path.Find("_Roughness."))
				{
					temporary_data._RoughnessTextureFilePath = entry_file_path;
				}

				//Is this the metallic texture?
				if (entry_file_path.Find("_Metalness."))
				{
					temporary_data._MetallicTextureFilePath = entry_file_path;
				}

				//Is this the ambient occlusion texture?
				if (entry_file_path.Find("_AO."))
				{
					temporary_data._AmbientOcclusionTextureFilePath = entry_file_path;
				}
			}

			//Is this a model?
			if (file_extension == File::Extension::FBX)
			{
				temporary_data._LevelOfDetailFilePaths.Emplace(entry_file_path);
			}
		}

		ASSERT(!temporary_data._LevelOfDetailFilePaths.Empty(), "No level of detail file paths found!");

		//Sort the level of detail file paths.
		SortingAlgorithms::InsertionSort<DynamicString>(
		temporary_data._LevelOfDetailFilePaths.Begin(),
		temporary_data._LevelOfDetailFilePaths.End(),
		nullptr,
		[](const void *const RESTRICT, const DynamicString *const RESTRICT first, const DynamicString *const RESTRICT second)
		{
			const char* const RESTRICT yes{ first->Find("_LOD") + 5 };
			const uint8 first_level_of_detail{ static_cast<uint8>(std::stoi(first->Find("_LOD") + 5)) };
			const uint8 second_level_of_detail{ static_cast<uint8>(std::stoi(second->Find("_LOD") + 5)) };

			return first_level_of_detail < second_level_of_detail;
		});

		//Create all the directories, if needed.
		{
			char buffer[128];

			sprintf_s(buffer, "..\\..\\..\\Resources\\Intermediate\\Textures");
			File::CreateDirectory(buffer);

			sprintf_s(buffer, "..\\..\\..\\Resources\\Intermediate\\Textures\\%s", temporary_data._Identifier.Data());
			File::CreateDirectory(buffer);

			sprintf_s(buffer, "..\\..\\..\\Resources\\Intermediate\\Materials");
			File::CreateDirectory(buffer);

			sprintf_s(buffer, "..\\..\\..\\Resources\\Intermediate\\Models");
			File::CreateDirectory(buffer);
		}

		//Create the albedo/thickness texture 2D.
		{
			Texture2DBuildParameters parameters;

			char output_file_path_buffer[128];
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Textures\\%s\\%s_AlbedoThickness_Texture2D", temporary_data._Identifier.Data(), temporary_data._Identifier.Data());
			parameters._Output = output_file_path_buffer;

			char identifier_buffer[128];
			sprintf_s(identifier_buffer, "%s_AlbedoThickness_Texture2D", temporary_data._Identifier.Data());

			parameters._ID = identifier_buffer;
			parameters._DefaultWidth = 0;
			parameters._DefaultHeight = 0;
			parameters._File1 = temporary_data._AlbedoTextureFilePath.Data();
			parameters._File2 = nullptr;
			parameters._File3 = nullptr;
			parameters._File4 = nullptr;
			parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f);
			parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
			parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
			parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
			parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::ALPHA);
			parameters._ApplyGammaCorrection = true;
			parameters._TransformFunction = nullptr;
			parameters._BaseMipmapLevel = temporary_data._BaseMipmapLevel;
			parameters._MipmapLevels = temporary_data._NumberOfMipmapLevels;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);

			//Now load the texture 2D.
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Textures\\%s\\%s_AlbedoThickness_Texture2D.cr", temporary_data._Identifier.Data(), temporary_data._Identifier.Data());
			ResourceSystem::Instance->LoadResource(output_file_path_buffer);
		}

		//Create the normal map/displacement texture 2D.
		{
			Texture2DBuildParameters parameters;

			char output_file_path_buffer[128];
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Textures\\%s\\%s_NormalMapDisplacement_Texture2D", temporary_data._Identifier.Data(), temporary_data._Identifier.Data());
			parameters._Output = output_file_path_buffer;

			char identifier_buffer[128];
			sprintf_s(identifier_buffer, "%s_NormalMapDisplacement_Texture2D", temporary_data._Identifier.Data());

			parameters._ID = identifier_buffer;
			parameters._DefaultWidth = 0;
			parameters._DefaultHeight = 0;
			parameters._File1 = temporary_data._NormalMapTextureFilePath.Data();
			parameters._File2 = nullptr;
			parameters._File3 = nullptr;
			parameters._File4 = temporary_data._DisplacementTextureFilePath.Data();
			parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.5f);
			parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
			parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
			parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
			parameters._ChannelMappings[3] = temporary_data._DisplacementTextureFilePath ? Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_4, Texture2DBuildParameters::Channel::RED) : Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::ALPHA);
			parameters._ApplyGammaCorrection = false;
			parameters._TransformFunction = nullptr;
			parameters._BaseMipmapLevel = temporary_data._BaseMipmapLevel;
			parameters._MipmapLevels = temporary_data._NumberOfMipmapLevels;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);

			//Now load the texture 2D.
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Textures\\%s\\%s_NormalMapDisplacement_Texture2D.cr", temporary_data._Identifier.Data(), temporary_data._Identifier.Data());
			ResourceSystem::Instance->LoadResource(output_file_path_buffer);
		}

		//Create the material properties texture 2D.
		{
			Texture2DBuildParameters parameters;

			char output_file_path_buffer[128];
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Textures\\%s\\%s_MaterialProperties_Texture2D", temporary_data._Identifier.Data(), temporary_data._Identifier.Data());
			parameters._Output = output_file_path_buffer;

			char identifier_buffer[128];
			sprintf_s(identifier_buffer, "%s_MaterialProperties_Texture2D", temporary_data._Identifier.Data());

			parameters._ID = identifier_buffer;
			parameters._DefaultWidth = 0;
			parameters._DefaultHeight = 0;
			parameters._File1 = temporary_data._RoughnessTextureFilePath.Data();
			parameters._File2 = temporary_data._MetallicTextureFilePath.Data();
			parameters._File3 = temporary_data._AmbientOcclusionTextureFilePath.Data();
			parameters._File4 = nullptr;
			parameters._Default = Vector4<float32>(1.0f, 0.0f, 1.0f, 0.0f);
			parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
			parameters._ChannelMappings[1] = temporary_data._MetallicTextureFilePath.Data() ? Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_2, Texture2DBuildParameters::Channel::RED) : Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::GREEN);
			parameters._ChannelMappings[2] =  temporary_data._AmbientOcclusionTextureFilePath ? Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_3, Texture2DBuildParameters::Channel::RED) : Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::BLUE);
			parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::RED);
			parameters._ApplyGammaCorrection = false;
			parameters._TransformFunction = nullptr;
			parameters._BaseMipmapLevel = temporary_data._BaseMipmapLevel;
			parameters._MipmapLevels = temporary_data._NumberOfMipmapLevels;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);

			//Now load the texture 2D.
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Textures\\%s\\%s_MaterialProperties_Texture2D.cr", temporary_data._Identifier.Data(), temporary_data._Identifier.Data());
			ResourceSystem::Instance->LoadResource(output_file_path_buffer);
		}

		//Build the material.
		{
			MaterialBuildParameters parameters;

			char output_file_path_buffer[128];
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Materials\\%s_Material", temporary_data._Identifier.Data());
			parameters._Output = output_file_path_buffer;

			char identifier_buffer[128];
			sprintf_s(identifier_buffer, "%s_Material", temporary_data._Identifier.Data());

			parameters._ID = identifier_buffer;
			parameters._Type = MaterialResource::Type::OPAQUE;
			parameters._AlbedoThicknessComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;

			char albedo_thickness_buffer[128];
			sprintf_s(albedo_thickness_buffer, "%s_AlbedoThickness_Texture2D", temporary_data._Identifier.Data());

			parameters._AlbedoThicknessComponent._TextureResourceIdentifier = HashString(albedo_thickness_buffer);
			parameters._NormalMapDisplacementComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;

			char normal_map_displcamenet_buffer[128];
			sprintf_s(normal_map_displcamenet_buffer, "%s_NormalMapDisplacement_Texture2D", temporary_data._Identifier.Data());

			parameters._NormalMapDisplacementComponent._TextureResourceIdentifier = HashString(normal_map_displcamenet_buffer);
			parameters._MaterialPropertiesComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;

			char material_properties_buffer[128];
			sprintf_s(material_properties_buffer, "%s_MaterialProperties_Texture2D", temporary_data._Identifier.Data());

			parameters._MaterialPropertiesComponent._TextureResourceIdentifier = HashString(material_properties_buffer);
			parameters._OpacityComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._OpacityComponent._Color = Color(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
			parameters._EmissiveMultiplier = 0.0f;
			parameters._DoubleSided = false;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildMaterial(parameters);

			//Now load the material.
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Materials\\%s_Material.cr", temporary_data._Identifier.Data());
			ResourceSystem::Instance->LoadResource(output_file_path_buffer);
		}

		//Build the model.
		{
			ModelBuildParameters parameters;

			char output_file_path_buffer[128];
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Models\\%s_Model", temporary_data._Identifier.Data());
			parameters._Output = output_file_path_buffer;

			char identifier_buffer[128];
			sprintf_s(identifier_buffer, "%s_Model", temporary_data._Identifier.Data());

			parameters._ResourceIdentifier = identifier_buffer;

			for (const DynamicString &level_of_detail_file_path : temporary_data._LevelOfDetailFilePaths)
			{
				parameters._LevelOfDetails.Emplace(level_of_detail_file_path.Data());
			}

			parameters._Transformation = MatrixConstants::IDENTITY;
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildModel(parameters);

			//Now load the model.
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Models\\%s_Model.cr", temporary_data._Identifier.Data());
			ResourceSystem::Instance->LoadResource(output_file_path_buffer);
		}

		//No longer creating a resource.
		_CurrentCreateResourceMode = CreateResourceMode::NONE;
	}

	ImGui::End();
}

/*
*	Adds the create level resource from .obj and .mtl files window.
*/
void EditorResourcesSystem::AddCreateLevelResourceFromObjAndMtlFilesWindow() NOEXCEPT
{
	/*
	*	Create level resource from .obj and .mtl files temporary data class definition.
	*/
	class CreateLevelResourceFomrObjAndMtlTemporaryData final
	{

	public:

		//The .obj file path..
		DynamicString _ObjFilePath;

		//The .mtl file path..
		DynamicString _MtlFilePath;

	};

	//Add the "Create Level From .obj And .mtl Files Resource" window.
	ImGui::Begin("Create Level From .obj And .mtl Files Resource", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowPos(ImVec2(256.0f, 256.0f));
	ImGui::SetWindowSize(ImVec2(512.0f, 512.0f));

	//If the user has already selected a directory path, display it.
	if (_CreateLevelResourceFromObjAndMtlData._DirectoryPath.Data())
	{
		ImGui::Text("Directory Path:");
		ImGui::Text(_CreateLevelResourceFromObjAndMtlData._DirectoryPath.Data());
	}

	//Add the button to set the directory path.
	if (ImGui::Button("Select Directory Path"))
	{
		File::BrowseForFolder(&_CreateLevelResourceFromObjAndMtlData._DirectoryPath);
	}

	//Add some padding before the "Create Level Resource" button.
	ImGui::Text("");

	//Add the button to create the level resource.
	if (ImGui::Button("Create Level Resource"))
	{
		//Fill in the temporary data.
		CreateLevelResourceFomrObjAndMtlTemporaryData temporary_data;

		//Iterate over all the files in the directory and process them.
		for (const auto &entry : std::filesystem::directory_iterator(std::string(_CreateLevelResourceFromObjAndMtlData._DirectoryPath.Data())))
		{
			//Cache the file path.
			const DynamicString entry_file_path{ entry.path().generic_u8string().c_str() };

			//Cache the file extension.
			const File::Extension file_extension{ File::GetExtension(entry_file_path.Data()) };

			//Is this a .jpg file?
			if (file_extension == File::Extension::JPG)
			{
				//Extract the identifier.
				DynamicString identifier;

				for (int64 i{ static_cast<int64>(entry_file_path.Length()) - 1 }; i >= 0; --i)
				{
					if (entry_file_path[i] == '\\' || entry_file_path[i] == '/')
					{
						identifier = &entry_file_path.Data()[i + 1];
						identifier[identifier.Length() - 4] = '\0';

						break;
					}
				}

				//Build the texture 2D.
				{
					Texture2DBuildParameters parameters;

					char output_file_path_buffer[128];
					sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Textures\\%s_Texture2D", identifier.Data());
					parameters._Output = output_file_path_buffer;

					char identifier_buffer[128];
					sprintf_s(identifier_buffer, "%s_Texture2D", identifier.Data());

					parameters._ID = identifier_buffer;
					parameters._DefaultWidth = 0;
					parameters._DefaultHeight = 0;
					parameters._File1 = entry_file_path.Data();
					parameters._File2 = nullptr;
					parameters._File3 = nullptr;
					parameters._File4 = nullptr;
					parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.5f);
					parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
					parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
					parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
					parameters._ChannelMappings[3] =  Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
					parameters._ApplyGammaCorrection = false;
					parameters._TransformFunction = nullptr;
					parameters._BaseMipmapLevel = 0;
					parameters._MipmapLevels = 1;

					ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);

					//Now load the texture 2D.
					sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\Textures\\%s_Texture2D.cr", identifier.Data());
					ResourceSystem::Instance->LoadResource(output_file_path_buffer);
				}
			}

			//Is this a .mtl file?
			if (file_extension == File::Extension::MTL)
			{
				//Remember the file path for later.
				temporary_data._MtlFilePath = entry_file_path;
			}

			//Is this a .obj file?
			if (file_extension == File::Extension::OBJ)
			{
				//Remember the file path for later.
				temporary_data._ObjFilePath = entry_file_path;
			}
		}
	}

	ImGui::End();
}
#endif