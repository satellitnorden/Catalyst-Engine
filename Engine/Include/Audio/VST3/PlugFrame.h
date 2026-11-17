#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Print.h>
#include <Core/Utilities/StringUtilities.h>

//Audio.
#include <Audio/VST3/ReferenceCounting.h>

//Platform.
#include <Platform/Core/WindowManager.h>

//Third party.
#include <VST3/pluginterfaces/gui/iplugview.h>

namespace VST3
{

	class PlugFrame final : public Steinberg::IPlugFrame
	{

	public:

		VST3_REFERENCE_COUNTING();

		/*
		*	Queries an instance.
		*/
		FORCE_INLINE Steinberg::tresult queryInterface(const Steinberg::TUID _iid, void **obj) override
		{
			//Check that we have valid arguments.
			if (!obj)
			{
				return Steinberg::kInvalidArgument;
			}

			//Reset the object.
			*obj = nullptr;

			//Are they asking for an unknown?
			if (Steinberg::FUnknownPrivate::iidEqual(_iid, Steinberg::FUnknown_iid))
			{
				*obj = static_cast<Steinberg::FUnknown*>(this);
			}

			//Are they asking for a plug frame?
			else if (Steinberg::FUnknownPrivate::iidEqual(_iid, Steinberg::IPlugFrame_iid))
			{
				*obj = static_cast<Steinberg::IPlugFrame*>(this);
			}

			//Add a reference if we set the object.
			if (*obj)
			{
				addRef();
				return Steinberg::kResultOk;
			}

			//Otherwise, return that we don't have this interface.
			else
			{
				return Steinberg::kNoInterface;
			}
		}

		/*
		*	Resizes the view.
		*/
		FORCE_INLINE Steinberg::tresult PLUGIN_API resizeView(Steinberg::IPlugView *view, Steinberg::ViewRect *newSize) NOEXCEPT override
		{
			//Do some sanity checking.
			if (!_FrameWindow)
			{
				return Steinberg::kInternalError;
			}

			if (!newSize)
			{
				return Steinberg::kInvalidArgument;
			}

			//Set the window resolution.
			Platform::WindowManager::SetResolution(_FrameWindow, Resolution(newSize->getWidth(), newSize->getHeight()));

			//The resize succeeded!
			return Steinberg::kResultOk;
		}

		/*
		*	Returns if this plug frame has a frame window.
		*/
		FORCE_INLINE NO_DISCARD bool HasFrameWindow() const NOEXCEPT
		{
			return _FrameWindow != Platform::INVALID_WINDOW_HANDLE && Platform::WindowManager::IsAlive(_FrameWindow);
		}

		/*
		*	Creates the frame window.
		*/
		FORCE_INLINE void CreateFrameWindow(const char *const RESTRICT name, const Steinberg::ViewRect &view_rect) NOEXCEPT
		{
			//Create the frame window.
			Platform::WindowManager::CreateParameters parameters;

			SNPRINTF(parameters._Title.Data(), parameters._Title.Size(), "%s", name);
			parameters._Resolution = Resolution(view_rect.getWidth(), view_rect.getHeight());
			parameters._FullScreen = false;

			_FrameWindow = Platform::WindowManager::Create(parameters);
		}

		/*
		*	Returns the frame window.
		*/
		FORCE_INLINE NO_DISCARD Platform::WindowHandle GetFrameWindow() const NOEXCEPT
		{
			return Platform::WindowManager::GetWindowHandle(_FrameWindow);
		}

		/*
		*	Destroys the frame window.
		*/
		FORCE_INLINE void DestroyFrameWindow() NOEXCEPT
		{
			Platform::WindowManager::Destroy(_FrameWindow);
			_FrameWindow = Platform::INVALID_WINDOW_HANDLE;
		}

	private:

		//The frame window.
		Platform::WindowHandle _FrameWindow{ Platform::INVALID_WINDOW_HANDLE };

	};

}