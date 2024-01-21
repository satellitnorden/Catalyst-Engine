#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//Concurrency.
#include <Concurrency/Task.h>

class ALIGN(8) ComponentSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ComponentSystem);

	/*
	*	Default constructor.
	*/
	ComponentSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the component system.
	*/
	void Initialize() NOEXCEPT;

private:

	/*
	*	Update data class definition.
	*/
	class UpdateData final
	{

	public:

		//The task.
		Task _Task;

		//The component.
		Component *RESTRICT _Component;

		//The update phase.
		UpdatePhase _UpdatePhase;

		//The start index.
		uint64 _StartIndex;

		//The end index.
		uint64 _EndIndex;

	};

	//The update data.
	DynamicArray<UpdateData> _UpdateData;

	/*
	*	Updates components for the given update phase.
	*/
	void UpdateComponents(const UpdatePhase update_phase) NOEXCEPT;

	/*
	*	Updates a component with the given update data.
	*/
	void UpdateComponent(const UpdateData &update_data) NOEXCEPT;

};