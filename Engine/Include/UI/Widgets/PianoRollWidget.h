#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//UI.
#include <UI/Core/Widget.h>

namespace UI
{

	/*
	*	Piano roll widget, renders a full 128-note piano roll that can be interacted with..
	*/
	class PianoRollWidget final : public UI::Widget
	{

	public:

		/*
		*	Default constructor.
		*/
		PianoRollWidget() NOEXCEPT;

		/*
		*	Returns the clickable interface (if this widget is clickable.
		*/
		FORCE_INLINE NO_DISCARD ArrayProxy<UI::ClickableInterface *const RESTRICT> GetClickableInterfaces() NOEXCEPT override
		{
			return ArrayProxy<UI::ClickableInterface *const RESTRICT>(_ClickableInterfacePointers.Data(), _ClickableInterfacePointers.Size());
		}

		/*
		*	Renders this widget.
		*/
		void Render(const UI::RenderContext &context) NOEXCEPT override;

	private:

		/*
		*	Key information class definition.
		*/
		class KeyInformation final
		{

		public:

			//The clickable interface.
			const UI::ClickableInterface *RESTRICT _ClickableInterface;

			//The bounding box.
			AxisAlignedBoundingBox2D _BoundingBox;

			//Denotes whether or not this key is black.
			bool _IsBlack;

		};

		//The key information.
		StaticArray<KeyInformation, 128> _KeyInformation;

		//The clickable interfaces.
		StaticArray<UI::ClickableInterface, 128> _ClickableInterfaces;

		//The clickable interface pointers.
		StaticArray<UI::ClickableInterface *RESTRICT, 128> _ClickableInterfacePointers;

		/*
		*	Returns if the given key is black.
		*/
		NO_DISCARD bool IsKeyBlack(const uint8 note) const NOEXCEPT;

		/*
		*	Sets up the key information.
		*/
		void SetupKeyInformation() NOEXCEPT;

		/*
		*	Returns the key color.
		*/
		NO_DISCARD Vector4<float32> GetKeyColor(const UI::ClickableInterface::State state, const bool is_black) NOEXCEPT;

	};

}