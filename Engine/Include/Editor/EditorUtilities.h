#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Systems.
#include <Systems/RenderingSystem.h>

//User interface.
#include <UserInterface/UserInterfaceCore.h>

//Third party.
#include <ThirdParty/imgui.h>

namespace EditorUtilities
{

	//Enumeration covering all Window anchor

	/*
	*	Sets the window position and size based on the given (normalized) parameters.
	*/
	FORCE_INLINE void SetWindowPositionAndSize(const WindowAnchor anchor, const Vector2<float32> position, const Vector2<float32> size) NOEXCEPT
	{
		//Retrieve the window resolution.
		const Vector2<float32> window_resolution{ static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Width), static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Height) };
	
		//Calculate the window position.
		Vector2<float32> window_position{ CatalystBaseMath::LinearlyInterpolate(0.0f, window_resolution._X, position._X), CatalystBaseMath::LinearlyInterpolate(0.0f, window_resolution._Y, position._Y) };

		switch (anchor)
		{
			case WindowAnchor::BOTTOM_LEFT:
			{
				window_position = window_position;

				break;
			}

			case WindowAnchor::BOTTOM_RIGHT:
			{
				window_position = window_position + Vector2<float32>(window_resolution._X, 0.0f);

				break;
			}

			case WindowAnchor::TOP_LEFT:
			{
				window_position = window_position + Vector2<float32>(0.0f, window_resolution._Y);

				break;
			}

			case WindowAnchor::TOP_RIGHT:
			{
				window_position = window_position + Vector2<float32>(window_resolution._X, window_resolution._Y);

				break;
			}

			case WindowAnchor::CENTER:
			{
				window_position = window_position + Vector2<float32>(window_resolution._X * 0.5f, window_resolution._Y * 0.5f);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Calculate the window size.
		Vector2<float32> window_size{ CatalystBaseMath::LinearlyInterpolate(0.0f, window_resolution._X, size._X), CatalystBaseMath::LinearlyInterpolate(0.0f, window_resolution._Y, size._Y) };

		//Modify the window position.
		window_position._Y = window_position._Y + window_size._Y;
		window_position._Y = window_resolution._Y - window_position._Y;

		//Set the window position and size.
		ImGui::SetWindowPos(ImVec2(window_position._X, window_position._Y));
		ImGui::SetWindowSize(ImVec2(window_size._X, window_size._Y));
	}
}
#endif