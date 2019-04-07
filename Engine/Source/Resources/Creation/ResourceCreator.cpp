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

	//Create the buffer.
	const void *const RESTRICT dataChunks[]{ &model->_AxisAlignedBoundingBox, model->_Vertices.Data(), model->_Indices.Data() };
	const uint64 dataSizes[]{ sizeof(AxisAlignedBoundingBox), sizeof(Vertex) * model->_Vertices.Size(), sizeof(uint32) * model->_Indices.Size() };
	RenderingSystem::Instance->CreateBuffer(dataSizes[0] + dataSizes[1] + dataSizes[2], BufferUsage::IndexBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &model->_Buffer);
	RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 3, &model->_Buffer);

	//Create the acceleration structure.
	RenderingSystem::Instance->CreateBottomLevelAccelerationStructure(	model->_Buffer,
																		static_cast<uint32>(dataSizes[0]),
																		static_cast<uint32>(model->_Vertices.Size()),
																		static_cast<uint32>(dataSizes[0] + dataSizes[1]),
																		static_cast<uint32>(model->_Indices.Size()),
																		&model->_AccelerationStructure);
}

/*
*	Creates a texture cube.
*/
void ResourceCreator::CreateTextureCube(TextureCubeData *const RESTRICT data, TextureCube *const RESTRICT texture) NOEXCEPT
{
	//Initialize the texture.
	texture->Initialize(data->_Resolution, reinterpret_cast<const Vector4<float> *const RESTRICT>(data->_Data.Data()));
}

/*
*	Creates a texture 2D.
*/
void ResourceCreator::CreateTexture2D(Texture2DData *const RESTRICT data, Texture2D<Vector4<byte>> *const RESTRICT texture) NOEXCEPT
{
	//Do something. Please. ):
	texture->Initialize(data->_Width, data->_Height, reinterpret_cast<const Vector4<byte> *const RESTRICT>(data->_Data[0].Data()));
}