//Header file.
#include <Resources/Creation/ResourceCreator.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Creates a font.
*/
void ResourceCreator::CreateFont(FontData *const RESTRICT data, Font *const RESTRICT font) NOEXCEPT
{
	//Just copy the character descriptions.
	font->_CharacterDescriptions = data->_CharacterDescriptions;

	//Create all textures for each character description.
	for (int8 i{ 0 }; i < INT8_MAXIMUM; ++i)
	{
		//It's valid for some characters to be missing (the font might be missing them), so just skip them. (:
		if (data->_CharacterDimensions[i]._X == 0
			|| data->_CharacterDimensions[i]._Y == 0)
		{
			continue;
		}

		//Create the texture.
		Texture2DHandle texture;
		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(data->_TextureData[i]), TextureFormat::R8_Byte), &texture);

		//Add the texture to the global render data.
		font->_CharacterDescriptions[i]._TextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(texture);
	}
}

/*
*	Creates a model.
*/
void ResourceCreator::CreateModel(ModelData *const RESTRICT data, Model *const RESTRICT model) NOEXCEPT
{
	//Copy the model space axis aligned bounding box.
	model->_ModelSpaceAxisAlignedBoundingBox = std::move(data->_AxisAlignedBoundingBox);

	//Create the buffers.
	{
		const void *const RESTRICT dataChunks[]{ data->_Vertices.Data() };
		const uint64 dataSizes[]{ sizeof(Vertex) * data->_Vertices.Size() };
		RenderingSystem::Instance->CreateBuffer(dataSizes[0], BufferUsage::StorageBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &model->_VertexBuffer);
		RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &model->_VertexBuffer);
	}

	{
		const void *const RESTRICT dataChunks[]{ data->_Indices.Data() };
		const uint64 dataSizes[]{ sizeof(uint32) * data->_Indices.Size() };
		RenderingSystem::Instance->CreateBuffer(dataSizes[0], BufferUsage::IndexBuffer | BufferUsage::StorageBuffer, MemoryProperty::DeviceLocal, &model->_IndexBuffer);
		RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &model->_IndexBuffer);
	}

	//Write the index count.
	model->_IndexCount = static_cast<uint32>(data->_Indices.Size());

	//Create the bottom level acceleration structure.
	RenderingSystem::Instance->CreateBottomLevelAccelerationStructure(	model->_VertexBuffer,
																		static_cast<uint32>(data->_Vertices.Size()),
																		model->_IndexBuffer,
																		static_cast<uint32>(data->_Indices.Size()),
																		&model->_BottomLevelAccelerationStructure);
}

/*
*	Creates a texture cube.
*/
void ResourceCreator::CreateTextureCube(TextureCubeData *const RESTRICT data, TextureCubeHandle *const RESTRICT texture) NOEXCEPT
{
	//Create the texture cube.
	RenderingSystem::Instance->CreateTextureCube(*data, texture);
}

/*
*	Creates a texture 2D.
*/
void ResourceCreator::CreateTexture2D(Texture2DData *const RESTRICT data, GlobalTexture2D *const RESTRICT texture) NOEXCEPT
{
	//Create the texture.
	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(data->_Data, data->_Width, data->_Height, 4), TextureFormat::R8G8B8A8_Byte), &texture->_Texture2D);

	//Add the texture to the global render data.
	texture->_Index = RenderingSystem::Instance->AddTextureToGlobalRenderData(texture->_Texture2D);
}