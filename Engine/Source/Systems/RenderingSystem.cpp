//Header file.
#include <Systems/RenderingSystem.h>

//Core.
#include <Core/General/CatalystProjectConfiguration.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Types/Entity.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>
#include <Math/Core/CatalystRandomMath.h>
#include <Math/Noise/HammersleySequence.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>
#include <Rendering/Native/Resolution.h>
#include <Rendering/Native/TextureData.h>
#include <Rendering/Native/NativeRenderPassManager.h>
#include <Rendering/Native/RenderPasses/DebugRenderPass.h>
#include <Rendering/Translation/OpenGL/OpenGLSubRenderingSystem.h>
#include <Rendering/Translation/Vulkan/VulkanSubRenderingSystem.h>

//Systems.
#include <Systems/AnimationSystem.h>
#include <Systems/CatalystEngineSystem.h>
#if !defined(CATALYST_CONFIGURATION_FINAL)
#include <Systems/DebugSystem.h>
#endif
#include <Systems/ResourceSystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/TerrainSystem.h>
#include <Systems/WorldSystem.h>

//Third party.
#if !defined(CATALYST_CONFIGURATION_FINAL)
#include <ThirdParty/imgui.h>
#endif

//Singleton definition.
DEFINE_SINGLETON(RenderingSystem);

/*
*	Initializes the rendering system.
*/
void RenderingSystem::Initialize(const CatalystProjectRenderingConfiguration &configuration) NOEXCEPT
{
	//Set the configuration.
	_Configuration = configuration;

	//Create the sub rendering system.
	switch (_Configuration._SubRenderingSystem)
	{
		case SubRenderingSystem::OPEN_GL:
		{
			_SubRenderingSystem = new OpenGLSubRenderingSystem();

			break;
		}

		case SubRenderingSystem::VULKAN:
		{
			_SubRenderingSystem = new VulkanSubRenderingSystem();

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Set the initial rendering path.
	_CurrentRenderingPath = _Configuration._InitialRenderingPath;

	//Set the full resolution.
	_FullResolution = _Configuration._Resolution.Valid() ? _Configuration._Resolution.Get() : CatalystPlatform::GetDefaultResolution();

	//Set the scaled resolutions.
	_ScaledResolutions[0] = Resolution(static_cast<uint32>(static_cast<float32>(_FullResolution._Width) * _Configuration._ResolutionScale), static_cast<uint32>(static_cast<float32>(_FullResolution._Height) * _Configuration._ResolutionScale));
	_ScaledResolutions[0].RoundUpToNearestMultipleOfTwo();

	_ScaledResolutions[1] = _ScaledResolutions[0] / 2;
	_ScaledResolutions[1].RoundUpToNearestMultipleOfTwo();

	_ScaledResolutions[2] = _ScaledResolutions[1] / 2;
	_ScaledResolutions[2].RoundUpToNearestMultipleOfTwo();

	_ScaledResolutions[3] = _ScaledResolutions[2] / 2;
	_ScaledResolutions[3].RoundUpToNearestMultipleOfTwo();

	_ScaledResolutions[4] = _ScaledResolutions[3] / 2;
	_ScaledResolutions[4].RoundUpToNearestMultipleOfTwo();

	_ScaledResolutions[5] = _ScaledResolutions[4] / 2;
	_ScaledResolutions[5].RoundUpToNearestMultipleOfTwo();

	_ScaledResolutions[6] = _ScaledResolutions[5] / 2;
	_ScaledResolutions[6].RoundUpToNearestMultipleOfTwo();

	_ScaledResolutions[7] = _ScaledResolutions[6] / 2;
	_ScaledResolutions[7].RoundUpToNearestMultipleOfTwo();

	_ScaledResolutions[8] = _ScaledResolutions[7] / 2;
	_ScaledResolutions[8].RoundUpToNearestMultipleOfTwo();

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Initialize the debug rendering system.
	_DebugRenderingSystem.Initialize();
#endif

	//Initialize the model system.
	_ModelSystem.Initialize();

	//Pre-initialize the sub rendering system.
	_SubRenderingSystem->PreInitialize();

	//Initialize the render input manager.
	_RenderInputManager.Initialize();

	//Initialize the shared render target manager.
	_SharedRenderTargetManager.Initialize(_CurrentRenderingPath);

	//Initialize all render targets.
	InitializeRenderTargets();

	//Initialize all samplers.
	InitializeSamplers();

	//Initialize all common render data table layouts.
	InitializeCommonRenderDataTableLayouts();

	//Initialize all common render data tables.
	InitializeCommonRenderDataTables();

	//Initialize the default texture.
	InitializeDefaultTexture();

	//Initialize the Hammersley hemisphere samples uniform buffer.
	InitializeHammersleyHemisphereSamplesUniformBuffer();

	//Pre-initialize the global render data.
	PreInitializeGlobalRenderData();

	//Initialize the sub rendering system.
	_SubRenderingSystem->Initialize();

	//Register uniformbuffers.
	_BufferManager.RegisterUniformBuffer
	(
		HashString("General"),
		&_GeneralUniformData,
		sizeof(GeneralUniformData)
	);

	_BufferManager.RegisterUniformBuffer
	(
		HashString("HammersleyHemisphereSamples"),
		&_HammersleyHemisphereSamplesUniformData,
		sizeof(HammersleyHemisphereSamplesUniformData)
	);

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Register debug commands.
	DebugSystem::Instance->RegisterDebugCommand
	(
		"Performance\\Rendering",
		[](class DebugCommand *const RESTRICT debug_command, void *const RESTRICT user_data)
		{
			RenderingSystem::Instance->_PerformanceWindowOpen = !RenderingSystem::Instance->_PerformanceWindowOpen;
		},
		nullptr
	);
	DebugSystem::Instance->RegisterDebugCommand
	(
		"Rendering\\Visualization Modes\\None",
		[](class DebugCommand *const RESTRICT debug_command, void *const RESTRICT user_data)
		{
			DebugRenderPass::Instance->SetMode(DebugRenderPass::Mode::NONE);
		},
		nullptr
	);
	DebugSystem::Instance->RegisterDebugCommand
	(
		"Rendering\\Visualization Modes\\Albedo",
		[](class DebugCommand *const RESTRICT debug_command, void *const RESTRICT user_data)
		{
			DebugRenderPass::Instance->SetMode(DebugRenderPass::Mode::ALBEDO);
		},
		nullptr
	);
	DebugSystem::Instance->RegisterDebugCommand
	(
		"Rendering\\Visualization Modes\\Thickness",
		[](class DebugCommand *const RESTRICT debug_command, void *const RESTRICT user_data)
		{
			DebugRenderPass::Instance->SetMode(DebugRenderPass::Mode::THICKNESS);
		},
		nullptr
	);
	DebugSystem::Instance->RegisterDebugCommand
	(
		"Rendering\\Visualization Modes\\Normal",
		[](class DebugCommand *const RESTRICT debug_command, void *const RESTRICT user_data)
		{
			DebugRenderPass::Instance->SetMode(DebugRenderPass::Mode::NORMAL);
		},
		nullptr
	);
	DebugSystem::Instance->RegisterDebugCommand
	(
		"Rendering\\Visualization Modes\\Depth",
		[](class DebugCommand *const RESTRICT debug_command, void *const RESTRICT user_data)
		{
			DebugRenderPass::Instance->SetMode(DebugRenderPass::Mode::DEPTH);
		},
		nullptr
	);
	DebugSystem::Instance->RegisterDebugCommand
	(
		"Rendering\\Visualization Modes\\Roughness",
		[](class DebugCommand *const RESTRICT debug_command, void *const RESTRICT user_data)
		{
			DebugRenderPass::Instance->SetMode(DebugRenderPass::Mode::ROUGHNESS);
		},
		nullptr
	);
	DebugSystem::Instance->RegisterDebugCommand
	(
		"Rendering\\Visualization Modes\\Metallic",
		[](class DebugCommand *const RESTRICT debug_command, void *const RESTRICT user_data)
		{
			DebugRenderPass::Instance->SetMode(DebugRenderPass::Mode::METALLIC);
		},
		nullptr
	);
	DebugSystem::Instance->RegisterDebugCommand
	(
		"Rendering\\Visualization Modes\\Ambient Occlusion",
		[](class DebugCommand *const RESTRICT debug_command, void *const RESTRICT user_data)
		{
			DebugRenderPass::Instance->SetMode(DebugRenderPass::Mode::AMBIENT_OCCLUSION);
		},
		nullptr
	);
	DebugSystem::Instance->RegisterDebugCommand
	(
		"Rendering\\Visualization Modes\\Emissive",
		[](class DebugCommand *const RESTRICT debug_command, void *const RESTRICT user_data)
		{
			DebugRenderPass::Instance->SetMode(DebugRenderPass::Mode::EMISSIVE);
		},
		nullptr
	);
#endif
}

/*
*	Post initializes the rendering system.
*/
void RenderingSystem::PostInitialize() NOEXCEPT
{
	//Post-initialize the global render data.
	PostInitializeGlobalRenderData();

	//Post-initialize the common render data tables.
	PostInitializeCommonRenderDataTables();

	//Initialize the camera system.
	_CameraSystem.Initialize();

	//Initialize the culling system.
	_CullingSystem.Initialize();

	//Post-initialize the lighting system.
	_LightingSystem.PostInitialize();

	//Post-initialize the material system.
	_MaterialSystem.PostInitialize();

	//Post-initialize the model system.
	_ModelSystem.PostInitialize();

	//Initialize the post processing system.
	_PostProcessingSystem.Initialize();

	//Post-initialize the ray tracing system.
	_RayTracingSystem.PostInitialize();

	//Post-initialize the shadows system.
	_ShadowsSystem.PostInitialize();

	//Initialize the rendering configuration.
	_RenderingConfiguration.Initialize();

	//Post-initialize the virtual reality system.
	_VirtualRealitySystem.PostInitialize();

	//Retrieve the render passes.
	if (_CurrentRenderingPath == RenderingPath::CUSTOM)
	{
		_Configuration._CustomRenderingPathCallback(&_RenderPasses);
	}

	else
	{
		NativeRenderPassManager::GetRenderPasses(_CurrentRenderingPath, &_RenderPasses);
	}

	//Initialize all render passes.
	for (RenderPass *const RESTRICT render_pass : _RenderPasses)
	{
		render_pass->Initialize();
	}

	//Post-initialize the sub rendering system.
	_SubRenderingSystem->PostInitialize();
}

/*
*	Updates the rendering system during the render update phase.
*/
void RenderingSystem::RenderUpdate() NOEXCEPT
{
	//Update the general uniform data.
	_GeneralUniformData._FullResolution = Vector2<float32>(static_cast<float32>(GetScaledResolution(0)._Width), static_cast<float32>(GetScaledResolution(0)._Height));
	_GeneralUniformData._InverseFullResolution = Vector2<float32>(1.0f / _GeneralUniformData._FullResolution._X, 1.0f / _GeneralUniformData._FullResolution._Y);
	_GeneralUniformData._HalfResolution = Vector2<float32>(static_cast<float32>(GetScaledResolution(1)._Width), static_cast<float32>(GetScaledResolution(1)._Height));
	_GeneralUniformData._InverseHalfResolution = Vector2<float32>(1.0f / _GeneralUniformData._HalfResolution._X, 1.0f / _GeneralUniformData._HalfResolution._Y);
	_GeneralUniformData._FrameIndex = static_cast<uint32>(CatalystEngineSystem::Instance->GetTotalFrames());
	_GeneralUniformData._BlueNoiseTextureIndex = _CurrentBlueNoiseTextureIndex;

	//Tell the sub rendering to begin the frame.
	{
		PROFILING_SCOPE(RenderingSystem_BeginFrame);

		_SubRenderingSystem->BeginFrame();
	}

#if !defined(CATALYST_CONFIGURATION_FINAL)
	{
		RenderingPerformanceData &current_performance_data{ _GlobalRenderData._PerformanceData[GetCurrentFramebufferIndex()] };

		//Update the performance window.
		if (_PerformanceWindowOpen)
		{
			//Retrieve the window resolution.
			const Vector2<float32> window_resolution{ static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Width), static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Height) };

			//Add the window.
			ImGui::Begin("Rendering Performance", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
			ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
			ImGui::SetWindowSize(ImVec2(window_resolution._X * 0.2'50f, window_resolution._Y));

			//Fill in the list.
			float64 total_execution_time{ 0.0 };

			for (uint64 i{ 0 }; i < current_performance_data._Names.Size(); ++i)
			{
				const uint64 execution_time{ GetExecutionTime(current_performance_data._QueryPool, static_cast<uint32>(i * 2)) };
				const float64 execution_time_milliseconds{ static_cast<float64>(execution_time) / 1'000.0 / 1'000.0 };

				ImGui::Text("%s - %f ms", current_performance_data._Names[i], execution_time_milliseconds);

				total_execution_time += execution_time_milliseconds;
			}

			ImGui::Text("--------");

			ImGui::Text("Total Execution Time - %f ms", total_execution_time);

			ImGui::End();
		}

		//ResetQueryPool(&current_performance_data._QueryPool);
	}
#endif

	//Update the shadows system.
	{
		PROFILING_SCOPE(RenderingSystem_ShadowsSystem_PreRenderUpdate);

		_ShadowsSystem.PreRenderUpdate();
	}

	//Update the culling system.
	{
		PROFILING_SCOPE(RenderingSystem_CullingSystem_PreRenderUpdate);

		_CullingSystem.PreRenderUpdate();
	}

	//Update the camera system.
	{
		PROFILING_SCOPE(RenderingSystem_CameraSystem_RenderUpdate);

		_CameraSystem.RenderUpdate();
	}

	//Update the post processing system.
	{
		PROFILING_SCOPE(RenderingSystem_PostProcessingSystem_RenderUpdate);

		_PostProcessingSystem.RenderUpdate();
	}

	//Update the rendering configuration.
	{
		PROFILING_SCOPE(RenderingSystem_RenderingConfiguration_RenderUpdate);

		_RenderingConfiguration.Update();
	}

	//Update the world system.
	{
		PROFILING_SCOPE(RenderingSystem_WorldSystem_RenderUpdate);

		WorldSystem::Instance->RenderUpdate();
	}

	//Update the global render data.
	{
		PROFILING_SCOPE(RenderingSystem_UpdateGlobalRenderData);

		UpdateGlobalRenderData();
	}

	//Update the lighting system.
	{
		PROFILING_SCOPE(RenderingSystem_LightingSystem_RenderUpdate);

		_LightingSystem.RenderUpdate();
	}

	//Update the material system.
	{
		PROFILING_SCOPE(RenderingSystem_MaterialSystem_RenderUpdate);

		_MaterialSystem.RenderUpdate();
	}

	//Update the ray tracing system.
	{
		PROFILING_SCOPE(RenderingSystem_RayTracingSystem_RenderUpdate);

		_RayTracingSystem.RenderUpdate();
	}

	//Update the virtual reality system.
	{
		PROFILING_SCOPE(RenderingSystem_VirtualRealitySystem_RenderUpdate);

		_VirtualRealitySystem.RenderUpdate();
	}

	//This shouldn't really be here, but let's have it here for now...
	{
		PROFILING_SCOPE(RenderingSystem_AnimationSystem_RenderUpdate);

		AnimationSystem::Instance->RenderUpdate();
	}

	//Update the render input manager.
	{
		PROFILING_SCOPE(RenderingSystem_RenderInputManager_RenderUpdate);

		_RenderInputManager.RenderUpdate();
	}

	//Update the buffer manager.
	{
		PROFILING_SCOPE(RenderingSystem_BufferManager_RenderUpdate);

		_BufferManager.RenderUpdate();
	}

	//Execute all render passes.
	{
		PROFILING_SCOPE(RenderingSystem_ExecuteAllRenderPasses);

		for (RenderPass *const RESTRICT render_pass : _RenderPasses)
		{
			render_pass->Execute();
		}
	}

	//Record the frame command buffer.
	CommandBuffer *const RESTRICT frame_command_buffer{ GetGlobalCommandBuffer(CommandBufferLevel::PRIMARY) };

	{
		//Begin the command buffer.
		frame_command_buffer->Begin(nullptr);

		//Record all execute commands.
#if !defined(CATALYST_CONFIGURATION_FINAL)
		_GlobalRenderData._PerformanceData[GetCurrentFramebufferIndex()]._Names.Clear();

		uint32 current_query_index{ 0 };
#endif
		for (RenderPass *const RESTRICT render_pass : _RenderPasses)
		{
			if (!render_pass->IsEnabled())
			{
				continue;
			}

			render_pass->PreRecord(frame_command_buffer);

			for (Pipeline *const RESTRICT pipeline : render_pass->GetPipelines())
			{
				if (!pipeline->IncludeInRender())
				{
					continue;
				}

#if !defined(CATALYST_CONFIGURATION_FINAL)
				if (pipeline->GetName())
				{
					_GlobalRenderData._PerformanceData[GetCurrentFramebufferIndex()]._Names.Emplace(pipeline->GetName());
					frame_command_buffer->WriteBeginTimestamp(pipeline, _GlobalRenderData._PerformanceData[GetCurrentFramebufferIndex()]._QueryPool, current_query_index++);
				}
#endif
				frame_command_buffer->ExecuteCommands(pipeline, pipeline->GetCommandBuffer());

#if !defined(CATALYST_CONFIGURATION_FINAL)
				if (pipeline->GetName())
				{
					frame_command_buffer->WriteEndTimestamp(pipeline, _GlobalRenderData._PerformanceData[GetCurrentFramebufferIndex()]._QueryPool, current_query_index++);
				}
#endif
			}

			render_pass->PostRecord(frame_command_buffer);
		}

		//End the command buffer.
		frame_command_buffer->End(nullptr);
	}

	//Tell the sub rendering system to end the frame.
	{
		PROFILING_SCOPE(RenderingSystem_SubRenderingSystem_EndFrame);

		_SubRenderingSystem->EndFrame(frame_command_buffer);
	}
}

/*
*	Terminates the rendering system.
*/
void RenderingSystem::Terminate() NOEXCEPT
{
	//Terminate the sub rendering system.
	_SubRenderingSystem->Terminate();

	//Destroy the sub rendering system.
	delete _SubRenderingSystem;
}

/*
*	Notifies the rendering system that a rendering platform event has occured.
*/
void RenderingSystem::OnRenderingPlatformEvent(const RenderingPlatformEvent rendering_platform_event) NOEXCEPT
{
	//Was the surface lost?
	if (TEST_BIT(UNDERLYING(rendering_platform_event), UNDERLYING(RenderingPlatformEvent::SURFACE_LOST)))
	{
		//Set the rendering path to NONE.
		_RestoreRenderingPath = _CurrentRenderingPath;
		SetCurrentRenderingPath(RenderingPath::NONE);

		//Tell the sub rendering system.
		_SubRenderingSystem->OnRenderingPlatformEvent(rendering_platform_event);
	}

	//Was the surface gained?
	else if (TEST_BIT(UNDERLYING(rendering_platform_event), UNDERLYING(RenderingPlatformEvent::SURFACE_GAINED)))
	{
		//The application will receive one of these messages in the beginning... Just ignore it. (:
		static bool first_time{ true };

		if (!first_time)
		{
			//Tell the sub rendering system.
			_SubRenderingSystem->OnRenderingPlatformEvent(rendering_platform_event);

			//Restore the rendering path.
			SetCurrentRenderingPath(_RestoreRenderingPath);
		}

		first_time = false;
	}
}

/*
*	Sets the current rendering path.
*/
void RenderingSystem::SetCurrentRenderingPath(const RenderingPath value) NOEXCEPT
{
	if (_CurrentRenderingPath == value)
	{
		return;
	}

	//Terminate all the current render passes.
	for (RenderPass *const RESTRICT render_pass : _RenderPasses)
	{
		render_pass->Terminate();
	}

	//Set the current rendering path.
	_CurrentRenderingPath = value;

	//Alert the shared render target manager.
	_SharedRenderTargetManager.OnSwitchRenderingPath(_CurrentRenderingPath);

	//Re-retrieve the render passes.
	if (_CurrentRenderingPath == RenderingPath::CUSTOM)
	{
		_Configuration._CustomRenderingPathCallback(&_RenderPasses);
	}

	else
	{
		NativeRenderPassManager::GetRenderPasses(_CurrentRenderingPath, &_RenderPasses);
	}

	//Initialize all new render passes.
	for (RenderPass *const RESTRICT render_pass : _RenderPasses)
	{
		render_pass->Initialize();
	}
}

/*
*	Returns whether or not geometry shaders is supported.
*/
NO_DISCARD bool RenderingSystem::IsGeometryShadersSupported() const NOEXCEPT
{
	return _SubRenderingSystem->IsGeometryShadersSupported();
}

/*
*	Returns whether or not multiview is supported.
*/
NO_DISCARD bool RenderingSystem::IsMultiviewSupported() const NOEXCEPT
{
	return _SubRenderingSystem->IsMultiviewSupported();
}

/*
*	Returns whether or not ray tracing is supported.
*/
NO_DISCARD bool RenderingSystem::IsRayTracingSupported() const NOEXCEPT
{
	return _SubRenderingSystem->IsRayTracingSupported();
}

/*
*	Returns whether or not ray tracing is active.
*/
NO_DISCARD bool RenderingSystem::IsRayTracingActive() const NOEXCEPT
{
	return	IsRayTracingSupported() &&
			(GetCurrentRenderingPath() == RenderingPath::PATH_TRACING
			|| _RenderingConfiguration.GetAmbientOcclusionMode() == RenderingConfiguration::AmbientOcclusionMode::RAY_TRACED
			|| _RenderingConfiguration.GetIndirectLightingMode() == RenderingConfiguration::IndirectLightingMode::RAY_TRACED
			|| _RenderingConfiguration.GetSurfaceShadowsMode() == RenderingConfiguration::SurfaceShadowsMode::RAY_TRACED
			|| _RenderingConfiguration.GetVolumetricShadowsMode() == RenderingConfiguration::VolumetricShadowsMode::RAY_TRACED);
}

/*
*	Returns whether or not ray tracing is possible.
*/
NO_DISCARD bool RenderingSystem::IsRayTracingPossible() const NOEXCEPT
{
	return _RayTracingSystem.DoesRayTracingDataExist();
}

/*
*	Returns the number of framebuffers.
*/
NO_DISCARD uint8 RenderingSystem::GetNumberOfFramebuffers() const NOEXCEPT
{
	return _SubRenderingSystem->GetNumberOfFramebuffers();
}

/*
*	Returns the current framebuffer index.
*/
NO_DISCARD uint8 RenderingSystem::GetCurrentFramebufferIndex() const NOEXCEPT
{
	return _SubRenderingSystem->GetCurrentFramebufferIndex();
}

/*
*	Returns the current surface transform.
*/
NO_DISCARD SurfaceTransform RenderingSystem::GetCurrentSurfaceTransform() const NOEXCEPT
{
	return _SubRenderingSystem->GetCurrentSurfaceTransform();
}

/*
*	Returns the current surface transform rotation matrix.
*/
Matrix4x4 RenderingSystem::GetCurrentSurfaceTransformRotationMatrix() const NOEXCEPT
{
	switch (GetCurrentSurfaceTransform())
	{
		case SurfaceTransform::IDENTITY:
		{
			return Matrix4x4();
		}

		case SurfaceTransform::ROTATE_90:
		{
			return Matrix4x4
			(
				0.0f, 1.0f, 0.0f, 0.0f,
				-1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			);
		}

		case SurfaceTransform::ROTATE_270:
		{
			return Matrix4x4
			(
					0.0f, -1.0f, 0.0f, 0.0f,
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
			);
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			return Matrix4x4();
		}
	}
}

/*
*	Returns the given render target.
*/
RenderTargetHandle RenderingSystem::GetRenderTarget(const RenderTarget render_target) NOEXCEPT
{
	if (!_RenderTargets[UNDERLYING(render_target)])
	{
		switch (render_target)
		{
			case RenderTarget::INTERMEDIATE_RGBA_UINT8:
			{
				CreateRenderTarget(GetScaledResolution(0), TextureFormat::RGBA_UINT8, SampleCount::SAMPLE_COUNT_1, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_UINT8)]);

				break;
			}

			case RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2:
			{
				CreateRenderTarget(GetScaledResolution(1), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2)]);

				break;
			}

			case RenderTarget::INTERMEDIATE_RGBA_FLOAT32_EIGHTH:
			{
				CreateRenderTarget(GetScaledResolution(3), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_EIGHTH)]);

				break;
			}

			case RenderTarget::INTERMEDIATE_RGBA_FLOAT32_SIXTEENTH:
			{
				CreateRenderTarget(GetScaledResolution(4), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_SIXTEENTH)]);

				break;
			}

			case RenderTarget::INTERMEDIATE_RGBA_FLOAT32_THIRTYSECOND:
			{
				CreateRenderTarget(GetScaledResolution(5), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_THIRTYSECOND)]);

				break;
			}

			case RenderTarget::INTERMEDIATE_RGBA_FLOAT32_SIXTYFOURTH:
			{
				CreateRenderTarget(GetScaledResolution(6), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_SIXTYFOURTH)]);

				break;
			}

			case RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HUNDREDTWENTYEIGHTH:
			{
				CreateRenderTarget(GetScaledResolution(7), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HUNDREDTWENTYEIGHTH)]);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	return _RenderTargets[UNDERLYING(render_target)];
}

/*
*	Returns the sampler with the given configuration.
*/
NO_DISCARD SamplerHandle RenderingSystem::GetSampler(const SamplerProperties &properties) NOEXCEPT
{
	for (const CachedSampler &cached_sampler : _CachedSamplers)
	{
		if (cached_sampler._Properties == properties)
		{
			return cached_sampler._Handle;
		}
	}

	_CachedSamplers.Emplace();
	CachedSampler &new_cached_sampler{ _CachedSamplers.Back() };

	new_cached_sampler._Properties = properties;
	CreateSampler(new_cached_sampler._Properties, &new_cached_sampler._Handle);

	return new_cached_sampler._Handle;
}

/*
*	Creates a bottom level acceleration structure.
*/
void RenderingSystem::CreateBottomLevelAccelerationStructure(	const BufferHandle vertex_buffer,
																const uint32 number_of_vertices,
																const BufferHandle index_buffer,
																const uint32 number_of_indices,
																AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT
{
	_SubRenderingSystem->CreateBottomLevelAccelerationStructure(vertex_buffer, number_of_vertices, index_buffer, number_of_indices, handle);
}

/*
*	Creates a top level acceleration structure.
*/
void RenderingSystem::CreateTopLevelAccelerationStructure(const ArrayProxy<TopLevelAccelerationStructureInstanceData> &instance_data, AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT
{
	_SubRenderingSystem->CreateTopLevelAccelerationStructure(instance_data, handle);
}

/*
*	Destroys an acceleration structure.
*/
void RenderingSystem::DestroyAccelerationStructure(AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT
{
	_SubRenderingSystem->DestroyAccelerationStructure(handle);
}

/*
*	Creates a buffer.
*/
void RenderingSystem::CreateBuffer(const uint64 size, const BufferUsage usage, const MemoryProperty memory_properties, BufferHandle *const RESTRICT handle) const NOEXCEPT
{
	_SubRenderingSystem->CreateBuffer(size, usage, memory_properties, handle);
}

/*
*	Uploads data to a buffer.
*/
void RenderingSystem::UploadDataToBuffer(const void *const RESTRICT *const RESTRICT data, const uint64 *const RESTRICT data_sizes, const uint8 data_chunks, BufferHandle *const RESTRICT handle) const NOEXCEPT
{
	_SubRenderingSystem->UploadDataToBuffer(data, data_sizes, data_chunks, handle);
}

/*
*	Destroys a buffer.
*/
void RenderingSystem::DestroyBuffer(BufferHandle *const RESTRICT handle) const NOEXCEPT
{
	_SubRenderingSystem->DestroyBuffer(handle);
}

/*
*	Creates a command pool.
*/
void RenderingSystem::CreateCommandPool(const Pipeline::Type type, CommandPoolHandle *const RESTRICT handle) const NOEXCEPT
{
	_SubRenderingSystem->CreateCommandPool(type, handle);
}

/*
*	Resets a command pool.
*/
void RenderingSystem::ResetCommandPool(CommandPoolHandle *const RESTRICT handle) NOEXCEPT
{
	_SubRenderingSystem->ResetCommandPool(handle);
}

/*
*	Allocates a command buffer from the given command pool.
*/
RESTRICTED NO_DISCARD CommandBuffer *const RESTRICT RenderingSystem::AllocateCommandBuffer(const CommandPoolHandle command_pool, const CommandBufferLevel level) const NOEXCEPT
{
	return _SubRenderingSystem->AllocateCommandBuffer(command_pool, level);
}

/*
*	Returns a global command buffer from the global command pool.
*/
RESTRICTED NO_DISCARD CommandBuffer *const RESTRICT RenderingSystem::GetGlobalCommandBuffer(const CommandBufferLevel level) NOEXCEPT
{
	//Retrieve the thread data for the current frame.
	GlobalCommandPoolData::ThreadData &thread_data{ _GlobalRenderData._GlobalCommandPoolData[GetCurrentFramebufferIndex()]._ThreadData[Concurrency::CurrentThread::Index()] };

	//Retrieve the command buffer.
	switch (level)
	{
		case CommandBufferLevel::PRIMARY:
		{
			//Does a new command buffer need to be allocated?
			if (thread_data._NumberOfPrimaryCommandBuffersUsed >= thread_data._PrimaryCommandBuffers.Size())
			{
				thread_data._PrimaryCommandBuffers.Emplace(AllocateCommandBuffer(thread_data._CommandPool, CommandBufferLevel::PRIMARY));
			}

			return thread_data._PrimaryCommandBuffers[thread_data._NumberOfPrimaryCommandBuffersUsed++];
		}

		case CommandBufferLevel::SECONDARY:
		{
			//Does a new command buffer need to be allocated?
			if (thread_data._NumberOfSecondaryCommandBuffersUsed >= thread_data._SecondaryCommandBuffers.Size())
			{
				thread_data._SecondaryCommandBuffers.Emplace(AllocateCommandBuffer(thread_data._CommandPool, CommandBufferLevel::SECONDARY));
			}

			return thread_data._SecondaryCommandBuffers[thread_data._NumberOfSecondaryCommandBuffersUsed++];
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			return nullptr;
		}
	}
}

/*
*	Submits a command buffer.
*/
void RenderingSystem::SubmitCommandBuffer(const CommandBuffer *const RESTRICT command_buffer) NOEXCEPT
{
	_SubRenderingSystem->SubmitCommandBuffer(command_buffer);
}

/*
*	Creates a depth buffer.
*/
void RenderingSystem::CreateDepthBuffer(const Resolution resolution, const TextureFormat format, const SampleCount sample_count, DepthBufferHandle *const RESTRICT handle) const NOEXCEPT
{
	_SubRenderingSystem->CreateDepthBuffer(resolution, format, sample_count, handle);
}

/*
*	Destroys a depth buffer.
*/
void RenderingSystem::DestroyDepthBuffer(DepthBufferHandle *const RESTRICT handle) const NOEXCEPT
{
	_SubRenderingSystem->DestroyDepthBuffer(handle);
}

/*
*	Creates an event.
*/
void RenderingSystem::CreateEvent(EventHandle *const RESTRICT handle) NOEXCEPT
{
	_SubRenderingSystem->CreateEvent(handle);
}

/*
*	Resets an event.
*/
void RenderingSystem::ResetEvent(const EventHandle handle) NOEXCEPT
{
	_SubRenderingSystem->ResetEvent(handle);
}

/*
*	Waits for an event.
*/
void RenderingSystem::WaitForEvent(const EventHandle handle) NOEXCEPT
{
	_SubRenderingSystem->WaitForEvent(handle);
}

/*
*	Creates a query pool.
*/
void RenderingSystem::CreateQueryPool(const uint32 query_count, QueryPoolHandle *const RESTRICT handle) NOEXCEPT
{
	_SubRenderingSystem->CreateQueryPool(query_count, handle);
}

/*
*	Resets the given query pool.
*/
void RenderingSystem::ResetQueryPool(QueryPoolHandle *const RESTRICT handle) NOEXCEPT
{
	_SubRenderingSystem->ResetQueryPool(handle);
}

/*
*	Returns the execution time, in nanoseconds, from the given query pool.
*	Assumption being that the query pool has been used to record two timestamps into a command buffer that has completed.
*/
NO_DISCARD uint64 RenderingSystem::GetExecutionTime(const QueryPoolHandle query_pool, const uint32 query_index) NOEXCEPT
{
	return _SubRenderingSystem->GetExecutionTime(query_pool, query_index);
}

/*
*	Creates a Shader.
*/
void RenderingSystem::CreateShader(const ArrayProxy<byte> &data, const ShaderStage stage, ShaderHandle *const RESTRICT handle) const NOEXCEPT
{
	_SubRenderingSystem->CreateShader(data, stage, handle);
}

/*
*	Creates a render data table layout.
*/
void RenderingSystem::CreateRenderDataTableLayout(const RenderDataTableLayoutBinding *const RESTRICT bindings, const uint32 number_of_bindings, RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT
{
	_SubRenderingSystem->CreateRenderDataTableLayout(bindings, number_of_bindings, handle);
}

/*
*	Destroys a render data table layout.
*/
void RenderingSystem::DestroyRenderDataTableLayout(RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT
{
	_SubRenderingSystem->DestroyRenderDataTableLayout(handle);
}

/*
*	Creates a render data table.
*/
void RenderingSystem::CreateRenderDataTable(const RenderDataTableLayoutHandle render_data_table_layout, RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT
{
	_SubRenderingSystem->CreateRenderDataTable(render_data_table_layout, handle);
}

/*
*	Binds an acceleration structure to a render data table.
*/
void RenderingSystem::BindAccelerationStructureToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const AccelerationStructureHandle acceleration_structure) const NOEXCEPT
{
	_SubRenderingSystem->BindAccelerationStructureToRenderDataTable(binding, array_element, handle, acceleration_structure);
}

/*
*	Binds a combined image sampler to a render data table.
*	Accepts render target, texture 2D and texture cube handles.
*/
void RenderingSystem::BindCombinedImageSamplerToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const OpaqueHandle image, const SamplerHandle sampler) const NOEXCEPT
{
	_SubRenderingSystem->BindCombinedImageSamplerToRenderDataTable(binding, array_element, handle, image, sampler);
}

/*
*	Binds a sampled image to a render data table.
*	Accepts render target, texture 2D and texture cube handles.
*/
void RenderingSystem::BindSampledImageToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const OpaqueHandle image) const NOEXCEPT
{
	_SubRenderingSystem->BindSampledImageToRenderDataTable(binding, array_element, handle, image);
}

/*
*	Binds a sampler to a render data table.
*/
void RenderingSystem::BindSamplerToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const SamplerHandle sampler) const NOEXCEPT
{
	_SubRenderingSystem->BindSamplerToRenderDataTable(binding, array_element, handle, sampler);
}

/*
*	Binds a storage buffer to a render data table.
*/
void RenderingSystem::BindStorageBufferToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const BufferHandle buffer) const NOEXCEPT
{
	_SubRenderingSystem->BindStorageBufferToRenderDataTable(binding, array_element, handle, buffer);
}

/*
*	Binds a storage image to a render data table.
*/
void RenderingSystem::BindStorageImageToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const OpaqueHandle image) const NOEXCEPT
{
	_SubRenderingSystem->BindStorageImageToRenderDataTable(binding, array_element, handle, image);
}

/*
*	Binds a uniform buffer to a render data table.
*/
void RenderingSystem::BindUniformBufferToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const BufferHandle buffer) const NOEXCEPT
{
	_SubRenderingSystem->BindUniformBufferToRenderDataTable(binding, array_element, handle, buffer);
}

/*
*	Destroys a render data table.
*/
void RenderingSystem::DestroyRenderDataTable(RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT
{
	_SubRenderingSystem->DestroyRenderDataTable(handle);
}

/*
*	Creates a render target.
*/
void RenderingSystem::CreateRenderTarget(const Resolution resolution, const TextureFormat format, const SampleCount sample_count, RenderTargetHandle *const RESTRICT handle) const NOEXCEPT
{
	_SubRenderingSystem->CreateRenderTarget(resolution, format, sample_count, handle);
}

/*
*	Destroys a render target.
*/
void RenderingSystem::DestroyRenderTarget(RenderTargetHandle *const RESTRICT handle) const NOEXCEPT
{
	_SubRenderingSystem->DestroyRenderTarget(handle);
}

/*
*	Creates and returns a sampler.
*/
void RenderingSystem::CreateSampler(const SamplerProperties &properties, SamplerHandle *const RESTRICT handle) const NOEXCEPT
{
	_SubRenderingSystem->CreateSampler(properties, handle);
}

/*
*	Creates a texture 2D.
*/
void RenderingSystem::CreateTexture2D(const TextureData &data, Texture2DHandle *const RESTRICT handle) const NOEXCEPT
{
	_SubRenderingSystem->CreateTexture2D(data, handle);
}

/*
*	Updates a texture 2D.
*/
void RenderingSystem::UpdateTexture2D(const TextureDataContainer &texture_data_container, Texture2DHandle *const RESTRICT handle) NOEXCEPT
{
	_SubRenderingSystem->UpdateTexture2D(texture_data_container, handle);
}

/*
*	Destroys a texture 2D.
*/
void RenderingSystem::DestroyTexture2D(Texture2DHandle *const RESTRICT handle) const NOEXCEPT
{
	_SubRenderingSystem->DestroyTexture2D(handle);
}

/*
*	Creates a texture 3D.
*/
void RenderingSystem::CreateTexture3D(const TextureData &data, Texture3DHandle *const RESTRICT handle) const NOEXCEPT
{
	_SubRenderingSystem->CreateTexture3D(data, handle);
}

/*
*	Creates a texture cube.
*/
void RenderingSystem::CreateTextureCube(const TextureCubeData &data, TextureCubeHandle *const RESTRICT handle) const NOEXCEPT
{
	_SubRenderingSystem->CreateTextureCube(data, handle);
}

/*
*	Initializes a pipeline
*/
void RenderingSystem::InitializePipeline(Pipeline *const RESTRICT pipeline) const NOEXCEPT
{
	_SubRenderingSystem->InitializePipeline(pipeline);
}

/*
*	Terminates a pipeline.
*/
void RenderingSystem::TerminatePipeline(Pipeline *const RESTRICT pipeline) const NOEXCEPT
{
	_SubRenderingSystem->TerminatePipeline(pipeline);
}

/*
*	Terminates a render pass.
*/
void RenderingSystem::TerminateRenderPass(RenderPass *const RESTRICT render_pass) const NOEXCEPT
{
	for (Pipeline* const RESTRICT pipeline : render_pass->GetPipelines())
	{
		TerminatePipeline(pipeline);
	}
}

/*
*	Returns the global render data table.
*/
RenderDataTableHandle RenderingSystem::GetGlobalRenderDataTable() const NOEXCEPT
{
	return _GlobalRenderData._RenderDataTables[GetCurrentFramebufferIndex()];
}

/*
*	Returns the global render data table 2.
*/
RenderDataTableHandle RenderingSystem::GetGlobalRenderDataTable2() const NOEXCEPT
{
	return _GlobalRenderData._RenderDataTables2[GetCurrentFramebufferIndex()];
}

/*
*	Adds a texture to the global render data and returns it's index.
*/
uint32 RenderingSystem::AddTextureToGlobalRenderData(Texture2DHandle texture) NOEXCEPT
{
	//Lock global texture slots.
	_GlobalRenderData._GlobalTexturesLock.Lock();

	//Find the first available index and store it.
	uint32 index{ UINT32_MAXIMUM };

	for (uint32 i{ 0 }; i < CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES; ++i)
	{
		//If this is available, grab it!
		if (!_GlobalRenderData._GlobalTextureSlots[i])
		{
			index = i;
			_GlobalRenderData._GlobalTextureSlots[i] = true;

			break;
		}
	}

	ASSERT(index != UINT32_MAXIMUM, "If no index could be found, then, well... This is bad. ):");

	//Add the global texture updates.
	for (DynamicArray<Pair<uint32, Texture2DHandle>> &globalTextureUpdate : _GlobalRenderData._AddGlobalTextureUpdates)
	{
		globalTextureUpdate.Emplace(index, texture);
	}

	//Unlock the global texture slots.
	_GlobalRenderData._GlobalTexturesLock.Unlock();

	//Return the index.
	return index;
}

/*
*	Returns a texture to the global render data and marks it's index as available.
*/
void RenderingSystem::ReturnTextureToGlobalRenderData(const uint32 index) NOEXCEPT
{
	//Lock the global texture slots.
	_GlobalRenderData._GlobalTexturesLock.Lock();

	//Add the global texture updates.
	for (DynamicArray<uint32> &globalTextureUpdate : _GlobalRenderData._RemoveGlobalTextureUpdates)
	{
		globalTextureUpdate.Emplace(index);
	}

	//Mark the global texture slot as available.
	_GlobalRenderData._GlobalTextureSlots[index] = false;

	//Unlock the global texture slots.
	_GlobalRenderData._GlobalTexturesLock.Unlock();
}

/*
*	Returns the given common render data table layout.
*/
RenderDataTableLayoutHandle RenderingSystem::GetCommonRenderDataTableLayout(const CommonRenderDataTableLayout commonRenderDataTableLayout) const NOEXCEPT
{
	//Return the given common render data table layout.
	return _CommonRenderDataTableLayouts[UNDERLYING(commonRenderDataTableLayout)];
}

/*
*	Returns the given common render data table.
*/
RenderDataTableHandle RenderingSystem::GetCommonRenderDataTable(const CommonRenderDataTable common_render_data_table) const NOEXCEPT
{
	//Return the given common render data table layout.
	return _CommonRenderDataTables[UNDERLYING(common_render_data_table)];
}

/*
*	Takes an immedate screenshot and writes it to the given file path.
*/
void RenderingSystem::TakeImmediateScreenshot(const char *const RESTRICT file_path) NOEXCEPT
{
	_SubRenderingSystem->TakeImmediateScreenshot(file_path);
}

/*
*	Starts taking a screenshot.
*	This will pause the game, turn off denoising for all effects and start accumulating results across frames.
*	This can be used to, for example, take reference screenshots to tweak visual effects, or to take "prettier" screenshots of the game.
*/
void RenderingSystem::StartTakingScreenshot() NOEXCEPT
{
	//Pause the game.
	CatalystEngineSystem::Instance->SetUpdateSpeed(0.0f);

	//Signal to other systems that the rendering system is taking a screenshot.
	_IsTakingScreenshot = true;
}

/*
*	Returns if the rendering system is currently taking a screenshot.
*/
NO_DISCARD bool RenderingSystem::IsTakingScreenshot() NOEXCEPT
{
	return _IsTakingScreenshot;
}

/*
*	Stops taking a screenshot and writes the result to the given file path.
*/
void RenderingSystem::StopTakingScreenshot(const char *const RESTRICT file_path) NOEXCEPT
{
	//Signal to other systems that the rendering system is no longertaking a screenshot.
	_IsTakingScreenshot = false;

	//Take the screenshot.
	TakeImmediateScreenshot(file_path);

	//Unpause the game.
	CatalystEngineSystem::Instance->SetUpdateSpeed(1.0f);
}

/*
*	Adds a custom render pass to the default rendering path.
*/
void RenderingSystem::AddCustomDefaultRenderPass(const DefaultNativeRenderPassStage stage, RenderPass *const RESTRICT render_pass) NOEXCEPT
{
	for (uint64 i{ 0 }; i < _RenderPasses.Size(); ++i)
	{
		if (_RenderPasses[i]->GetDefaultNativeStage() == stage)
		{
			_RenderPasses.Insert(render_pass, i);

			render_pass->Initialize();

			break;
		}
	}
}

/*
*	Pre-initializes the global render data.
*/
void RenderingSystem::PreInitializeGlobalRenderData() NOEXCEPT
{
	//Get the number of frame buffers.
	const uint8 number_of_framebuffers{ GetNumberOfFramebuffers() };

	//Upsize the buffers.
	_GlobalRenderData._RenderDataTables.Upsize<false>(number_of_framebuffers);
	_GlobalRenderData._RenderDataTables2.Upsize<false>(number_of_framebuffers);
	_GlobalRenderData._DynamicUniformDataBuffers.Upsize<false>(number_of_framebuffers);
	_GlobalRenderData._RemoveGlobalTextureUpdates.Upsize<true>(number_of_framebuffers);
	_GlobalRenderData._AddGlobalTextureUpdates.Upsize<true>(number_of_framebuffers);
	_GlobalRenderData._GlobalCommandPoolData.Upsize<true>(number_of_framebuffers);
#if !defined(CATALYST_CONFIGURATION_FINAL)
	_GlobalRenderData._PerformanceData.Upsize<true>(number_of_framebuffers);
#endif

	for (uint8 i{ 0 }; i < number_of_framebuffers; ++i)
	{
		//Create the render data table.
		CreateRenderDataTable(GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL), &_GlobalRenderData._RenderDataTables[i]);
		CreateRenderDataTable(GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL_2), &_GlobalRenderData._RenderDataTables2[i]);

		//Create the dynamic uniform data buffer.
		CreateBuffer(sizeof(DynamicUniformData), BufferUsage::UniformBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible, &_GlobalRenderData._DynamicUniformDataBuffers[i]);

		//Bind the dynamic uniform data buffer to the render data table.
		BindUniformBufferToRenderDataTable(0, 0, &_GlobalRenderData._RenderDataTables[i], _GlobalRenderData._DynamicUniformDataBuffers[i]);
	
#if !defined(CATALYST_CONFIGURATION_FINAL)
		CreateQueryPool(128, &_GlobalRenderData._PerformanceData[i]._QueryPool);
#endif
	}

	//Mark all global texture slots as free.
	for (uint32 i{ 0 }; i < CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES; ++i)
	{
		_GlobalRenderData._GlobalTextureSlots[i] = false;
	}
}

/*
*	Initializes all render targets.
*/
void RenderingSystem::InitializeRenderTargets() NOEXCEPT
{
	//Initialize all render targets.
	CreateRenderTarget(GetScaledResolution(0), TextureFormat::RGBA_UINT8, SampleCount::SAMPLE_COUNT_1, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_UINT8)]);
	CreateRenderTarget(GetScaledResolution(1), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2)]);
	CreateRenderTarget(GetScaledResolution(3), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_EIGHTH)]);
	CreateRenderTarget(GetScaledResolution(4), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_SIXTEENTH)]);
	CreateRenderTarget(GetScaledResolution(5), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_THIRTYSECOND)]);
	CreateRenderTarget(GetScaledResolution(6), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_SIXTYFOURTH)]);
	CreateRenderTarget(GetScaledResolution(7), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HUNDREDTWENTYEIGHTH)]);
}

/*
*	Initializes all samplers.
*/
void RenderingSystem::InitializeSamplers() NOEXCEPT
{
	CreateSampler(SamplerProperties(TextureFilter::LINEAR, MipmapMode::LINEAR, AddressMode::CLAMP_TO_EDGE, 0), &_Samplers[UNDERLYING(Sampler::FilterLinear_MipmapModeLinear_AddressModeClampToEdge)]);
	CreateSampler(SamplerProperties(TextureFilter::LINEAR, MipmapMode::LINEAR, AddressMode::REPEAT, 8), &_Samplers[UNDERLYING(Sampler::FilterLinear_MipmapModeLinear_AddressModeRepeat)]);
	CreateSampler(SamplerProperties(TextureFilter::LINEAR, MipmapMode::NEAREST, AddressMode::CLAMP_TO_EDGE, 0), &_Samplers[UNDERLYING(Sampler::FilterLinear_MipmapModeNearest_AddressModeClampToEdge)]);
	CreateSampler(SamplerProperties(TextureFilter::LINEAR, MipmapMode::NEAREST, AddressMode::REPEAT, 0), &_Samplers[UNDERLYING(Sampler::FilterLinear_MipmapModeNearest_AddressModeRepeat)]);
	CreateSampler(SamplerProperties(TextureFilter::NEAREST, MipmapMode::NEAREST, AddressMode::CLAMP_TO_EDGE, 0), &_Samplers[UNDERLYING(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge)]);
	CreateSampler(SamplerProperties(TextureFilter::NEAREST, MipmapMode::NEAREST, AddressMode::REPEAT, 0), &_Samplers[UNDERLYING(Sampler::FilterNearest_MipmapModeNearest_AddressModeRepeat)]);
}

/*
*	Initializes all common render data table layouts.
*/
void RenderingSystem::InitializeCommonRenderDataTableLayouts() NOEXCEPT
{
	//Initialize the CLOUDS render data table layout.
	{
		constexpr StaticArray<RenderDataTableLayoutBinding, 1> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::FRAGMENT)
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::CLOUDS)]);
	}

	{
		//Initialize the dynamic uniform data render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 8> bindings
		{
			//Global uniform data.
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::COMPUTE | ShaderStage::FRAGMENT | ShaderStage::GEOMETRY | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_GENERATION | ShaderStage::RAY_MISS | ShaderStage::VERTEX),
			
			//Global textures.
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::SampledImage, CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES, ShaderStage::COMPUTE | ShaderStage::FRAGMENT | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_GENERATION | ShaderStage::VERTEX),
			
			//Global samplers.
			RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::Sampler, UNDERLYING(Sampler::NumberOfSamplers), ShaderStage::COMPUTE | ShaderStage::FRAGMENT | ShaderStage::GEOMETRY | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_GENERATION | ShaderStage::VERTEX),
			
			//Global materials.
			RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::FRAGMENT | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_GENERATION | ShaderStage::VERTEX),

			//Blue noise textures.
			RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::CombinedImageSampler, CatalystShaderConstants::NUMBER_OF_BLUE_NOISE_TEXTURES, ShaderStage::COMPUTE | ShaderStage::FRAGMENT | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_MISS | ShaderStage::RAY_GENERATION),

			//Sky texture.
			RenderDataTableLayoutBinding(5, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::FRAGMENT | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_GENERATION | ShaderStage::RAY_MISS),

			//Star texture.
			RenderDataTableLayoutBinding(6, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::FRAGMENT | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_GENERATION | ShaderStage::RAY_MISS),

			//Hammersley hemisphere samples uniform buffer.
			RenderDataTableLayoutBinding(7, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::FRAGMENT | ShaderStage::RAY_GENERATION),
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::GLOBAL)]);
	}

	{
		//Initialize the dynamic uniform data render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 3> bindings
		{
			//Global textures.
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::SampledImage, CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES, ShaderStage::COMPUTE | ShaderStage::FRAGMENT | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_GENERATION | ShaderStage::VERTEX),

			//Global materials.
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::FRAGMENT | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_GENERATION | ShaderStage::VERTEX),
		
			//Blue noise textures.
			RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::CombinedImageSampler, CatalystShaderConstants::NUMBER_OF_BLUE_NOISE_TEXTURES, ShaderStage::COMPUTE | ShaderStage::FRAGMENT | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_MISS | ShaderStage::RAY_GENERATION),
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::GLOBAL_2)]);
	}

	{
		//Initialize the particle system data render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 1> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageBuffer, 1, ShaderStage::COMPUTE)
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::PARTICLE_SYSTEM)]);
	}

	{
		//Initialize the user interface data render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 1> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::VERTEX | ShaderStage::FRAGMENT)
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::USER_INTERFACE)]);
	}
}

/*
*	Initializes all common render data tables.
*/
void RenderingSystem::InitializeCommonRenderDataTables() NOEXCEPT
{
	//Initialize the CLOUDS common render data table.
	{
		CreateRenderDataTable(GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::CLOUDS), &_CommonRenderDataTables[UNDERLYING(CommonRenderDataTable::CLOUDS)]);
	}
}

/*
*	Initializes the default texture.
*/
void RenderingSystem::InitializeDefaultTexture() NOEXCEPT
{
	//Create the default texture 2D.
	StaticArray<byte, 4> default_texture_2d_data{ static_cast<byte>(255), static_cast<byte>(0), static_cast<byte>(0), static_cast<byte>(255) };
	CreateTexture2D(TextureData(TextureDataContainer(default_texture_2d_data.Data(), 1, 1, 1, 4), TextureFormat::RGBA_UINT8, TextureUsage::NONE, false), &_DefaultTexture2D);
}

/*
*	Initializes the Hammersley hemisphere samples uniform buffer.
*/
void RenderingSystem::InitializeHammersleyHemisphereSamplesUniformBuffer() NOEXCEPT
{
	//Calculate the hemisphere samples.
	StaticArray<Vector4<float32>, 64> hemisphere_samples;

	uint8 counter{ 0 };

	for (uint8 i{ 0 }; i < 8; ++i)
	{
		for (uint8 j{ 0 }; j < 8; ++j)
		{
			hemisphere_samples[counter] = Vector4<float32>(HammersleySequence::CalculateCoordinateHemisphereCosinus(i + j * 8 + 1, 65), HammersleySequence::RadicalInverse(counter + 1));
			_HammersleyHemisphereSamplesUniformData._HammersleyCosinusSamples[counter] = hemisphere_samples[counter];

			++counter;
		}
	}

	//Create the buffer and upload the data.
	const void* const RESTRICT data_chunks[]{ hemisphere_samples.Data() };
	const uint64 data_sizes[]{ hemisphere_samples.Size() * sizeof(Vector4<float32>) };
	CreateBuffer(data_sizes[0], BufferUsage::UniformBuffer, MemoryProperty::DeviceLocal, &_HammersleyHemisphereSamplesUniformBuffer);
	UploadDataToBuffer(data_chunks, data_sizes, 1, &_HammersleyHemisphereSamplesUniformBuffer);
}

/*
*	Post-initializes the global render data.
*/
void RenderingSystem::PostInitializeGlobalRenderData() NOEXCEPT
{
	for (uint8 i{ 0 }; i < GetNumberOfFramebuffers(); ++i)
	{
		//Bind some default texture to the global textures, because... Validation layers tells me I need to do this. (:
		for (uint32 j{ 0 }; j < CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES; ++j)
		{
			BindSampledImageToRenderDataTable(1, j, &_GlobalRenderData._RenderDataTables[i], _DefaultTexture2D);
			BindSampledImageToRenderDataTable(0, j, &_GlobalRenderData._RenderDataTables2[i], _DefaultTexture2D);
		}

		//Bind all the samplers to the render data table.
		for (uint32 j{ 0 }; j < UNDERLYING(Sampler::NumberOfSamplers); ++j)
		{
			BindSamplerToRenderDataTable(2, j, &_GlobalRenderData._RenderDataTables[i], _Samplers[j]);
		}

		//Bind the Hammersley hemisphere samples uniform buffer.
		BindUniformBufferToRenderDataTable(7, 0, &_GlobalRenderData._RenderDataTables[i], _HammersleyHemisphereSamplesUniformBuffer);
	}

	//Bind all the blue noise textures to the global render data tables.
	for (uint8 i{ 0 }; i < CatalystShaderConstants::NUMBER_OF_BLUE_NOISE_TEXTURES; ++i)
	{
		//Create the identifier.
		char buffer[32];
		sprintf_s(buffer, "Blue_Noise_%u_Texture2D", i);
		const HashString identifier{ buffer };

		//Retrieve the texture 2D handle.
		const Texture2DHandle texture_2D_handle{ ResourceSystem::Instance->GetTexture2DResource(identifier)->_Texture2DHandle };

		//Bind the texture to the global render data tables.
		for (uint8 j{ 0 }; j < GetNumberOfFramebuffers(); ++j)
		{
			BindCombinedImageSamplerToRenderDataTable(4, i, &_GlobalRenderData._RenderDataTables[j], texture_2D_handle, GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeRepeat));
			BindCombinedImageSamplerToRenderDataTable(2, i, &_GlobalRenderData._RenderDataTables2[j], texture_2D_handle, GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeRepeat));
		}
	}

	//Initialize the global command pool data.
	for (GlobalCommandPoolData &global_command_pool_data : _GlobalRenderData._GlobalCommandPoolData)
	{
		//Allocate enough thread data for all threads (main thread + task executor threads).
		global_command_pool_data._ThreadData.Upsize<true>(1 + TaskSystem::Instance->GetNumberOfTaskExecutors());

		//Initialize all thread data.
		for (GlobalCommandPoolData::ThreadData &thread_data : global_command_pool_data._ThreadData)
		{
			//Create the command pool.
			CreateCommandPool(Pipeline::Type::COMPUTE, &thread_data._CommandPool);
		}
	}
}

/*
*	Post-initializes the common render data tables.
*/
void RenderingSystem::PostInitializeCommonRenderDataTables() NOEXCEPT
{
	//Bind the clouds texture to the CLOUDS common render data table.
#if defined(CATALYST_INCLUDE_ENVIRONMENT_RESOURCE_COLLECTION)
	BindCombinedImageSamplerToRenderDataTable(0, 0, &_CommonRenderDataTables[UNDERLYING(CommonRenderDataTable::CLOUDS)], ResourceSystem::Instance->GetTexture3DResource(HashString("Cloud_Texture3D"))->_Texture3DHandle, RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeRepeat));
#else
	BindCombinedImageSamplerToRenderDataTable(0, 0, &_CommonRenderDataTables[UNDERLYING(CommonRenderDataTable::CLOUDS)], ResourceSystem::Instance->GetTexture3DResource(HashString("Catalyst_Engine_Default_Texture_3D"))->_Texture3DHandle, RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeRepeat));
#endif
}

/*
*	Updates the global render data.
*/
void RenderingSystem::UpdateGlobalRenderData() NOEXCEPT
{
	//Retrieve the current frame buffer index.
	const uint8 current_framebuffer_index{ GetCurrentFramebufferIndex() };

	//Update the global uniform data.
	UpdateGlobalUniformData(current_framebuffer_index);

	//Update the global textures.
	UpdateGlobalTextures(current_framebuffer_index);

	//Update global materials.
	UpdateGlobalMaterials(current_framebuffer_index);

	//Update the global command pool data.
	UpdateGlobalCommandPoolData(current_framebuffer_index);

	//Bind the sky texture.
	if (WorldSystem::Instance->GetSkySystem()->GetSkyTexture())
	{
		BindCombinedImageSamplerToRenderDataTable(5, 0, &_GlobalRenderData._RenderDataTables[current_framebuffer_index], WorldSystem::Instance->GetSkySystem()->GetSkyTexture()->_TextureCubeHandle, RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeLinear_AddressModeClampToEdge));
	}

#if defined(CATALYST_INCLUDE_ENVIRONMENT_RESOURCE_COLLECTION)
	BindCombinedImageSamplerToRenderDataTable(6, 0, &_GlobalRenderData._RenderDataTables[current_framebuffer_index], ResourceSystem::Instance->GetTextureCubeResource(HashString("Catalyst_Engine_Star_TextureCube"))->_TextureCubeHandle, RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeLinear_AddressModeClampToEdge));
#else
	BindCombinedImageSamplerToRenderDataTable(6, 0, &_GlobalRenderData._RenderDataTables[current_framebuffer_index], ResourceSystem::Instance->GetTextureCubeResource(HashString("Catalyst_Engine_Default_TextureCube"))->_TextureCubeHandle, RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeLinear_AddressModeClampToEdge));
#endif
}

/*
*	Updates the global render data.
*/
void RenderingSystem::UpdateGlobalUniformData(const uint8 current_framebuffer_index) NOEXCEPT
{
	//Update the previous camera world transform.
	_PreviousCameraWorldTransform = _CurrentCameraWorldTransform;
	_CurrentCameraWorldTransform = _CameraSystem.GetCurrentCamera()->GetWorldTransform();

	//Calculate the previous and current camera matrices, as well as their inverses.
	const Matrix4x4 previous_camera_matrix{ Matrix4x4::LookAt(_PreviousCameraWorldTransform.GetRelativePosition(_CurrentCameraWorldTransform.GetCell()), _PreviousCameraWorldTransform.GetRelativePosition(_CurrentCameraWorldTransform.GetCell()) + CatalystCoordinateSpacesUtilities::RotatedWorldForwardVector(_PreviousCameraWorldTransform.GetRotation().ToEulerAngles()), CatalystCoordinateSpacesUtilities::RotatedWorldUpVector(_PreviousCameraWorldTransform.GetRotation().ToEulerAngles())) };

	//Update matrices.
	_DynamicUniformData._PreviousWorldToClipMatrix = *_CameraSystem.GetCurrentCamera()->GetProjectionMatrix() * previous_camera_matrix;
	_DynamicUniformData._InverseWorldToCameraMatrix = *_CameraSystem.GetCurrentCamera()->GetInverseCameraMatrix();
	_DynamicUniformData._InverseCameraToClipMatrix = *_CameraSystem.GetCurrentCamera()->GetInverseProjectionMatrix();
	_DynamicUniformData._WorldToCameraMatrix = *_CameraSystem.GetCurrentCamera()->GetCameraMatrix();
	_DynamicUniformData._WorldToClipMatrix = *_CameraSystem.GetCurrentCamera()->GetViewMatrix();
	
	//Rotate the relevant matrices to fit the surface transform.
	{
		const Matrix4x4 rotation_matrix{ GetCurrentSurfaceTransformRotationMatrix() };

		_DynamicUniformData._PreviousWorldToClipMatrix = rotation_matrix * _DynamicUniformData._PreviousWorldToClipMatrix;
		_DynamicUniformData._UserInterfaceMatrix = rotation_matrix;
		_DynamicUniformData._WorldToClipMatrix = rotation_matrix * _DynamicUniformData._WorldToClipMatrix;
	}

	//Update vectors.
	_DynamicUniformData._SkyLightRadiance = VectorConstants::ZERO;
	_DynamicUniformData._SkyLightDirection = CatalystWorldCoordinateSpace::DOWN;

	{
		const uint64 number_of_light_components{ ComponentManager::GetNumberOfLightComponents() };
		const LightComponent* RESTRICT component{ ComponentManager::GetLightLightComponents() };

		for (uint64 i{ 0 }; i < number_of_light_components; ++i, ++component)
		{
			if (component->_LightType == LightType::DIRECTIONAL)
			{
				_DynamicUniformData._SkyLightRadiance = Vector4<float32>(component->_Color, component->_Intensity);
				_DynamicUniformData._SkyLightDirection = CatalystCoordinateSpacesUtilities::RotatedWorldDownVector(component->_Rotation);

				break;
			}
		}
	}

	_DynamicUniformData._CameraForwardVector = _CameraSystem.GetCurrentCamera()->GetForwardVector();
	_DynamicUniformData._CameraWorldPosition = _CameraSystem.GetCurrentCamera()->GetWorldTransform().GetLocalPosition();

	_DynamicUniformData._UpperSkyColor = WorldSystem::Instance->GetSkySystem()->GetSkyGradient()._UpperSkyColor;
	_DynamicUniformData._LowerSkyColor = WorldSystem::Instance->GetSkySystem()->GetSkyGradient()._LowerSkyColor;

	_DynamicUniformData._CurrentWorldGridCell = WorldSystem::Instance->GetCurrentWorldGridCell();

	_DynamicUniformData._ScaledResolution = Vector2<float32>(static_cast<float32>(GetScaledResolution(0)._Width), static_cast<float32>(GetScaledResolution(0)._Height));
	_DynamicUniformData._InverseScaledResolution = 1.0f / Vector2<float32>(static_cast<float32>(GetScaledResolution(0)._Width), static_cast<float32>(GetScaledResolution(0)._Height));
	_DynamicUniformData._PreviousFramejitter = _DynamicUniformData._CurrentFrameJitter;
	_DynamicUniformData._CurrentFrameJitter = _CameraSystem.GetCurrentCamera()->GetProjectionMatrixJitter();

	_DynamicUniformData._HeightMapCoordinateOffset._X = 0.0f;
	_DynamicUniformData._HeightMapCoordinateOffset._Y = 0.0f;

	//Update floats.
	_DynamicUniformData._DeltaTime = CatalystEngineSystem::Instance->GetDeltaTime();
	_DynamicUniformData._GlobalRandomSeed1 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._GlobalRandomSeed2 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._GlobalRandomSeed3 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._TotalTime = CatalystEngineSystem::Instance->GetTotalTime();

	_DynamicUniformData._AspectRatio = static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Width) / static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Height);

	_DynamicUniformData._CurrentBlueNoiseTextureIndex = _CurrentBlueNoiseTextureIndex;
	_DynamicUniformData._CurrentBlueNoiseTextureOffsetX = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._CurrentBlueNoiseTextureOffsetY = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._ViewDistance = _Configuration._ViewDistance;
	_DynamicUniformData._MaximumSkyTextureMipmapLevel = WorldSystem::Instance->GetSkySystem()->GetSkyTexture() ? static_cast<float32>(WorldSystem::Instance->GetSkySystem()->GetSkyTexture()->_MipmapLevels) : 1.0f;
	_DynamicUniformData._Wetness = WorldSystem::Instance->GetWetness();
	_DynamicUniformData._NearPlane = _CameraSystem.GetCurrentCamera()->GetNearPlane();
	_DynamicUniformData._FarPlane = _CameraSystem.GetCurrentCamera()->GetFarPlane();
	_DynamicUniformData._CameraAbsoluteHeight = _CurrentCameraWorldTransform.GetAbsolutePosition()._Y;

	_DynamicUniformData._SkyMode = static_cast<uint32>(WorldSystem::Instance->GetSkySystem()->GetSkyMode());
	_DynamicUniformData._SkyIntensity = WorldSystem::Instance->GetSkySystem()->GetSkyIntensity();
	_DynamicUniformData._StarIntensity = WorldSystem::Instance->GetSkySystem()->GetStarIntensity();

	_DynamicUniformData._VolumetricLightingDistance = WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Distance;
	_DynamicUniformData._VolumetricLightingHeight = WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Height;
	_DynamicUniformData._VolumetricLightingThickness = WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Thickness;

	_DynamicUniformData._WorldGridSize = WorldSystem::Instance->GetWorldGridSize();

	void *const RESTRICT dataChunks[]{ &_DynamicUniformData };
	const uint64 dataSizes[]{ sizeof(DynamicUniformData) };

	UploadDataToBuffer(dataChunks, dataSizes, 1, &_GlobalRenderData._DynamicUniformDataBuffers[current_framebuffer_index]);

	//Update the active noise texture index.
	_CurrentBlueNoiseTextureIndex = _CurrentBlueNoiseTextureIndex == CatalystShaderConstants::NUMBER_OF_BLUE_NOISE_TEXTURES - 1 ? 0 : _CurrentBlueNoiseTextureIndex + 1;
}

/*
*	Updates the global textures.
*/
void RenderingSystem::UpdateGlobalTextures(const uint8 current_framebuffer_index) NOEXCEPT
{
	//Lock the global textures.
	_GlobalRenderData._GlobalTexturesLock.Lock();

	for (const uint32 update : _GlobalRenderData._RemoveGlobalTextureUpdates[current_framebuffer_index])
	{
		BindSampledImageToRenderDataTable(1, update, &_GlobalRenderData._RenderDataTables[current_framebuffer_index], _DefaultTexture2D);
		BindSampledImageToRenderDataTable(0, update, &_GlobalRenderData._RenderDataTables2[current_framebuffer_index], _DefaultTexture2D);
	}

	_GlobalRenderData._RemoveGlobalTextureUpdates[current_framebuffer_index].Clear();

	for (Pair<uint32, Texture2DHandle> &update : _GlobalRenderData._AddGlobalTextureUpdates[current_framebuffer_index])
	{
		BindSampledImageToRenderDataTable(1, update._First, &_GlobalRenderData._RenderDataTables[current_framebuffer_index], update._Second);
		BindSampledImageToRenderDataTable(0, update._First, &_GlobalRenderData._RenderDataTables2[current_framebuffer_index], update._Second);
	}

	_GlobalRenderData._AddGlobalTextureUpdates[current_framebuffer_index].Clear();

	//Unlock the global textures.
	_GlobalRenderData._GlobalTexturesLock.Unlock();
}

/*
*	Updates the global materials.
*/
void RenderingSystem::UpdateGlobalMaterials(const uint8 current_framebuffer_index) NOEXCEPT
{
	//Bind the current global material uniform buffer to the global render data table.
	BindUniformBufferToRenderDataTable(3, 0, &_GlobalRenderData._RenderDataTables[current_framebuffer_index], _MaterialSystem.GetCurrentMaterialUnifomBuffer());
	BindUniformBufferToRenderDataTable(1, 0, &_GlobalRenderData._RenderDataTables2[current_framebuffer_index], _MaterialSystem.GetCurrentMaterialUnifomBuffer());
}

/*
*	Updates the global command pool data.
*/
void RenderingSystem::UpdateGlobalCommandPoolData(const uint8 current_framebuffer_index) NOEXCEPT
{
	for (GlobalCommandPoolData::ThreadData &thread_data : _GlobalRenderData._GlobalCommandPoolData[current_framebuffer_index]._ThreadData)
	{
		//Reset the command pool.
		ResetCommandPool(&thread_data._CommandPool);

		//Reset the number of primary command buffers used.
		thread_data._NumberOfPrimaryCommandBuffersUsed = 0;

		//Reset the number of secondary command buffers used.
		thread_data._NumberOfSecondaryCommandBuffersUsed = 0;
	}
}