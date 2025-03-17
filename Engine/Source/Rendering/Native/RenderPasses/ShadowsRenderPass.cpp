//Header file.
#include <Rendering/Native/RenderPasses/ShadowsRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/RenderingSystem.h>

//STL.
#include <stdio.h>

/*
*	Default constructor.
*/
ShadowsRenderPass::ShadowsRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::SHADOWS, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		ShadowsRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		ShadowsRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		ShadowsRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void ShadowsRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Register the input stream(s).
	{
		struct ShadowsSpatialDenoisingPushConstantData final
		{
			uint32 _Direction;
		};

		RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
		(
			HashString("ShadowsHorizontalSpatialDenoising"),
			DynamicArray<VertexInputAttributeDescription>(),
			DynamicArray<VertexInputBindingDescription>(),
			sizeof(ShadowsSpatialDenoisingPushConstantData),
			[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
			{
				//Clear the entries.
				input_stream->_Entries.Clear();

				//Clear the push constant data memory.
				input_stream->_PushConstantDataMemory.Clear();

				//Add an entry.
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = 0;
				new_entry._VertexBuffer = EMPTY_HANDLE;
				new_entry._IndexBuffer = EMPTY_HANDLE;
				new_entry._IndexBufferOffset = 0;
				new_entry._InstanceBuffer = EMPTY_HANDLE;
				new_entry._VertexCount = 3;
				new_entry._IndexCount = 0;
				new_entry._InstanceCount = 0;

				//Set up the push constant data.
				ShadowsSpatialDenoisingPushConstantData push_constant_data;

				push_constant_data._Direction = 0;

				for (uint64 i{ 0 }; i < sizeof(ShadowsSpatialDenoisingPushConstantData); ++i)
				{
					input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
				}
			},
			RenderInputStream::Mode::DRAW,
			nullptr
		);

		RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
		(
			HashString("ShadowsVerticalSpatialDenoising"),
			DynamicArray<VertexInputAttributeDescription>(),
			DynamicArray<VertexInputBindingDescription>(),
			sizeof(ShadowsSpatialDenoisingPushConstantData),
			[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
			{
				//Clear the entries.
				input_stream->_Entries.Clear();

				//Clear the push constant data memory.
				input_stream->_PushConstantDataMemory.Clear();

				//Add an entry.
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = 0;
				new_entry._VertexBuffer = EMPTY_HANDLE;
				new_entry._IndexBuffer = EMPTY_HANDLE;
				new_entry._IndexBufferOffset = 0;
				new_entry._InstanceBuffer = EMPTY_HANDLE;
				new_entry._VertexCount = 3;
				new_entry._IndexCount = 0;
				new_entry._InstanceCount = 0;

				//Set up the push constant data.
				ShadowsSpatialDenoisingPushConstantData push_constant_data;

				push_constant_data._Direction = 1;

				for (uint64 i{ 0 }; i < sizeof(ShadowsSpatialDenoisingPushConstantData); ++i)
				{
					input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
				}
			},
			RenderInputStream::Mode::DRAW,
			nullptr
		);
	}

	//Add the pipelines.
	SetNumberOfPipelines(_ClearShadowMapPipelines.Size() + _OpaqueModelShadowMapPipelines.Size() + _MaskedModelShadowMapPipelines.Size() + 2 + _ShadowsSpatialDenoisingPipelines.Size());

	//Add all pipelines.
	for (uint8 i{ 0 }; i < 4; ++i)
	{
		AddPipeline(&_ClearShadowMapPipelines[i]);
	}

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		AddPipeline(&_OpaqueModelShadowMapPipelines[i]);
	}

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		AddPipeline(&_MaskedModelShadowMapPipelines[i]);
	}

	AddPipeline(&_ShadowMapResolvePipeline);
	AddPipeline(&_RayTracedShadowsPipeline);

	for (GraphicsRenderPipeline &pipeline : _ShadowsSpatialDenoisingPipelines)
	{
		AddPipeline(&pipeline);
	}

	//Initialize all pipelines.
	for (uint8 i{ 0 }; i < ShadowsSystem::MAXIMUM_NUMBER_OF_SHADOW_MAP_DATA; ++i)
	{
		_ClearShadowMapPipelines[i].SetIncludeInRender(false);
	}

	for (uint8 i{ 0 }; i < ShadowsSystem::MAXIMUM_NUMBER_OF_SHADOW_MAP_DATA; ++i)
	{
		_OpaqueModelShadowMapPipelines[i].SetIncludeInRender(false);
	}

	for (uint8 i{ 0 }; i < ShadowsSystem::MAXIMUM_NUMBER_OF_SHADOW_MAP_DATA; ++i)
	{
		_MaskedModelShadowMapPipelines[i].SetIncludeInRender(false);
	}

	_ShadowMapResolvePipeline.Initialize();

	{
		RayTracingRenderPipelineParameters parameters;

		parameters._ComputeRenderTargets.Emplace(HashString("Shadows"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1));

		_RayTracedShadowsPipeline.Initialize(parameters);
	}

	for (uint64 i{ 0 }; i < _ShadowsSpatialDenoisingPipelines.Size(); ++i)
	{
		GraphicsRenderPipelineInitializeParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("InputShadows"), i == 0 ? RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1) : RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2));
		parameters._OutputRenderTargets.Emplace(HashString("OutputShadows"), i == 0 ? RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2) : RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1));
		parameters._InputStreamSubscriptions.Emplace(i == 0 ? HashString("ShadowsHorizontalSpatialDenoising") : HashString("ShadowsVerticalSpatialDenoising"));

		_ShadowsSpatialDenoisingPipelines[i].Initialize(parameters);
	}
}

/*
*	Executes this render pass.
*/
void ShadowsRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	if (RenderingSystem::Instance->GetRenderingConfiguration()->GetSurfaceShadowsMode() == RenderingConfiguration::SurfaceShadowsMode::NONE)
	{
		SetEnabled(false);

		return;
	}

	if (RenderingSystem::Instance->GetRenderingConfiguration()->GetSurfaceShadowsMode() == RenderingConfiguration::SurfaceShadowsMode::RASTERIZED)
	{
		//Cache the number of shadow map data.
		const uint32 number_of_shadow_map_data{ RenderingSystem::Instance->GetShadowsSystem()->GetNumberOfShadowMapData() };

		//Execute all shadow map rendering.
		for (uint32 i{ 0 }; i < number_of_shadow_map_data; ++i)
		{
			const ShadowsSystem::ShadowMapData &shadow_map_data{ RenderingSystem::Instance->GetShadowsSystem()->GetShadowMapData(i) };

			if (!_ShadowMapPipelinesIsInitialized[i])
			{
				{
					GraphicsRenderPipelineInitializeParameters parameters;

					parameters._DepthBuffer = Pair<HashString, DepthBufferHandle>(HashString("ShadowMapDepthBuffer"), shadow_map_data._DepthBuffer);
					parameters._OutputRenderTargets.Emplace(HashString("ShadowMap"), shadow_map_data._RenderTarget);

					_ClearShadowMapPipelines[i].Initialize(parameters);
				}

				{
					GraphicsRenderPipelineInitializeParameters parameters;

					parameters._DepthBuffer = Pair<HashString, DepthBufferHandle>(HashString("ShadowMapDepthBuffer"), shadow_map_data._DepthBuffer);
					parameters._OutputRenderTargets.Emplace(HashString("ShadowMap"), shadow_map_data._RenderTarget);
					char buffer[64];
					sprintf_s(buffer, "OpaqueModelsShadowMap%u", static_cast<uint32>(i));
					parameters._InputStreamSubscriptions.Emplace(buffer);

					_OpaqueModelShadowMapPipelines[i].Initialize(parameters);
				}

				{
					GraphicsRenderPipelineInitializeParameters parameters;

					parameters._DepthBuffer = Pair<HashString, DepthBufferHandle>(HashString("ShadowMapDepthBuffer"), shadow_map_data._DepthBuffer);
					parameters._OutputRenderTargets.Emplace(HashString("ShadowMap"), shadow_map_data._RenderTarget);
					char buffer[64];
					sprintf_s(buffer, "MaskedModelsShadowMap%u", static_cast<uint32>(i));
					parameters._InputStreamSubscriptions.Emplace(buffer);

					_MaskedModelShadowMapPipelines[i].Initialize(parameters);
				}

				_ShadowMapPipelinesIsInitialized[i] = true;
			}

			_ClearShadowMapPipelines[i].Execute();
			_OpaqueModelShadowMapPipelines[i].Execute();
			_MaskedModelShadowMapPipelines[i].Execute();
		}

		for (uint32 i{ number_of_shadow_map_data }; i < ShadowsSystem::MAXIMUM_NUMBER_OF_SHADOW_MAP_DATA; ++i)
		{
			if (_ShadowMapPipelinesIsInitialized[i])
			{
				_ClearShadowMapPipelines[i].Terminate();
				_ClearShadowMapPipelines[i].SetIncludeInRender(false);
				_OpaqueModelShadowMapPipelines[i].Terminate();
				_OpaqueModelShadowMapPipelines[i].SetIncludeInRender(false);
				_MaskedModelShadowMapPipelines[i].Terminate();
				_MaskedModelShadowMapPipelines[i].SetIncludeInRender(false);

				_ShadowMapPipelinesIsInitialized[i] = false;
			}
		}

		_ShadowMapResolvePipeline.Execute();
		_RayTracedShadowsPipeline.SetIncludeInRender(false);
	}

	else if (RenderingSystem::Instance->GetRenderingConfiguration()->GetSurfaceShadowsMode() == RenderingConfiguration::SurfaceShadowsMode::RAY_TRACED)
	{
		for (uint32 i{ 0 }; i < ShadowsSystem::MAXIMUM_NUMBER_OF_SHADOW_MAP_DATA; ++i)
		{
			_ClearShadowMapPipelines[i].SetIncludeInRender(false);
			_OpaqueModelShadowMapPipelines[i].SetIncludeInRender(false);
			_MaskedModelShadowMapPipelines[i].SetIncludeInRender(false);
		}

		_ShadowMapResolvePipeline.SetIncludeInRender(false);
		_RayTracedShadowsPipeline.Execute();
	}

	for (GraphicsRenderPipeline &pipeline : _ShadowsSpatialDenoisingPipelines)
	{
		pipeline.Execute();
	}

	//Enable this render pass.
	SetEnabled(true);
}

/*
*	Terminates this render pass.
*/
void ShadowsRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	for (uint32 i{ 0 }; i < ShadowsSystem::MAXIMUM_NUMBER_OF_SHADOW_MAP_DATA; ++i)
	{
		if (_ShadowMapPipelinesIsInitialized[i])
		{
			_ClearShadowMapPipelines[i].Terminate();
			_OpaqueModelShadowMapPipelines[i].Terminate();
			_MaskedModelShadowMapPipelines[i].Terminate();

			_ShadowMapPipelinesIsInitialized[i] = false;
		}
	}

	_ShadowMapResolvePipeline.Terminate();
	_RayTracedShadowsPipeline.Terminate();

	for (GraphicsRenderPipeline &pipeline : _ShadowsSpatialDenoisingPipelines)
	{
		pipeline.Terminate();
	}
}