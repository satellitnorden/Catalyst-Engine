//Header file.
#include <Resources/Creation/ResourceCreationSystem.h>

//Systems.
#include <Systems/ContentSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

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
	if (!data->_RayMissShaderData.Empty())
	{
		resource->_RayMissShaderHandles.Reserve(data->_RayMissShaderData.Size());

		for (RenderPipelineData::ShaderData &ray_miss_shader_data : data->_RayMissShaderData)
		{
			resource->_RayMissShaderHandles.Emplace();
			RenderingSystem::Instance->CreateShader(ArrayProxy<byte>(ray_miss_shader_data._GLSLData), ShaderStage::RAY_MISS, &resource->_RayMissShaderHandles.Back());
		}
	}

	//Create the ray hit groups.
	if (!data->_RayHitGroupShaderData.Empty())
	{
		resource->_RayHitGroupShaderData.Upsize<true>(data->_RayHitGroupShaderData.Size());

		for (uint64 ray_hit_group_index{ 0 }; ray_hit_group_index < data->_RayHitGroupShaderData.Size(); ++ray_hit_group_index)
		{
			resource->_RayHitGroupShaderData[ray_hit_group_index]._Identifier = data->_RayHitGroupShaderData[ray_hit_group_index]._Identifier;

			if (!data->_RayHitGroupShaderData[ray_hit_group_index]._RayClosestHitShaderData._GLSLData.Empty())
			{
				RenderingSystem::Instance->CreateShader(ArrayProxy<byte>(data->_RayHitGroupShaderData[ray_hit_group_index]._RayClosestHitShaderData._GLSLData), ShaderStage::RAY_CLOSEST_HIT, &resource->_RayHitGroupShaderData[ray_hit_group_index]._RayClosestHitShaderHandle);
			}

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
	resource->_OutputRenderTargets = std::move(data->_OutputRenderTargets);

	//Copy the blend properties.
	resource->_BlendColorOperator = data->_BlendColorOperator;
	resource->_BlendAlphaSourceFactor = data->_BlendAlphaSourceFactor;
	resource->_BlendAlphaDestinationFactor = data->_BlendAlphaDestinationFactor;
	resource->_BlendAlphaOperator = data->_BlendAlphaOperator;

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