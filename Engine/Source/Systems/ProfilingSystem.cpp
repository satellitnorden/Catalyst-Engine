//Header file.
#include <Systems/ProfilingSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Resources.
#include <Resources/Loading/ResourceLoader.h>

//Systems.
#include <Systems/UserInterfaceSystem.h>

//User interface.
#include <UserInterface/TextUserInterfaceElement.h>
#include <UserInterface/TextUserInterfaceElementDescription.h>

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

	//Container for all user interface elements for all profiling entries.
	DynamicArray<UserInterfaceElement *RESTRICT> _UserInterfaceElements;

}

/*
*	Updates the profiling system during the physics update phase.
*/
void ProfilingSystem::PhysicsUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Create user interface elements for all profiling entries.
	for (const ProfilingSystemData::ProfilingEntry &entry : ProfilingSystemData::_ProfilingEntries)
	{
		TextUserInterfaceElementDescription description;

		description._Type = UserInterfaceElementType::TEXT;
		description._Minimum = Vector2<float>(0.01f, 0.89f);
		description._Maximum = Vector2<float>(0.99f, 0.99f);
		description._Font = &ResourceLoader::GetFontResource(HashString("Catalyst_Engine_Default_Font"));

		char buffer[64];
		sprintf_s(buffer, " - %f milliseconds", entry._Duration);

		description._Text = entry._Name + buffer;

		UserInterfaceElement *const RESTRICT element{ UserInterfaceSystem::Instance->CreateUserInterfaceElement(&description) };

		ProfilingSystemData::_UserInterfaceElements.Emplace(element);
	}
}

/*
*	Updates the profiling system during the pre update phase.
*/
void ProfilingSystem::PostUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Clear all profiling entries and their associated data.
	ProfilingSystemData::_ProfilingEntries.ClearSlow();

	for (UserInterfaceElement *const RESTRICT element : ProfilingSystemData::_UserInterfaceElements)
	{
		UserInterfaceSystem::Instance->DestroyUserInterfaceElement(element);
	}

	ProfilingSystemData::_UserInterfaceElements.ClearFast();
}

/*
*	Adds a profiling entry.
*/
void ProfilingSystem::AddProfilingEntry(const DynamicString &name, const float duration) NOEXCEPT
{
	//Add the profiling entry.
	ProfilingSystemData::_ProfilingEntries.Emplace(name, duration);
}