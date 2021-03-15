//Header file.
#include <Systems/ProfilingSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Systems.
#include <Systems/ResourceSystem.h>
#include <Systems/UserInterfaceSystem.h>

//User interface.
#include <UserInterface/TextUserInterfacePrimitive.h>
#include <UserInterface/TextUserInterfacePrimitiveDescription.h>

//Profiling system data.
namespace ProfilingSystemData
{

	/*
	*	Profiling entry class definition.
	*/
	class ProfilingEntry final
	{

	public:

		/*
		*	Default constructor, prohibited - must be constructed with proper arguments..
		*/
		FORCE_INLINE ProfilingEntry() NOEXCEPT = delete;

		/*
		*	Constructor taking all values as arguments.
		*/
		FORCE_INLINE ProfilingEntry(const DynamicString &initial_name, const float initial_duration) NOEXCEPT
			:
			_Name(initial_name),
			_Duration(initial_duration)
		{

		}

		//The name.
		DynamicString _Name;

		//The duration.
		float _Duration;

	};

	//Container for all profiling entries.
	DynamicArray<ProfilingEntry> _ProfilingEntries;

	//Container for all user interface primitives for all profiling entries.
	DynamicArray<UserInterfacePrimitive *RESTRICT> _UserInterfacePrimitives;

}

/*
*	Updates the profiling system during the physics update phase.
*/
void ProfilingSystem::PhysicsUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	
}

/*
*	Updates the profiling system during the pre update phase.
*/
void ProfilingSystem::PostUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Clear all profiling entries and their associated data.
	ProfilingSystemData::_ProfilingEntries.Clear();

	for (UserInterfacePrimitive *const RESTRICT primitive : ProfilingSystemData::_UserInterfacePrimitives)
	{
		UserInterfaceSystem::Instance->DestroyUserInterfacePrimitive(primitive);
	}

	ProfilingSystemData::_UserInterfacePrimitives.Clear();
}

/*
*	Adds a profiling entry.
*/
void ProfilingSystem::AddProfilingEntry(const DynamicString &name, const float duration) NOEXCEPT
{
	//Add the profiling entry.
	ProfilingSystemData::_ProfilingEntries.Emplace(name, duration);
}