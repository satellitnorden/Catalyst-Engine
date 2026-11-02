#if defined(AUDIO_PLUGIN_VST3)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Plugins/Adapters/VST3/VST3PluginData.h>

//Third party.
#include <VST3/pluginterfaces/gui/iplugview.h>

class VST3PlugView final : public Steinberg::IPlugView
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE VST3PlugView(VST3PluginData *const RESTRICT plugin_data) NOEXCEPT
		:
		_PluginData(plugin_data)
	{

	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~VST3PlugView() NOEXCEPT
	{

	}

	/*
	*	Queries if this object has the given interface.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID _iid, void **obj) override
	{
		if (!obj)
		{
			return Steinberg::kInvalidArgument;
		}

		*obj = nullptr;

		QUERY_INTERFACE(_iid, obj, Steinberg::FUnknown_iid, VST3PlugView);
		QUERY_INTERFACE(_iid, obj, Steinberg::IPlugView_iid, VST3PlugView);

		return Steinberg::kNoInterface;
	}

	/*
	*	Adds a reference.
	*/
	FORCE_INLINE uint32 PLUGIN_API addRef() override
	{
		return 1;
	}

	/*
	*	Releases a reference.
	*/
	FORCE_INLINE uint32 PLUGIN_API release() override
	{
		return 1;
	}

	/*
	*	Returns if the given platform type is supported.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API isPlatformTypeSupported(Steinberg::FIDString type) override
	{
		if (Steinberg::FIDStringsEqual(type, Steinberg::kPlatformTypeHWND))
		{
			return Steinberg::kResultTrue;
		}

		else
		{
			return Steinberg::kResultFalse;
		}
	}

	/*
	*	Callback for when this plug view is attached.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API attached(void *parent, Steinberg::FIDString type) override
	{
		//Create the editor.
		_PluginData->_Editor.CreateEditor(parent);

		//Show the editor.
		_PluginData->_Editor.ShowEditorWindow();

		//Return success.
		return Steinberg::kResultOk;
	}

	/*
	*	Callback for when this plug view is removed.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API removed() override
	{
		//Hide the editor.
		_PluginData->_Editor.HideEditorWindow();

		//Destroy the editor.
		_PluginData->_Editor.DestroyEditor();

		//Return success.
		return Steinberg::kResultOk;
	}

	/*
	*	Callback for when the mouse wheel scrolls.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API onWheel(float distance) override
	{
		//Do nothing. (:
		return Steinberg::kResultOk;
	}

	/*
	*	Callback for when a keyboard key is down.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API onKeyDown(Steinberg::char16 key, int16 keyCode, int16 modifiers) override
	{
		//Do nothing. (:
		return Steinberg::kResultOk;
	}

	/*
	*	Callback for when a keyboard key is up.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API onKeyUp(Steinberg::char16 key, int16 keyCode, int16 modifiers) override
	{
		//Do nothing. (:
		return Steinberg::kResultOk;
	}

	/*
	*	Returns the size.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API getSize(Steinberg::ViewRect *size) override
	{
		//Fill in the size.
		size->left = 0;
		size->top = 0;
		size->right = AudioPluginGlue::GetResolution()._Width;
		size->bottom = AudioPluginGlue::GetResolution()._Height;

		//Return success!
		return Steinberg::kResultOk;
	}

	/*
	*	Callback when the window is resized.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API onSize(Steinberg::ViewRect *newSize) override
	{
		//Not supported.
		return Steinberg::kNotImplemented;
	}

	/*
	*	Callback for when the window is focused.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API onFocus(Steinberg::TBool state) override
	{
		//Do nothing. (:
		return Steinberg::kResultOk;
	}

	/*
	*	Sets the frame.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API setFrame(Steinberg::IPlugFrame *frame) override
	{
		//Do nothing. (:
		return Steinberg::kResultOk;
	}

	/*
	*	Returns if this plug view can resize.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API canResize() override
	{
		//Nope. (:
		return Steinberg::kResultFalse;
	}

	/*
	*	Checks the size constrant.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API checkSizeConstraint(Steinberg::ViewRect *rect) override
	{
		//Nope. (:
		return Steinberg::kResultFalse;
	}

private:

	//The plugin data.
	VST3PluginData *RESTRICT _PluginData;

};
#endif