#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/AtomicQueue.h>

//Input.
#include <Input/InputCore.h>

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
		SYSTEM_INITIALIZE()
		SYSTEM_UPDATE(RANGE(USER_INTERFACE, RUN_ON_MAIN_THREAD))
	);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE UISystem() NOEXCEPT
	{

	}

	/*
	*	Sets the reference resolution.
	*/
	void SetReferenceResolution(const Vector2<float32> value) NOEXCEPT;

	/*
	*	Adds a UI scene of the given type.
	*/
	template <typename TYPE>
	FORCE_INLINE TYPE *const RESTRICT AddScene() NOEXCEPT
	{
		TYPE *const RESTRICT scene{ new TYPE() };
		scene->SetReferenceResolution(_ReferenceResolution);
		_AddSceneRequests.Push(scene);
		return scene;
	}

	/*
	*	Removes the given UI scene.
	*/
	FORCE_INLINE void RemoveScene(UI::Scene *const RESTRICT scene) NOEXCEPT
	{
		_RemoveSceneRequests.Push(scene);
	}

	/*
	*	Updates the UI system.
	*/
	void Update(const float32 delta_time, const MouseState *const RESTRICT mouse_state) NOEXCEPT;

	/*
	*	Returns the render commands.
	*/
	FORCE_INLINE NO_DISCARD const DynamicArray<UI::RenderCommand> &GetRenderCommands() const NOEXCEPT
	{
		return _RenderCommands;
	}

private:

	//The reference resolution.
	Vector2<float32> _ReferenceResolution{ 1'920.0f, 1'080.0f };

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
	void UpdateWidgets(const MouseState *const RESTRICT mouse_state) NOEXCEPT;

	/*
	*	Returns if any widget is blocking the given position for the given widget.
	*/
	NO_DISCARD bool AnyWidgetBlockingPosition(const Vector2<float32> position, const UI::Widget *const RESTRICT widget) NOEXCEPT;

	/*
	*	Updates the state of the given clickable interface with the given parameters.
	*	Returns if the mouse input was consumed.
	*/
	NO_DISCARD bool UpdateClickableInterfaceState
	(
		UI::Widget *const RESTRICT widget,
		UI::ClickableInterface *const RESTRICT clickable_interface,
		const ButtonState mouse_button_state,
		const bool is_inside,
		bool mouse_input_consumed
	) NOEXCEPT;

};