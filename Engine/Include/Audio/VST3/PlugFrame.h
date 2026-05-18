#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Utilities/StringUtilities.h>

//Audio.
#include <Audio/VST3/ReferenceCounting.h>

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

			//Set the window size.
			CatalystPlatform::SetPlatformWindowSize(_FrameWindow, newSize->getWidth(), newSize->getHeight());

			//The resize succeeded!
			return Steinberg::kResultOk;
		}

		/*
		*	Creates the frame window.
		*/
		FORCE_INLINE void CreateFrameWindow(const Steinberg::ViewRect &view_rect) NOEXCEPT
		{
			//Create the frame window.
			_FrameWindow = CatalystPlatform::CreatePlatformWindow("VST3 Editor Host", view_rect.getWidth(), view_rect.getHeight(), false);
		}

		/*
		*	Returns the frame window.
		*/
		FORCE_INLINE NO_DISCARD void *const RESTRICT GetFrameWindow() const NOEXCEPT
		{
			return _FrameWindow;
		}

		/*
		*	Destroys the frame window.
		*/
		FORCE_INLINE void DestroyFrameWindow() NOEXCEPT
		{
			CatalystPlatform::DestroyPlayformWindow(_FrameWindow);
			_FrameWindow = nullptr;
		}

	private:

		//The frame window.
		void *RESTRICT _FrameWindow{ nullptr };

	};

}