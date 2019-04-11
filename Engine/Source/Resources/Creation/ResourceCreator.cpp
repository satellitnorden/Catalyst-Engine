//Header file.
#include <Resources/Creation/ResourceCreator.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Creates a model.
*/
void ResourceCreator::CreateModel(ModelData *const RESTRICT data, Model *const RESTRICT model) NOEXCEPT
{
	//Just... Copy. (:
	model->_AxisAlignedBoundingBox = std::move(data->_AxisAlignedBoundingBox);
	model->_Vertices = std::move(data->_Vertices);
	model->_Indices = std::move(data->_Indices);

	//Create the buffers.
	{
		const void *const RESTRICT dataChunks[]{ model->_Vertices.Data() };
		const uint64 dataSizes[]{ sizeof(Vertex) * model->_Vertices.Size() };
		RenderingSystem::Instance->CreateBuffer(dataSizes[0], BufferUsage::StorageBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &model->_VertexBuffer);
		RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &model->_VertexBuffer);
	}

	{
		const void *const RESTRICT dataChunks[]{ model->_Indices.Data() };
		const uint64 dataSizes[]{ sizeof(uint32) * model->_Indices.Size() };
		RenderingSystem::Instance->CreateBuffer(dataSizes[0], BufferUsage::IndexBuffer | BufferUsage::StorageBuffer, MemoryProperty::DeviceLocal, &model->_IndexBuffer);
		RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &model->_IndexBuffer);
	}

	//Create the acceleration structure.
	RenderingSystem::Instance->CreateBottomLevelAccelerationStructure(	model->_VertexBuffer,
																		static_cast<uint32>(model->_Vertices.Size()),
																		model->_IndexBuffer,
																		static_cast<uint32>(model->_Indices.Size()),
																		&model->_AccelerationStructure);
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