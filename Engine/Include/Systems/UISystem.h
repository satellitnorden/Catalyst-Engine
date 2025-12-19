#if defined(USE_NEW_UI_SYSTEM)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/AtomicQueue.h>

//UI.
#include <UI/Core/RenderCommand.h>
#include <UI/Core/Scene.h>
#include <UI/Core/WidgetAllocator.h>

//Systems.
#include <Systems/System.h>

class UISystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		UISystem,
		SYSTEM_DEFINED_REQUIREMENT(USE_NEW_UI_SYSTEM)
		SYSTEM_INITIALIZE()
		SYSTEM_UPDATE(RANGE(USER_INTERFACE, RUN_ON_MAIN_THREAD), RANGE(PRE_RENDER, RENDER))
	);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE UISystem() NOEXCEPT
	{

	}

	/*
	*	Adds a UI scene of the given type.
	*/
	template <typename TYPE>
	FORCE_INLINE void AddScene() NOEXCEPT
	{
		UI::Scene *const RESTRICT scene{ new TYPE() };
		_AddSceneRequests.Push(scene);
	}

	/*
	*	Removes the given UI scene.
	*/
	FORCE_INLINE void RemoveScene(UI::Scene *const RESTRICT scene) NOEXCEPT
	{
		_RemoveSceneRequests.Push(scene);
	}

private:

	//The widget allocator.
	UI::WidgetAllocator _WidgetAllocator;

	//The add scene requests.
	AtomicQueue<UI::Scene *RESTRICT, 64, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _AddSceneRequests;

	//The remove scene requests.
	AtomicQueue<UI::Scene *RESTRICT, 64, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _RemoveSceneRequests;

	//The scenes.
	DynamicArray<UI::Scene *RESTRICT> _Scenes;

	//The render commands.
	DynamicArray<UI::RenderCommand> _RenderCommands;

	/*
	*	Updates widgets.
	*/
	void UpdateWidgets() NOEXCEPT;

	/*
	*	Updates during the USER_INTERFACE phase.
	*/
	void UpdateUserInterface() NOEXCEPT;

	/*
	*	Updates during the PRE_RENDER phase.
	*/
	void UpdatePreRender() NOEXCEPT;

};
#endif