#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Third party.
#include <ThirdParty/ImGui/imgui.h>

//Editor constants.
namespace EditorConstants
{
	constexpr float32 GENERAL_WINDOW_WIDTH{ 0.1'50f };
	constexpr ImGuiWindowFlags WINDOW_FLAGS{ ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings };
}

//Enumeration covering all editor icons.
enum class EditorIcon : uint8
{
	PLAY,
	CREATE,
	TRANSLATE,

	NUMBER_OF_EDITOR_ICONS
};