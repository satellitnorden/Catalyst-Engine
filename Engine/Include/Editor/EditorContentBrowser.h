#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>

//Components.
#include <Components/Core/Component.h>

//Math.
#include <Math/General/Vector.h>

//Forward declarations.
class Asset;

//Type aliases.
using ContentBrowserCallbackFunction = void(*)(Component *const RESTRICT component, Entity *const RESTRICT entity, const ComponentEditableField *const RESTRICT editable_field, Asset *const RESTRICT asset, void *const RESTRICT user_data);

class EditorContentBrowser final
{

public:

	/*
	*	Request browsing for an asset.
	*/
	void Request
	(
		const char *const RESTRICT prompt,
		const HashString asset_type_identifier,
		Component *const RESTRICT component,
		Entity *const RESTRICT entity,
		const ComponentEditableField *const RESTRICT editable_field,
		void *const RESTRICT user_data,
		const ContentBrowserCallbackFunction callback_function
	) NOEXCEPT;

private:

	//The filtered assets.
	DynamicArray<Asset *const RESTRICT> _FilteredAssets;

	//The prompt.
	const char *RESTRICT _Prompt;

	//The asset type identifier.
	HashString _AssetTypeIdentifier;

	//The component.
	Component *RESTRICT _Component;

	//The entity.
	Entity *RESTRICT _Entity;

	//The editable field.
	const ComponentEditableField *RESTRICT _EditableField;

	//The user data.
	void* RESTRICT _UserData;

	//The callback function.
	ContentBrowserCallbackFunction _CallbackFunction;

	//Denotes if the content browser should be open.
	bool _ShouldBeOpen;

	/*
	*	Updates the floating window.
	*/
	NO_DISCARD bool UpdateFloatingWindow(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT;

};

#endif
