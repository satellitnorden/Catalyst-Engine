//Header file.
#include <Resources/Loading/ResourceLoadingSystem.h>

//Animation.
#include <Animation/AnimatedVertex.h>

//File.
#include <File/Core/BinaryInputFile.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Given a file, load raw data data.
*/
void ResourceLoadingSystem::LoadRawData(BinaryInputFile *const RESTRICT file, RawDataData* const RESTRICT data) NOEXCEPT
{
	//Read the data size.
	uint64 data_size;
	file->Read(&data_size, sizeof(uint64));

	//Reserve the appropriate amount of memory.
	data->_Data.Upsize<false>(data_size);

	//Read the data.
	file->Read(data->_Data.Data(), data_size);
}

/*
*	Given a file, load texture 3D data.
*/
void ResourceLoadingSystem::LoadTexture3D(BinaryInputFile *const RESTRICT file, Texture3DData *const RESTRICT data) NOEXCEPT
{
	//Read the number of mipmap levels.
	file->Read(&data->_MipmapLevels, sizeof(uint8));

	//Read the width.
	file->Read(&data->_Width, sizeof(uint32));

	//Read the height.
	file->Read(&data->_Height, sizeof(uint32));

	//Read the depth.
	file->Read(&data->_Depth, sizeof(uint32));

	//Read the data.
	data->_Data.Upsize<true>(data->_MipmapLevels);

	for (uint8 i{ 0 }; i < data->_MipmapLevels; ++i)
	{
		const uint64 textureSize{ (data->_Width >> i) * (data->_Height >> i) * (data->_Depth >> i) * sizeof(Vector4<byte>) };

		data->_Data[i].Reserve(textureSize);

		file->Read(data->_Data[i].Data(), textureSize);
	}
}