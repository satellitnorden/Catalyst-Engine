//Header file.
#include <Resources/Creation/ResourceCreationSystem.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Creates an animated model.
*/
void ResourceCreationSystem::CreateAnimatedModel(AnimatedModelData *const RESTRICT data, AnimatedModelResource *const RESTRICT resource) NOEXCEPT
{
	//Copy the model space axis aligned bounding box.
	resource->_ModelSpaceAxisAlignedBoundingBox = std::move(data->_AxisAlignedBoundingBox);

	//Create the buffers.
	{
		const void *const RESTRICT dataChunks[]{ data->_Vertices.Data() };
		const uint64 dataSizes[]{ sizeof(AnimatedVertex) * data->_Vertices.Size() };
		RenderingSystem::Instance->CreateBuffer(dataSizes[0], BufferUsage::StorageBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &resource->_VertexBuffer);
		RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &resource->_VertexBuffer);
	}

	{
		const void *const RESTRICT dataChunks[]{ data->_Indices.Data() };
		const uint64 dataSizes[]{ sizeof(uint32) * data->_Indices.Size() };
		RenderingSystem::Instance->CreateBuffer(dataSizes[0], BufferUsage::IndexBuffer | BufferUsage::StorageBuffer, MemoryProperty::DeviceLocal, &resource->_IndexBuffer);
		RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &resource->_IndexBuffer);
	}

	//Write the index count.
	resource->_IndexCount = static_cast<uint32>(data->_Indices.Size());

	//Create the bottom level acceleration structure.
	RenderingSystem::Instance->CreateBottomLevelAccelerationStructure(	resource->_VertexBuffer,
																		static_cast<uint32>(data->_Vertices.Size()),
																		resource->_IndexBuffer,
																		static_cast<uint32>(data->_Indices.Size()),
																		&resource->_BottomLevelAccelerationStructure);

	//Copy the skeleton.
	resource->_Skeleton = data->_Skeleton;
}

/*
*	Creates an animation
*/
void ResourceCreationSystem::CreateAnimation(AnimationData *const RESTRICT data, AnimationResource *const RESTRICT resource) NOEXCEPT
{
	//Just... Copy.
	*resource = data->_Animation;
}

/*
*	Creates a font.
*/
void ResourceCreationSystem::CreateFont(FontData *const RESTRICT data, Font *const RESTRICT font) NOEXCEPT
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
		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(data->_TextureData[i]), TextureFormat::R_UINT8), &texture);

		//Add the texture to the global render data.
		font->_CharacterDescriptions[i]._TextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(texture);
	}
}

/*
*	Creates a model.
*/
void ResourceCreationSystem::CreateModel(ModelData *const RESTRICT data, Model *const RESTRICT model) NOEXCEPT
{
	//Copy the model space axis aligned bounding box.
	model->_ModelSpaceAxisAlignedBoundingBox = data->_AxisAlignedBoundingBox;

	model->_Meshes.Upsize<true>(data->_NumberOfMeshes);

	for (uint64 i{ 0 }; i < data->_NumberOfMeshes; ++i)
	{
		model->_Meshes[i]._VertexBuffers.Upsize<false>(data->_NumberOfLevelfDetails);
		model->_Meshes[i]._IndexBuffers.Upsize<false>(data->_NumberOfLevelfDetails);
		model->_Meshes[i]._IndexCounts.Upsize<false>(data->_NumberOfLevelfDetails);

		for (uint64 j{ 0 }; j < data->_NumberOfLevelfDetails; ++j)
		{
			//Create the buffers.
			{
				const void* const RESTRICT dataChunks[]{ data->_Vertices[i][j].Data() };
				const uint64 dataSizes[]{ sizeof(Vertex) * data->_Vertices[i][j].Size() };
				RenderingSystem::Instance->CreateBuffer(dataSizes[0], BufferUsage::StorageBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &model->_Meshes[i]._VertexBuffers[j]);
				RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &model->_Meshes[i]._VertexBuffers[j]);
			}

			{
				const void* const RESTRICT dataChunks[]{ data->_Indices[i][j].Data() };
				const uint64 dataSizes[]{ sizeof(uint32) * data->_Indices[i][j].Size() };
				RenderingSystem::Instance->CreateBuffer(dataSizes[0], BufferUsage::IndexBuffer | BufferUsage::StorageBuffer, MemoryProperty::DeviceLocal, &model->_Meshes[i]._IndexBuffers[j]);
				RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &model->_Meshes[i]._IndexBuffers[j]);
			}

			//Write the index count.
			model->_Meshes[i]._IndexCounts[j] = static_cast<uint32>(data->_Indices[i][j].Size());
		}

		//Create the bottom level acceleration structure.
		RenderingSystem::Instance->CreateBottomLevelAccelerationStructure(	model->_Meshes[i]._VertexBuffers[0],
																			static_cast<uint32>(data->_Vertices[i][0].Size()),
																			model->_Meshes[i]._IndexBuffers[0],
																			static_cast<uint32>(data->_Indices[i][0].Size()),
																			&model->_Meshes[i]._BottomLevelAccelerationStructure);
	
#if defined(CATALYST_ENABLE_RENDERING_REFERENCE)
		//Copy the vertices and indices.
		model->_Meshes[i]._Vertices = data->_Vertices[i][0];
		model->_Meshes[i]._Indices = data->_Indices[i][0];
#endif
	}
}

/*
*	Creates a shader.
*/
void ResourceCreationSystem::CreateShader(ShaderData *const RESTRICT data, ShaderResource *const RESTRICT resource) NOEXCEPT
{
	//Create the shader.
	RenderingSystem::Instance->CreateShader(ArrayProxy<byte>(data->_Data), data->_Stage, &resource->_Handle);
}

/*
*	Creates a sound.
*/
void ResourceCreationSystem::CreateSound(SoundData *const RESTRICT data, SoundResource *const RESTRICT resource) NOEXCEPT
{
	//Just copy over all data.
	resource->_SampleRate = data->_SampleRate;
	resource->_NumberOfChannels = data->_NumberOfChannels;
	resource->_Samples = std::move(data->_Samples);
}

/*
*	Creates a texture cube.
*/
void ResourceCreationSystem::CreateTextureCube(TextureCubeData *const RESTRICT data, TextureCubeHandle *const RESTRICT texture) NOEXCEPT
{
	//Create the texture cube.
	RenderingSystem::Instance->CreateTextureCube(*data, texture);
}

/*
*	Creates a texture 2D.
*/
void ResourceCreationSystem::CreateTexture2D(Texture2DData *const RESTRICT data, Texture2DResource *const RESTRICT resource) NOEXCEPT
{
	//Create the texture.
	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(data->_Data, data->_Width, data->_Height, 4), TextureFormat::RGBA_UINT8), &resource->_Texture2DHandle);

	//Add the texture to the global render data.
	resource->_Index = RenderingSystem::Instance->AddTextureToGlobalRenderData(resource->_Texture2DHandle);

#if defined(CATALYST_ENABLE_RENDERING_REFERENCE)
	//Create the texture 2D.
	resource->_Texture2D.Initialize(data->_Width, data->_Height);

	for (uint32 Y{ 0 }; Y < data->_Height; ++Y)
	{
		for (uint32 X{ 0 }; X < data->_Width; ++X)
		{
			uint64 source_texture_index{ (X + (Y * data->_Width)) * 4 };

			resource->_Texture2D.At(X, Y)._X = static_cast<float>(data->_Data[0][source_texture_index++]) / 255.0f;
			resource->_Texture2D.At(X, Y)._Y = static_cast<float>(data->_Data[0][source_texture_index++]) / 255.0f;
			resource->_Texture2D.At(X, Y)._Z = static_cast<float>(data->_Data[0][source_texture_index++]) / 255.0f;
			resource->_Texture2D.At(X, Y)._W = static_cast<float>(data->_Data[0][source_texture_index++]) / 255.0f;
		}
	}
#endif
}

/*
*	Creates a texture 3D.
*/
void ResourceCreationSystem::CreateTexture3D(Texture3DData* const RESTRICT data, Texture3DHandle* const RESTRICT texture) NOEXCEPT
{
	//Create a texture 3D ouf of the incoming data.
	Texture3D<Vector4<byte>> temporary_texture{ data->_Width, data->_Height, data->_Depth, data->_Data[0].Data() };

	//Create the texture!
	RenderingSystem::Instance->CreateTexture3D(TextureData(TextureDataContainer(temporary_texture), TextureFormat::RGBA_UINT8), texture);
}