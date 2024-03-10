//Header file.
#include <Resources/Creation/ResourceCreationSystem.h>

//Systems.
#include <Systems/ContentSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

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
																		BottomLevelAccelerationStructureFlag::NONE,
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
void ResourceCreationSystem::CreateFont(FontData *const RESTRICT data, FontResource *const RESTRICT resource) NOEXCEPT
{
	//Just copy the character descriptions.
	resource->_CharacterDescriptions = data->_CharacterDescriptions;

	//Create the master texture.
	Texture2DHandle master_texture;
	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(data->_MasterTextureData, data->_MasterTextureWidth, data->_MasterTextureHeight, 1), TextureFormat::R_UINT8, TextureUsage::NONE, false), &master_texture);

	resource->_MasterTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(master_texture);

	//Calculate the default height.
	resource->_DefaultHeight = FLOAT32_MAXIMUM;

	for (uint8 i{ 0 }; i < FontResource::NUMBER_OF_CHARACTER_DESCRIPTIONS; ++i)
	{
		if (resource->_CharacterDescriptions[i]._Size._Y != 0.0f
			&& ((i >= 'A' && i <= 'Z')
			|| (i >= 'a' && i <= 'z')))
		{
			resource->_DefaultHeight = CatalystBaseMath::Minimum<float32>(resource->_DefaultHeight, resource->_CharacterDescriptions[i]._Size._Y);
		}
	}

	if (resource->_DefaultHeight == FLOAT32_MAXIMUM)
	{
		ASSERT(false, "This shouldn't happen...");

		resource->_DefaultHeight = 0.0f;

		float32 number_of_valid_characters{ 0.0f };

		for (uint8 i{ 0 }; i < FontResource::NUMBER_OF_CHARACTER_DESCRIPTIONS; ++i)
		{
			if (resource->_CharacterDescriptions[i]._Size._Y != 0.0f)
			{
				resource->_DefaultHeight += resource->_CharacterDescriptions[i]._Size._Y;
			
				++number_of_valid_characters;
			}
		}

		if (number_of_valid_characters > 0.0f)
		{
			resource->_DefaultHeight /= number_of_valid_characters;
		}

		else
		{
			ASSERT(false, "This shouldn't happen...");

			resource->_DefaultHeight = 0.1f;
		}
	}
}

/*
*	Creates a level.
*/
void ResourceCreationSystem::CreateLevel(LevelData *const RESTRICT data, LevelResource *const RESTRICT resource) NOEXCEPT
{
	//Copy over all level entries.
	resource->_LevelEntries.Reserve(data->_LevelEntries.Size());

	for (const LevelData::LevelEntry &level_entry : data->_LevelEntries)
	{
		resource->_LevelEntries.Emplace();
		LevelResource::LevelEntry &new_level_entry{ resource->_LevelEntries.Back() };

		new_level_entry._Name = level_entry._Name;
		new_level_entry._ComponentEntries.Reserve(level_entry._ComponentEntries.Size());

		for (const LevelData::LevelEntry::ComponentEntry &component_entry : level_entry._ComponentEntries)
		{
			new_level_entry._ComponentEntries.Emplace();
			LevelResource::LevelEntry::ComponentEntry &new_component_entry{ new_level_entry._ComponentEntries.Back() };

			for (Component *const RESTRICT component : AllComponents())
			{
				if (component->_Identifier == component_entry._ComponentIdentifier)
				{
					new_component_entry._Component = component;

					break;
				}
			}

			new_component_entry._NumberOfEditableFields = component_entry._NumberOfEditableFields;
			new_component_entry._FieldDataPosition = component_entry._FieldDataPosition;
		}
	}

	//Copy the stream archive.
	resource->_StreamArchive = data->_StreamArchive;
}

/*
*	Creates a raw data.
*/
void ResourceCreationSystem::CreateRawData(RawDataData *const RESTRICT data, RawDataResource *const RESTRICT resource) NOEXCEPT
{
	//Move the data.
	resource->_Data = std::move(data->_Data);
}

/*
*	Creates a render pipeline.
*/
void ResourceCreationSystem::CreateRenderPipeline(RenderPipelineData *const RESTRICT data, RenderPipelineResource *const RESTRICT resource) NOEXCEPT
{
	//Create the compute shader.
	if (!data->_ComputeShaderData._GLSLData.Empty())
	{
		RenderingSystem::Instance->CreateShader(ArrayProxy<byte>(data->_ComputeShaderData._GLSLData), ShaderStage::COMPUTE, &resource->_ComputeShaderHandle);
	}

	//Create the vertex shader.
	if (!data->_VertexShaderData._GLSLData.Empty())
	{
		RenderingSystem::Instance->CreateShader(ArrayProxy<byte>(data->_VertexShaderData._GLSLData), ShaderStage::VERTEX, &resource->_VertexShaderHandle);
	}

	//Create the fragment shader.
	if (!data->_FragmentShaderData._GLSLData.Empty())
	{
		RenderingSystem::Instance->CreateShader(ArrayProxy<byte>(data->_FragmentShaderData._GLSLData), ShaderStage::FRAGMENT, &resource->_FragmentShaderHandle);
	}

	//Create the ray generation shader.
	if (!data->_RayGenerationShaderData._GLSLData.Empty())
	{
		RenderingSystem::Instance->CreateShader(ArrayProxy<byte>(data->_RayGenerationShaderData._GLSLData), ShaderStage::RAY_GENERATION, &resource->_RayGenerationShaderHandle);
	}

	//Create the ray miss shader.
	if (!data->_RayMissShaderData._GLSLData.Empty())
	{
		RenderingSystem::Instance->CreateShader(ArrayProxy<byte>(data->_RayMissShaderData._GLSLData), ShaderStage::RAY_MISS, &resource->_RayMissShaderHandle);
	}

	//Create the ray hit groups.
	if (!data->_RayHitGroupShaderData.Empty())
	{
		resource->_RayHitGroupShaderData.Upsize<true>(data->_RayHitGroupShaderData.Size());

		for (uint64 ray_hit_group_index{ 0 }; ray_hit_group_index < data->_RayHitGroupShaderData.Size(); ++ray_hit_group_index)
		{
			resource->_RayHitGroupShaderData[ray_hit_group_index]._Identifier = data->_RayHitGroupShaderData[ray_hit_group_index]._Identifier;

			if (!data->_RayHitGroupShaderData[ray_hit_group_index]._RayAnyHitShaderData._GLSLData.Empty())
			{
				RenderingSystem::Instance->CreateShader(ArrayProxy<byte>(data->_RayHitGroupShaderData[ray_hit_group_index]._RayAnyHitShaderData._GLSLData), ShaderStage::RAY_ANY_HIT, &resource->_RayHitGroupShaderData[ray_hit_group_index]._RayAnyHitShaderHandle);
			}
		}
	}

	//Copy the included buffers.
	resource->_IncludedUniformBuffers = std::move(data->_IncludedUniformBuffers);
	resource->_IncludedStorageBuffers = std::move(data->_IncludedStorageBuffers);

	//Copy the compute render targets.
	resource->_ComputeRenderTargets = std::move(data->_ComputeRenderTargets);

	//Copy the input/output depth buffer/render targets.
	resource->_InputRenderTargets = std::move(data->_InputRenderTargets);
	resource->_OutputDepthBuffer = data->_OutputDepthBuffer;
	resource->_OutputRenderTargets = std::move(data->_OutputRenderTargets);

	//Copy the render resolution.
	resource->_RenderResolution = data->_RenderResolution;

	//Copy the load/store operators.
	resource->_ColorLoadOperator = data->_ColorLoadOperator;
	resource->_ColorStoreOperator = data->_ColorStoreOperator;
	resource->_DepthStencilLoadOperator = data->_DepthStencilLoadOperator;
	resource->_DepthStencilStoreOperator = data->_DepthStencilStoreOperator;

	//Copy the blend properties.
	resource->_BlendEnabled = data->_BlendEnabled;
	resource->_BlendColorSourceFactor = data->_BlendColorSourceFactor;
	resource->_BlendColorDestinationFactor = data->_BlendColorDestinationFactor;
	resource->_BlendColorOperator = data->_BlendColorOperator;
	resource->_BlendAlphaSourceFactor = data->_BlendAlphaSourceFactor;
	resource->_BlendAlphaDestinationFactor = data->_BlendAlphaDestinationFactor;
	resource->_BlendAlphaOperator = data->_BlendAlphaOperator;

	//Copy the cull mode.
	resource->_CullMode = data->_CullMode;

	//Copy the depth/stencil properties.
	resource->_DepthTestEnabled = data->_DepthTestEnabled;
	resource->_DepthWriteEnabled = data->_DepthWriteEnabled;
	resource->_DepthCompareOperator = data->_DepthCompareOperator;
	resource->_StencilTestEnabled = data->_StencilTestEnabled;
	resource->_StencilFailOperator = data->_StencilFailOperator;
	resource->_StencilPassOperator = data->_StencilPassOperator;
	resource->_StencilDepthFailOperator = data->_StencilDepthFailOperator;
	resource->_StencilCompareOperator = data->_StencilCompareOperator;
	resource->_StencilCompareMask = data->_StencilCompareMask;
	resource->_StencilWriteMask = data->_StencilWriteMask;
	resource->_StencilReferenceMask = data->_StencilReferenceMask;

	//Copy the topology.
	resource->_Topology = data->_Topology;

	//Copy the sampler properties.
	resource->_SamplerProperties = std::move(data->_SamplerProperties);

	//Copy the input stream subscriptions.
	resource->_InputStreamSubscriptions = std::move(data->_InputStreamSubscriptions);
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
*	Creates a texture 3D.
*/
void ResourceCreationSystem::CreateTexture3D(Texture3DData *const RESTRICT data, Texture3DResource *const RESTRICT resource) NOEXCEPT
{
	//Create a texture 3D ouf of the incoming data.
	Texture3D<Vector4<byte>> temporary_texture{ data->_Width, data->_Height, data->_Depth, data->_Data[0].Data() };

	//Create the texture!
	RenderingSystem::Instance->CreateTexture3D(TextureData(TextureDataContainer(temporary_texture), TextureFormat::RGBA_UINT8, TextureUsage::NONE, false), &resource->_Texture3DHandle);
}

/*
*	Creates a texture cube.
*/
void ResourceCreationSystem::CreateTextureCube(TextureCubeData *const RESTRICT data, TextureCubeResource *const RESTRICT resource) NOEXCEPT
{
	//Create the texture cube.
	resource->_TextureCube.Initialize(data->_Resolution, data->_Data[0]);

	//Set the number of mipmap levels.
	resource->_MipmapLevels = data->_MipmapLevels;

	//Create the texture cube handle.
	RenderingSystem::Instance->CreateTextureCube(*data, &resource->_TextureCubeHandle);
}

/*
*	Creates a video.
*/
void ResourceCreationSystem::CreateVideo(VideoData *const RESTRICT data, VideoResource *const RESTRICT resource) NOEXCEPT
{
	//Copy the width/height/frames per second/frames per second reciprocal.
	resource->_Width = data->_Width;
	resource->_Height = data->_Height;
	resource->_FramesPerSecond = data->_FramesPerSecond;
	resource->_FramesPerSecondReciprocal = data->_FramesPerSecondReciprocal;

	//Move the frames.
	resource->_Frames = std::move(data->_Frames);
}