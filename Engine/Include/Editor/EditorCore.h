#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Third party.
#include <ThirdParty/imgui.h>

//Editor constants.
namespace EditorConstants
{
	constexpr float32 GENERAL_WINDOW_WIDTH{ 0.1'25f };
	constexpr ImGuiWindowFlags WINDOW_FLAGS{ ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings };
}