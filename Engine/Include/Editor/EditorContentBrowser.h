#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>

//Math.
#include <Math/General/Vector.h>

//Forward declarations.
class Resource;

//Type aliases.
using ContentBrowserCallbackFunction = void(*)(Resource *const RESTRICT resource, void *const RESTRICT arguments);

class EditorContentBrowser final
{

public:

	/*
	*	Request browsing for an asset.
	*/
	void Request
	(
		const char *const RESTRICT prompt,
		const HashString resource_identifier,
		const ContentBrowserCallbackFunction callback_function,
		void *const RESTRICT user_data
	) NOEXCEPT;

private:

	//The filtered resources.
	DynamicArray<Resource *const RESTRICT> _FilteredResources;

	//The prompt.
	const char *RESTRICT _Prompt;

	//The resource identifier.
	HashString _ResourceIdentifier;

	//The callback function.
	ContentBrowserCallbackFunction _CallbackFunction;

	//The user data.
	void *RESTRICT _UserData;

	//Denotes if the content browser should be open.
	bool _ShouldBeOpen;

	/*
	*	Updates the floating window.
	*/
	NO_DISCARD bool UpdateFloatingWindow(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT;

};

#endif
