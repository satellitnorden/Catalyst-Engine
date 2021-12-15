#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorResourcesSystem.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>

//Editor.
#include <Editor/EditorCore.h>
#include <Editor/EditorUtilities.h>

//File.
#include <File/Core/FileCore.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/ResourceSystem.h>

//Third party.
#include <ThirdParty/imgui.h>
#include <ThirdParty/tiny_gltf.h>

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
	ImGui::Begin("Resources", nullptr, EditorConstants::WINDOW_FLAGS);
	EditorUtilities::SetWindowPositionAndSize(WindowAnchor::BOTTOM_LEFT, Vector2<float32>(0.0f, 0.0f), Vector2<float32>(EditorConstants::GENERAL_WINDOW_WIDTH, 0.5f));

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

	//Add the button for creating a level resource from .gltf.
	if (ImGui::Button("Create Level Resource From .gltf"))
	{
		//Reset the data.
		_CreateLevelResourceFromGLTFData.~CreateLevelResourceFromGLTFData();
		new (&_CreateLevelResourceFromGLTFData) CreateLevelResourceFromGLTFData();

		//Set the current create resource mode.
		_CurrentCreateResourceMode = CreateResourceMode::LEVEL_FROM_GLTF;
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

		case CreateResourceMode::LEVEL_FROM_GLTF:
		{
			AddCreateLevelResourceFromGLTFWindow();

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
	ImGui::Begin("Create Material Resource", nullptr, EditorConstants::WINDOW_FLAGS);
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
	ImGui::Begin("Create Model Resource", nullptr, EditorConstants::WINDOW_FLAGS);
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
	ImGui::Begin("Create Texture 2D Resource", nullptr, EditorConstants::WINDOW_FLAGS);
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
	ImGui::Begin("Create Quixel Material Resource", nullptr, EditorConstants::WINDOW_FLAGS);
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
	ImGui::Begin("Create Quixel Model Resource", nullptr, EditorConstants::WINDOW_FLAGS);
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
*	Adds the create level resource from .gltf window.
*/
void EditorResourcesSystem::AddCreateLevelResourceFromGLTFWindow() NOEXCEPT
{
	//Add the "Create Level Resource From .gltf" window.
	ImGui::Begin("Create Level Resource From .gltf", nullptr, EditorConstants::WINDOW_FLAGS);
	ImGui::SetWindowPos(ImVec2(256.0f, 256.0f));
	ImGui::SetWindowSize(ImVec2(512.0f, 512.0f));

	//If the user has already selected a file path, display it.
	if (_CreateLevelResourceFromGLTFData._FilePath.Data())
	{
		ImGui::Text("File Path:");
		ImGui::Text(_CreateLevelResourceFromGLTFData._FilePath.Data());
	}

	//Add the button to set the file path.
	if (ImGui::Button("Select File Path"))
	{
		File::BrowseForFile(false, &_CreateLevelResourceFromGLTFData._FilePath);
	}

	//Add a slider for the scale.
	ImGui::DragFloat("Scale", &_CreateLevelResourceFromGLTFData._Scale, 0.01f);

	//Add some padding before the "Create Level Resource" button.
	ImGui::Text("");

	//Add the button to create the level resource.
	if (ImGui::Button("Create Level Resource"))
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string error;
		std::string warning;

		const bool succeeded{ loader.LoadASCIIFromFile(&model, &error, &warning, _CreateLevelResourceFromGLTFData._FilePath.Data()) };
	
		if (!error.empty())
		{
			PRINT_TO_OUTPUT("Tiny GLTF Error: " << error);
		}

		if (!warning.empty())
		{
			PRINT_TO_OUTPUT("Tiny GLTF Warning: " << warning);
		}

		if (!succeeded)
		{
			ASSERT(false, "Tiny GLTF Failed!");
		}

		//Figure out the base file path.
		DynamicString base_file_path;

		{
			char base_file_path_buffer[MAXIMUM_FILE_PATH_LENGTH];
			sprintf_s(base_file_path_buffer, _CreateLevelResourceFromGLTFData._FilePath.Data());

			for (int64 i{ MAXIMUM_FILE_PATH_LENGTH - 1 }; i >= 0; --i)
			{
				if (base_file_path_buffer[i] == '\\')
				{
					base_file_path_buffer[i + 1] = '\0';

					break;
				}
			}

			base_file_path = base_file_path_buffer;
		}

		//Figure out the base file name.
		DynamicString base_file_name;

		{
			char base_file_name_buffer[MAXIMUM_FILE_PATH_LENGTH];
			sprintf_s(base_file_name_buffer, _CreateLevelResourceFromGLTFData._FilePath.FindLastOfCharacter('\\') + 1);

			for (uint64 i{ 0 }; i < MAXIMUM_FILE_PATH_LENGTH; ++i)
			{
				if (base_file_name_buffer[i] == '.')
				{
					base_file_name_buffer[i] = '\0';

					break;
				}
			}

			base_file_name = base_file_name_buffer;
		}

		//Create all the models.
		{
			//Sort primitives based on the material they have.
			DynamicArray<DynamicArray<tinygltf::Primitive>> primitives;
			primitives.Upsize<true>(model.materials.size());

			for (const tinygltf::Mesh &mesh : model.meshes)
			{
				for (const tinygltf::Primitive &primitive : mesh.primitives)
				{
					primitives[primitive.material].Emplace(primitive);
				}
			}

			//Now create all the models.
			for (uint64 model_index{ 0 }; model_index < primitives.Size(); ++model_index)
			{
				//Determine the output.
				char output_buffer[MAXIMUM_FILE_PATH_LENGTH];
				sprintf_s(output_buffer, "..\\..\\..\\Resources\\Intermediate\\%s_%llu_Model.cr", base_file_name.Data(), model_index);

				//Determine the resoure identifier.
				char resource_identifier_buffer[MAXIMUM_FILE_PATH_LENGTH];
				sprintf_s(resource_identifier_buffer, "%s_%llu_Model", base_file_name.Data(), model_index);

				//Open the output file to be written to.
				BinaryFile<BinaryFileMode::OUT> output_file{ output_buffer };

				//Write the resource header to the file.
				const ResourceHeader header{ ResourceConstants::MODEL_TYPE_IDENTIFIER, HashString(resource_identifier_buffer), resource_identifier_buffer };
				output_file.Write(&header, sizeof(ResourceHeader));

				//Start filling in the vertices and indices.
				ModelFile model_file;
				model_file._Meshes.Emplace();

				uint64 current_vertex_offset{ 0 };

				for (const tinygltf::Primitive &primitive : primitives[model_index])
				{
					//Cache some values.
					const bool has_position{ primitive.attributes.find("POSITION") != primitive.attributes.end() };
					const bool has_normal{ primitive.attributes.find("NORMAL") != primitive.attributes.end() };
					const bool has_tangent{ primitive.attributes.find("TANGENT") != primitive.attributes.end() };
					const bool has_texture_coordinate{ primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end() };

					//Do some sanity checking.
					ASSERT(has_position, "POSITION is not in attributes!");
					ASSERT(has_normal, "NORMAL is not in attributes!");
					ASSERT(has_tangent, "TANGENT is not in attributes!");
					ASSERT(has_texture_coordinate, "TEXCOORD_0 is not in attributes!");
					ASSERT(primitive.mode == TINYGLTF_MODE_TRIANGLES, "Mode of primitive is not triangles!");

					//Go through the attributes.
					uint64 vertices_count{ 0 };

					for (const std::pair<std::string, int> &attribute : primitive.attributes)
					{
						if (attribute.first == "POSITION")
						{
							//Cache the accessor.
							const tinygltf::Accessor &accessor{ model.accessors[attribute.second] };

							ASSERT(accessor.bufferView >= 0, "Accessor has no buffer view!");
							ASSERT(accessor.type == TINYGLTF_TYPE_VEC3, "Type of POSITION accessor is not TINYGLTF_TYPE_VEC3!");

							//Cache the buffer view.
							const tinygltf::BufferView &buffer_view{ model.bufferViews[accessor.bufferView] };

							ASSERT(buffer_view.buffer >= 0, "Buffer view has no buffer!");

							//Cache the buffer.
							const tinygltf::Buffer &buffer{ model.buffers[buffer_view.buffer] };

							//Add the positions.
							for (uint64 i{ 0 }; i < accessor.count; ++i)
							{
								//Calculate the byte offset.
								const uint64 byte_offset{ buffer_view.byteOffset + sizeof(Vector3<float32>) * i };

								//Retrieve the position.
								const Vector3<float32> position{ *reinterpret_cast<const Vector3<float32> *const RESTRICT>(&buffer.data[byte_offset]) };

								//Calculate the vertex index.
								const uint64 vertex_index{ current_vertex_offset + i };

								//Retrieve the vertex.
								Vertex *RESTRICT vertex{ nullptr };

								if (vertex_index >= model_file._Meshes[0]._Vertices.Size())
								{
									model_file._Meshes[0]._Vertices.Emplace();
									vertex = &model_file._Meshes[0]._Vertices.Back();
								}

								else
								{
									vertex = &model_file._Meshes[0]._Vertices[vertex_index];
								}

								//Set the position.
								vertex->_Position = position;
							}

							//Set the vertices count.
							if (vertices_count == 0)
							{
								vertices_count = accessor.count;
							}

							else
							{
								ASSERT(vertices_count == accessor.count, "Mismatch!");
							}
						}

						else if (attribute.first == "NORMAL")
						{
							//Cache the accessor.
							const tinygltf::Accessor &accessor{ model.accessors[attribute.second] };

							ASSERT(accessor.bufferView >= 0, "Accessor has no buffer view!");
							ASSERT(accessor.type == TINYGLTF_TYPE_VEC3, "Type of NORMAL accessor is not TINYGLTF_TYPE_VEC3!");

							//Cache the buffer view.
							const tinygltf::BufferView &buffer_view{ model.bufferViews[accessor.bufferView] };

							ASSERT(buffer_view.buffer >= 0, "Buffer view has no buffer!");

							//Cache the buffer.
							const tinygltf::Buffer &buffer{ model.buffers[buffer_view.buffer] };

							//Add the normals.
							for (uint64 i{ 0 }; i < accessor.count; ++i)
							{
								//Calculate the byte offset.
								const uint64 byte_offset{ buffer_view.byteOffset + sizeof(Vector3<float32>) * i };
							
								//Retrieve the normal.
								const Vector3<float32> normal{ *reinterpret_cast<const Vector3<float32> *const RESTRICT>(&buffer.data[byte_offset]) };
							
								//Calculate the vertex index.
								const uint64 vertex_index{ current_vertex_offset + i };

								//Retrieve the vertex.
								Vertex *RESTRICT vertex{ nullptr };

								if (vertex_index >= model_file._Meshes[0]._Vertices.Size())
								{
									model_file._Meshes[0]._Vertices.Emplace();
									vertex = &model_file._Meshes[0]._Vertices.Back();
								}

								else
								{
									vertex = &model_file._Meshes[0]._Vertices[vertex_index];
								}

								//Set the normal.
								vertex->_Normal = normal;
							}

							//Set the vertices count.
							if (vertices_count == 0)
							{
								vertices_count = accessor.count;
							}

							else
							{
								ASSERT(vertices_count == accessor.count, "Mismatch!");
							}
						}

						else if (attribute.first == "TANGENT")
						{
							//Cache the accessor.
							const tinygltf::Accessor &accessor{ model.accessors[attribute.second] };

							ASSERT(accessor.bufferView >= 0, "Accessor has no buffer view!");
							ASSERT(accessor.type == TINYGLTF_TYPE_VEC4, "Type of TANGENT accessor is not TINYGLTF_TYPE_VEC4!");

							//Cache the buffer view.
							const tinygltf::BufferView &buffer_view{ model.bufferViews[accessor.bufferView] };

							ASSERT(buffer_view.buffer >= 0, "Buffer view has no buffer!");

							//Cache the buffer.
							const tinygltf::Buffer &buffer{ model.buffers[buffer_view.buffer] };

							//Add the tangents.
							for (uint64 i{ 0 }; i < accessor.count; ++i)
							{
								//Calculate the byte offset.
								const uint64 byte_offset{ buffer_view.byteOffset + sizeof(Vector4<float32>) * i };

								//Retrieve the tangent.
								const Vector4<float32> tangent{ *reinterpret_cast<const Vector4<float32> *const RESTRICT>(&buffer.data[byte_offset]) };

								//Calculate the vertex index.
								const uint64 vertex_index{ current_vertex_offset + i };

								//Retrieve the vertex.
								Vertex *RESTRICT vertex{ nullptr };

								if (vertex_index >= model_file._Meshes[0]._Vertices.Size())
								{
									model_file._Meshes[0]._Vertices.Emplace();
									vertex = &model_file._Meshes[0]._Vertices.Back();
								}

								else
								{
									vertex = &model_file._Meshes[0]._Vertices[vertex_index];
								}

								//Set the tangent.
								vertex->_Tangent = Vector3<float32>(tangent._X, tangent._Y, tangent._Z);
							}

							//Set the vertices count.
							if (vertices_count == 0)
							{
								vertices_count = accessor.count;
							}

							else
							{
								ASSERT(vertices_count == accessor.count, "Mismatch!");
							}
						}

						else if (attribute.first == "TEXCOORD_0")
						{
							//Cache the accessor.
							const tinygltf::Accessor& accessor{ model.accessors[attribute.second] };

							ASSERT(accessor.bufferView >= 0, "Accessor has no buffer view!");
							ASSERT(accessor.type == TINYGLTF_TYPE_VEC2, "Type of TEXCOORD_0 accessor is not TINYGLTF_TYPE_VEC2!");

							//Cache the buffer view.
							const tinygltf::BufferView& buffer_view{ model.bufferViews[accessor.bufferView] };

							ASSERT(buffer_view.buffer >= 0, "Buffer view has no buffer!");

							//Cache the buffer.
							const tinygltf::Buffer& buffer{ model.buffers[buffer_view.buffer] };

							//Add the texture coordinates.
							for (uint64 i{ 0 }; i < accessor.count; ++i)
							{
								//Calculate the byte offset.
								const uint64 byte_offset{ buffer_view.byteOffset + sizeof(Vector2<float32>) * i };

								//Retrieve the texture coordinate.
								const Vector2<float32> texture_coordinate{ *reinterpret_cast<const Vector2<float32> *const RESTRICT>(&buffer.data[byte_offset]) };

								//Calculate the vertex index.
								const uint64 vertex_index{ current_vertex_offset + i };

								//Retrieve the vertex.
								Vertex* RESTRICT vertex{ nullptr };

								if (vertex_index >= model_file._Meshes[0]._Vertices.Size())
								{
									model_file._Meshes[0]._Vertices.Emplace();
									vertex = &model_file._Meshes[0]._Vertices.Back();
								}

								else
								{
									vertex = &model_file._Meshes[0]._Vertices[vertex_index];
								}

								//Set the texture coordinate.
								vertex->_TextureCoordinate = texture_coordinate;
							}

							//Set the vertices count.
							if (vertices_count == 0)
							{
								vertices_count = accessor.count;
							}

							else
							{
								ASSERT(vertices_count == accessor.count, "Mismatch!");
							}
						}
					}

					//Add the indices.
					{
						//Cache the accessor.
						const tinygltf::Accessor& accessor{ model.accessors[primitive.indices] };

						ASSERT(accessor.bufferView >= 0, "Accessor has no buffer view!");
						ASSERT(accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, "Component Type of INDICES accessor is not TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT!");
						ASSERT(accessor.type == TINYGLTF_TYPE_SCALAR, "Type of INDICES accessor is not TINYGLTF_TYPE_SCALAR!");

						//Cache the buffer view.
						const tinygltf::BufferView& buffer_view{ model.bufferViews[accessor.bufferView] };

						ASSERT(buffer_view.buffer >= 0, "Buffer view has no buffer!");

						//Cache the buffer.
						const tinygltf::Buffer& buffer{ model.buffers[buffer_view.buffer] };

						//Add the texture coordinates.
						for (uint64 i{ 0 }; i < accessor.count; ++i)
						{
							//Calculate the byte offset.
							const uint64 byte_offset{ buffer_view.byteOffset + sizeof(uint16) * i };

							//Retrieve the index
							const uint16 index{ *reinterpret_cast<const uint16* const RESTRICT>(&buffer.data[byte_offset]) };

							//Add the index.
							model_file._Meshes[0]._Indices.Emplace(static_cast<uint32>(current_vertex_offset) + static_cast<uint32>(index));
						}
					}

					//Update the current vertex offset.
					current_vertex_offset += vertices_count;
				}

				//Do some sanity-checking.
				{
					for (const uint32 index : model_file._Meshes[0]._Indices)
					{
						ASSERT(index < model_file._Meshes[0]._Vertices.Size(), "Oh no...");
					}
				}

				//Scale the vertices.
				for (Vertex &vertex : model_file._Meshes[0]._Vertices)
				{
					vertex._Position *= _CreateLevelResourceFromGLTFData._Scale;
				}

				//Post process the model file.
				model_file.PostProcess();

				//Determine the model space axis aligned bounding box.
				{
					//Iterate over all vertices in all meshes and expand the bounding box.
					AxisAlignedBoundingBox3D axis_aligned_bounding_box;

					for (const Vertex &vertex : model_file._Meshes[0]._Vertices)
					{
						axis_aligned_bounding_box.Expand(vertex._Position);
					}

					//Write the axis-aligned bounding box to the file.
					output_file.Write(&axis_aligned_bounding_box, sizeof(AxisAlignedBoundingBox3D));
				}

				//Write the number of meshes.
				constexpr uint64 NUMBER_OF_MESHES{ 1 };
				output_file.Write(&NUMBER_OF_MESHES, sizeof(uint64));

				//Write the number of level of details.
				constexpr uint64 NUMBER_OF_LEVEL_OF_DETAILS{ 1 };
				output_file.Write(&NUMBER_OF_LEVEL_OF_DETAILS, sizeof(uint64));

				//Write the number of vertices to the file.
				const uint64 number_of_vertices{ model_file._Meshes[0]._Vertices.Size() };
				output_file.Write(&number_of_vertices, sizeof(uint64));

				//Write the vertices to the file.
				output_file.Write(model_file._Meshes[0]._Vertices.Data(), sizeof(Vertex) * number_of_vertices);

				//Write the number of indices to the file.
				const uint64 number_of_indices{ model_file._Meshes[0]._Indices.Size() };
				output_file.Write(&number_of_indices, sizeof(uint64));

				//Write the vertices to the file.
				output_file.Write(model_file._Meshes[0]._Indices.Data(), sizeof(uint32) * number_of_indices);

				//Write that there doesn't exist a collision model.
				bool collision_model_exists{ false };
				output_file.Write(&collision_model_exists, sizeof(bool));

				//Close the output file.
				output_file.Close();

				//Was the model file even valid?
				if (!model_file.IsValid())
				{
					File::Delete(output_buffer);
				}

				else
				{
					ResourceSystem::Instance->LoadResource(output_buffer);
				}
			}
		}

		//Create all the materials.
		for (uint64 material_index{ 0 }; material_index < model.materials.size(); ++material_index)
		{
			//Cache the material.
			tinygltf::Material &material{ model.materials[material_index] };

			//Start creating the material.
			MaterialBuildParameters parameters;

			char output_buffer[MAXIMUM_FILE_PATH_LENGTH];
			sprintf_s(output_buffer, "..\\..\\..\\Resources\\Intermediate\\%s_%llu_Material", base_file_name.Data(), material_index);
			parameters._Output = output_buffer;

			char ID_buffer[MAXIMUM_FILE_PATH_LENGTH];
			sprintf_s(ID_buffer, "%s_%llu_Material", base_file_name.Data(), material_index);
			parameters._ID = ID_buffer;

			parameters._Type = MaterialResource::Type::OPAQUE;

			//Does this material have an albedo texture?
			if (material.pbrMetallicRoughness.baseColorTexture.index == -1)
			{
				parameters._AlbedoThicknessComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
				parameters._AlbedoThicknessComponent._Color = Color(Vector4<float32>(static_cast<float32>(material.pbrMetallicRoughness.baseColorFactor[0]), static_cast<float32>(material.pbrMetallicRoughness.baseColorFactor[1]), static_cast<float32>(material.pbrMetallicRoughness.baseColorFactor[2]), 1.0f));
			}

			else
			{
				//Cache the image.
				tinygltf::Image &image{ model.images[material.pbrMetallicRoughness.baseColorTexture.index] };

				//Create the texture.
				char texture_ID_buffer[MAXIMUM_FILE_PATH_LENGTH];
				
				{
					//Build the texture 2D.
					Texture2DBuildParameters texture_parameters;

					char texture_output_buffer[MAXIMUM_FILE_PATH_LENGTH];
					sprintf_s(texture_output_buffer, "..\\..\\..\\Resources\\Intermediate\\%s_%llu_Material_AlbedoThickness_Texture2D", base_file_name.Data(), material_index);
					texture_parameters._Output = texture_output_buffer;

					sprintf_s(texture_ID_buffer, "%s_%llu_Material_AlbedoThickness_Texture2D", base_file_name.Data(), material_index);
					texture_parameters._ID = texture_ID_buffer;

					texture_parameters._DefaultWidth = 0;
					texture_parameters._DefaultHeight = 0;

					char file_1_buffer[MAXIMUM_FILE_PATH_LENGTH];
					sprintf_s(file_1_buffer, "%s%s", base_file_path.Data(), image.uri.c_str());
					texture_parameters._File1 = file_1_buffer;

					texture_parameters._File2 = nullptr;
					texture_parameters._File3 = nullptr;
					texture_parameters._File4 = nullptr;
					texture_parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f);
					texture_parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
					texture_parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
					texture_parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
					texture_parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::ALPHA);
					texture_parameters._ApplyGammaCorrection = true;
					texture_parameters._TransformFunction = nullptr;
					texture_parameters._BaseMipmapLevel = 0;
					texture_parameters._MipmapLevels = 9;

					ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(texture_parameters);
					
					{
						char load_buffer[MAXIMUM_FILE_PATH_LENGTH];
						sprintf_s(load_buffer, "%s.cr", texture_parameters._Output);

						ResourceSystem::Instance->LoadResource(load_buffer);
					}
				}

				//Set the material properties.
				parameters._AlbedoThicknessComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
				parameters._AlbedoThicknessComponent._TextureResourceIdentifier = HashString(texture_ID_buffer);
			}

			//Does this material have a normal map texture?
			if (material.normalTexture.index == -1)
			{
				parameters._NormalMapDisplacementComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
				parameters._NormalMapDisplacementComponent._Color = Color(Vector4<float32>(0.5f, 0.5f, 1.0f, 0.5f));
			}

			else
			{
				//Cache the image.
				tinygltf::Image &image{ model.images[material.normalTexture.index] };

				//Create the texture.
				char texture_ID_buffer[MAXIMUM_FILE_PATH_LENGTH];

				{
					//Build the texture 2D.
					Texture2DBuildParameters texture_parameters;

					char texture_output_buffer[MAXIMUM_FILE_PATH_LENGTH];
					sprintf_s(texture_output_buffer, "..\\..\\..\\Resources\\Intermediate\\%s_%llu_Material_NormalMapDisplacement_Texture2D", base_file_name.Data(), material_index);
					texture_parameters._Output = texture_output_buffer;

					sprintf_s(texture_ID_buffer, "%s_%llu_Material_NormalMapDisplacement_Texture2D", base_file_name.Data(), material_index);
					texture_parameters._ID = texture_ID_buffer;

					texture_parameters._DefaultWidth = 0;
					texture_parameters._DefaultHeight = 0;

					char file_1_buffer[MAXIMUM_FILE_PATH_LENGTH];
					sprintf_s(file_1_buffer, "%s%s", base_file_path.Data(), image.uri.c_str());
					texture_parameters._File1 = file_1_buffer;

					texture_parameters._File2 = nullptr;
					texture_parameters._File3 = nullptr;
					texture_parameters._File4 = nullptr;
					texture_parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.5f);
					texture_parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
					texture_parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
					texture_parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
					texture_parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::ALPHA);
					texture_parameters._ApplyGammaCorrection = false;
					texture_parameters._TransformFunction = [](const Texture2D<Vector4<float32>> &input_texture, Texture2D<Vector4<float32>> *const RESTRICT output_texture)
					{
						for (uint32 Y{ 0 }; Y < input_texture.GetHeight(); ++Y)
						{
							for (uint32 X{ 0 }; X < input_texture.GetWidth(); ++X)
							{
								const Vector4<float32> &input_texel{ input_texture.At(X, Y) };

								if (input_texel._X == 0.0f
									&& input_texel._Y == 0.0f
									&& input_texel._Z == 0.0f)
								{
									output_texture->At(X, Y) = Vector4<float32>(0.5f, 0.5f, 1.0f, 0.5f);
								}

								else
								{
									output_texture->At(X, Y) = input_texel;
								}
							}
						}
					};
					texture_parameters._BaseMipmapLevel = 0;
					texture_parameters._MipmapLevels = 9;

					ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(texture_parameters);

					{
						char load_buffer[MAXIMUM_FILE_PATH_LENGTH];
						sprintf_s(load_buffer, "%s.cr", texture_parameters._Output);

						ResourceSystem::Instance->LoadResource(load_buffer);
					}
				}

				//Set the material properties.
				parameters._NormalMapDisplacementComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
				parameters._NormalMapDisplacementComponent._TextureResourceIdentifier = HashString(texture_ID_buffer);
			}

			//Does this material have a roughness/metallic texture?
			if (material.pbrMetallicRoughness.metallicRoughnessTexture.index == -1)
			{
				parameters._MaterialPropertiesComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
				parameters._MaterialPropertiesComponent._Color = Color(Vector4<float32>(static_cast<float32>(material.pbrMetallicRoughness.roughnessFactor), static_cast<float32>(material.pbrMetallicRoughness.metallicFactor), 1.0f, 0.0f));
			}

			else
			{
				//Cache the image.
				tinygltf::Image& image{ model.images[material.pbrMetallicRoughness.metallicRoughnessTexture.index] };

				//Create the texture.
				char texture_ID_buffer[MAXIMUM_FILE_PATH_LENGTH];

				{
					//Build the texture 2D.
					Texture2DBuildParameters texture_parameters;

					char texture_output_buffer[MAXIMUM_FILE_PATH_LENGTH];
					sprintf_s(texture_output_buffer, "..\\..\\..\\Resources\\Intermediate\\%s_%llu_Material_MaterialProperties_Texture2D", base_file_name.Data(), material_index);
					texture_parameters._Output = texture_output_buffer;

					sprintf_s(texture_ID_buffer, "%s_%llu_Material_MaterialProperties_Texture2D", base_file_name.Data(), material_index);
					texture_parameters._ID = texture_ID_buffer;

					texture_parameters._DefaultWidth = 0;
					texture_parameters._DefaultHeight = 0;

					char file_1_buffer[MAXIMUM_FILE_PATH_LENGTH];
					sprintf_s(file_1_buffer, "%s%s", base_file_path.Data(), image.uri.c_str());
					texture_parameters._File1 = file_1_buffer;

					texture_parameters._File2 = nullptr;
					texture_parameters._File3 = nullptr;
					texture_parameters._File4 = nullptr;
					texture_parameters._Default = Vector4<float32>(1.0f, 0.0f, 1.0f, 0.0f);
					texture_parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
					texture_parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
					texture_parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::BLUE);
					texture_parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::ALPHA);
					texture_parameters._ApplyGammaCorrection = false;
					texture_parameters._TransformFunction = nullptr;
					texture_parameters._BaseMipmapLevel = 0;
					texture_parameters._MipmapLevels = 9;

					ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(texture_parameters);

					{
						char load_buffer[MAXIMUM_FILE_PATH_LENGTH];
						sprintf_s(load_buffer, "%s.cr", texture_parameters._Output);

						ResourceSystem::Instance->LoadResource(load_buffer);
					}
				}

				//Set the material properties.
				parameters._MaterialPropertiesComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
				parameters._MaterialPropertiesComponent._TextureResourceIdentifier = HashString(texture_ID_buffer);
			}

			//TODO.
			parameters._OpacityComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._OpacityComponent._Color = Color(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));

			parameters._EmissiveMultiplier = 0.0f;
			parameters._DoubleSided = material.doubleSided;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildMaterial(parameters);

			{
				char load_buffer[MAXIMUM_FILE_PATH_LENGTH];
				sprintf_s(load_buffer, "%s.cr", parameters._Output);

				ResourceSystem::Instance->LoadResource(load_buffer);
			}
		}

		//Finally, create the level!
		{
			//Build the level.
			LevelBuildParameters parameters;

			char output_file_path_buffer[MAXIMUM_FILE_PATH_LENGTH];
			sprintf_s(output_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\%s_Level", base_file_name.Data());
			parameters._OutputFilePath = output_file_path_buffer;

			char identifier_buffer[MAXIMUM_FILE_PATH_LENGTH];
			sprintf_s(identifier_buffer, "%s_Level", base_file_name.Data());
			parameters._Identifier = identifier_buffer;

			for (uint64 model_index{ 0 }; model_index < model.materials.size(); ++model_index)
			{
				//First of all, does this model exist?
				char model_file_path_buffer[MAXIMUM_FILE_PATH_LENGTH];
				sprintf_s(model_file_path_buffer, "..\\..\\..\\Resources\\Intermediate\\%s_%llu_Model.cr", base_file_name.Data(), model_index);

				if (!File::Exists(model_file_path_buffer))
				{
					continue;
				}

				parameters._LevelEntries.Emplace();
				LevelEntry &level_entry{ parameters._LevelEntries.Back() };

				level_entry._Type = LevelEntry::Type::STATIC_MODEL;
				level_entry._Version = LevelEntry::StaticModelData::CURRENT_VERSION;

				{
					level_entry._StaticModelData._WorldTransform = WorldTransform();

					char model_resource_identifier_buffer[MAXIMUM_FILE_PATH_LENGTH];
					sprintf_s(model_resource_identifier_buffer, "%s_%llu_Model", base_file_name.Data(), model_index);
					level_entry._StaticModelData._ModelResourceIdentifier = HashString(model_resource_identifier_buffer);

					char material_resource_identifier_buffer[MAXIMUM_FILE_PATH_LENGTH];
					sprintf_s(material_resource_identifier_buffer, "%s_%llu_Material", base_file_name.Data(), model_index);
					level_entry._StaticModelData._MaterialResourceIdentifiers.Emplace(HashString(material_resource_identifier_buffer));
				
					level_entry._StaticModelData._ModelCollisionConfiguration._Type = ModelCollisionType::NONE;
				}
			}

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildLevel(parameters);

			{
				char load_buffer[MAXIMUM_FILE_PATH_LENGTH];
				sprintf_s(load_buffer, "%s.cr", parameters._OutputFilePath);

				ResourceSystem::Instance->LoadResource(load_buffer);
			}
		}
	}

	ImGui::End();
}
#endif