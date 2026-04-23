#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//UI.
#include <UI/Core/Animator.h>
#include <UI/Core/Widget.h>

namespace UI
{

	/*
	*	Slider widget.
	*/
	class SelectorWidget final : public UI::Widget
	{

	public:

		//Type aliases.
		using OnIndexChangedCallback = void(*)(UI::Scene *const RESTRICT scene, const uint64 new_index);

		/*
		*	Default constructor.
		*/
		SelectorWidget() NOEXCEPT;

		/*
		*	Callback for when the parent is available.
		*/
		void OnParentAvailable() NOEXCEPT override;

		/*
		*	Positions this widget. Usually called right after creation.
		*/
		void Position() NOEXCEPT override;

		/*
		*	Callback for when this widget is disabled.
		*/
		void OnDisabled() NOEXCEPT override;

		/*
		*	Returns the clickable interface (if this widget is clickable.
		*/
		FORCE_INLINE NO_DISCARD ArrayProxy<UI::ClickableInterface *const RESTRICT> GetClickableInterfaces() NOEXCEPT override
		{
			return ArrayProxy<UI::ClickableInterface *const RESTRICT>(_ClickableInterfacePointers.Data(), 2);
		}

		/*
		*	Sets the values.
		*/
		FORCE_INLINE SelectorWidget *const RESTRICT SetValues(const DynamicArray<DynamicString> &values, const uint64 current_index) NOEXCEPT
		{
			_Values = values;
			_CurrentIndex = current_index;

			return this;
		}

		/*
		*	Sets the prefix.
		*/
		FORCE_INLINE SelectorWidget *const RESTRICT SetPrefix(const char *const RESTRICT prefix) NOEXCEPT
		{
			_Prefix = prefix;
			return this;
		}

		/*
		*	Sets the postfix.
		*/
		FORCE_INLINE SelectorWidget *const RESTRICT SetPostfix(const char *const RESTRICT postfix) NOEXCEPT
		{
			_Postfix = postfix;
			return this;
		}

		/*
		*	Sets the text scale.
		*/
		FORCE_INLINE SelectorWidget *const RESTRICT SetTextScale(const float32 value) NOEXCEPT
		{
			_TextScale = value;
			return this;
		}

		/*
		*	Sets the on index changed callback.
		*/
		FORCE_INLINE SelectorWidget *const RESTRICT SetOnIndexChangedCallback(const OnIndexChangedCallback callback) NOEXCEPT
		{
			_OnIndexChangedCallback = callback;
			return this;
		}

		/*
		*	Renders this widget.
		*/
		void Render(const UI::RenderContext &context) NOEXCEPT override;

	private:

		//Enumeration covering all animation directions.
		enum class AnimationDirection : uint8
		{
			RIGHT,
			LEFT
		};

		/*
		*	Clickable interface data class definition.
		*/
		class ClickableInterfaceData final
		{

		public:

			//The axis aligned bounding box.
			AxisAlignedBoundingBox2D _AxisAlignedBoundingBox;

			//The index.
			uint64 _Index;

		};

		//The clickable interfaces.
		StaticArray<UI::ClickableInterface, 2> _ClickableInterfaces;

		//The clickable interface data.
		StaticArray<ClickableInterfaceData, 2> _ClickableInterfaceData;

		//The clickable interface pointers.
		StaticArray<UI::ClickableInterface *RESTRICT, 2> _ClickableInterfacePointers;

		//The animators.
		StaticArray<UI::Animator, 2> _Animators;

		//The source colors.
		StaticArray<Vector4<float32>, 2> _SourceColors;

		//The destination colors.
		StaticArray<Vector4<float32>, 2> _DestinationColors;

		//The animation directions.
		StaticArray<AnimationDirection, 2> _AnimationDirections{ AnimationDirection::RIGHT, AnimationDirection::RIGHT };

		//The values.
		DynamicArray<DynamicString> _Values;

		//The current index.
		uint64 _CurrentIndex;

		//The prefix.
		DynamicString _Prefix;

		//The postfix.
		DynamicString _Postfix;

		//The text scale.
		float32 _TextScale;

		//The on index changed callback.
		OnIndexChangedCallback _OnIndexChangedCallback{ nullptr };

		/*
		*	Resets the animation.
		*/
		void ResetAnimation() NOEXCEPT;

		/*
		*	Decrements.
		*/
		void Decrement() NOEXCEPT;

		/*
		*	Increments.
		*/
		void Increment() NOEXCEPT;

	};

}