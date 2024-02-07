#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>
#include <Core/General/HashString.h>
#include <Core/General/Optional.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Resolution.h>

//Sound.
#include <Sound/SoundCore.h>

//World.
#include <World/Core/WorldPosition.h>

class CatalystProjectGeneralConfiguration final
{

public:

	/*
	*	The project name.
	*	Will be the name of the window.
	*/
	DynamicString _ProjectName;

	/*
	*	The common initialize function.
	*/
	void(*_CommonInitializeFunction)();

	/*
	*	The editor initialize function.
	*/
	void(*_EditorInitializeFunction)();

	/*
	*	The game initialize function.
	*/
	void(*_GameInitializeFunction)();

	/*
	*	The common post initialize function.
	*/
	void(*_CommonPostInitializeFunction)();

	/*
	*	The editor post initialize function.
	*/
	void(*_EditorPostInitializeFunction)();

	/*
	*	The game post initialize function.
	*/
	void(*_GamePostInitializeFunction)();

	/*
	*	The start game function.
	*/
	void(*_StartGameFunction)();

	/*
	*	The end game function.
	*/
	void(*_EndGameFunction)();

	/*
	*	The terminate function.
	*/
	void(*_TerminateFunction)();

	/*
	*	Default constructor.
	*/
	CatalystProjectGeneralConfiguration() NOEXCEPT
	{

	}

};

class CatalystProjectConcurrencyConfiguration final
{

public:

	/*
	*	Default constructor.
	*/
	CatalystProjectConcurrencyConfiguration() NOEXCEPT
	{

	}

	/*
	*	Details the number of task executors.
	*	This can be used to override the number of task executors,
	*	for example to save some processing power if you know that your game won't use that many async operations
	*	Default power is to run as many task executors as there are hardware threads.
	*/
	Optional<uint32> _NumberOfTaskExecutors;

};

class CatalystProjectInputConfiguration final
{

public:

	/*
	*	The number of supported gamepads.
	*/
	OptionalWithDefault<uint8> _NumberOfSupportedGamepads{ 1 };

	/*
	*	Default constructor.
	*/
	CatalystProjectInputConfiguration() NOEXCEPT
	{

	}

};

class CatalystProjectRenderingConfiguration final
{

public:

	/*
	*	Determines the sub rendering system.
	*/
	OptionalWithDefault<SubRenderingSystem> _SubRenderingSystem{ SubRenderingSystem::VULKAN };

	/*
	*	The initial rendering path.
	*/
#if defined(CATALYST_PLATFORM_ANDROID)
	OptionalWithDefault<RenderingPath> _InitialRenderingPath{ RenderingPath::MOBILE };
#elif defined(CATALYST_PLATFORM_OCULUS_QUEST)
	OptionalWithDefault<RenderingPath> _InitialRenderingPath{ RenderingPath::VIRTUAL_REALITY };
#elif defined(CATALYST_PLATFORM_WINDOWS)
	OptionalWithDefault<RenderingPath> _InitialRenderingPath{ RenderingPath::DEFAULT };
#endif

	/*
	*	The custom rendering path callback.
	*/
	OptionalWithDefault<CustomRenderingPathCallback> _CustomRenderingPathCallback{ nullptr };

	/*
	*	Denotes the focused refresh rate, denoted in milliseconds. Set to <= 0.0f if it doesn't matter.
	*/
	OptionalWithDefault<float32> _FocusedRefreshRate{ 120.0f };

	/*
	*	Denotes the unfocused refresh rate, denoted in milliseconds. Set to <= 0.0f if it doesn't matter.
	*/
	OptionalWithDefault<float32> _UnfocusedRefreshRate{ 60.0f };

	/*
	*	Denotes if the game should start in fullscreen initially.
	*	Recommended: true/false.
	*/
#if defined(CATALYST_EDITOR)
	OptionalWithDefault<bool> _InitialFullScreen{ false };
#else
	OptionalWithDefault<bool> _InitialFullScreen{ true };
#endif

	/*
	*	The resolution.
	*/
	Optional<Resolution> _Resolution;

	/*
	*	The resolution scale.
	*/
	OptionalWithDefault<float32> _ResolutionScale{ 1.0f };

	/*
	*	The view distance.
	*/
	OptionalWithDefault<float32> _ViewDistance{ 1'024.0f };

	/*
	*	The shadow map resolution.
	*/
	OptionalWithDefault<uint32> _ShadowMapResolution{ 1'024 };

	/*
	*	Default constructor.
	*/
	CatalystProjectRenderingConfiguration() NOEXCEPT
	{

	}

};

class CatalystProjectSoundConfiguration final
{

public:

	/*
	*	The sound system mode.
	*/
	OptionalWithDefault<SoundSubSystemType> _SoundSubSystemType{ SoundSubSystemType::DEFAULT };

	/*
	*	If set, the sound system will open this device. Otherwise, it defaults to the default audio device.
	*/
	Optional<DynamicString> _AudioDevice;

	/*
	*	Default constructor.
	*/
	CatalystProjectSoundConfiguration() NOEXCEPT
	{

	}

};

class CatalystProjectWorldConfiguration final
{

public:

	/*
	*	The world grid size.
	*	The world is divided into a three-dimensional grid in the Catalyst Engine™ to enable infinite worlds. 
	*	This is the size of each grid cell.
	*/
	OptionalWithDefault<float32> _WorldGridSize{ 1'024.0f };

	/*
	*	Default constructor.
	*/
	CatalystProjectWorldConfiguration() NOEXCEPT
	{

	}

};

class CatalystProjectConfiguration final
{

public:

	//The project general configuration.
	CatalystProjectGeneralConfiguration _GeneralConfiguration;

	//The project concurrency configuration.
	CatalystProjectConcurrencyConfiguration _ConcurrencyConfiguration;

	//The project input configuration.
	CatalystProjectInputConfiguration _InputConfiguration;

	//The project rendering configuration.
	CatalystProjectRenderingConfiguration _RenderingConfiguration;

	//The project sound configuration.
	CatalystProjectSoundConfiguration _SoundConfiguration;

	//The project world configuration.
	CatalystProjectWorldConfiguration _WorldConfiguration;

};