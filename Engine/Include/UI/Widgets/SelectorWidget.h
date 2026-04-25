#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//UI.
#include <UI/Core/Animator.h>
#include <UI/Core/Style.h>
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
		using OnIndexChangedCallback = void(*)(UI::Scene *const RESTRICT scene, UI::Widget *const RESTRICT widget, void *const RESTRICT user_data, const uint64 new_index);
		using OnValueChangedCallback = void(*)(UI::Scene *const RESTRICT scene, UI::Widget *const RESTRICT widget, void *const RESTRICT user_data, const int32 new_value);

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
			_ValuesType = ValuesType::VALUES_INDEX;
			_ValuesIndexData._Values = values;
			_ValuesIndexData._CurrentIndex = current_index;

			return this;
		}

		/*
		*	Sets the values.
		*/
		FORCE_INLINE SelectorWidget *const RESTRICT SetValues(const int32 minimum, const int32 maximum, const int32 current) NOEXCEPT
		{
			_ValuesType = ValuesType::MINIMUM_MAXIMUM;
			_MinimumMaximumData._Minimum = minimum;
			_MinimumMaximumData._Maximum = maximum;
			_MinimumMaximumData._Current = current;

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
		*	Sets the on index changed callback user data.
		*/
		FORCE_INLINE SelectorWidget *const RESTRICT SetOnIndexChangedCallbackUserData(void *const RESTRICT user_data) NOEXCEPT
		{
			_OnIndexChangedCallbackUserData = user_data;
			return this;
		}

		/*
		*	Sets the on value changed callback.
		*/
		FORCE_INLINE SelectorWidget *const RESTRICT SetOnValueChangedCallback(const OnValueChangedCallback callback) NOEXCEPT
		{
			_OnValueChangedCallback = callback;
			return this;
		}

		/*
		*	Sets the on value changed callback user data.
		*/
		FORCE_INLINE SelectorWidget *const RESTRICT SetOnValueChangedCallbackUserData(void *const RESTRICT user_data) NOEXCEPT
		{
			_OnValueChangedCallbackUserData = user_data;
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

		//Enumeration covering all values types.
		enum class ValuesType : uint8
		{
			/*
			*	None is set. (:
			*/
			NONE,

			/*
			*	Uses an array of strings + current index.
			*/
			VALUES_INDEX,

			/*
			*	Uses minimum + maximum + current value.
			*/
			MINIMUM_MAXIMUM
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

		//The source states.
		StaticArray<UI::ClickableInterface::State, 2> _SourceStates;

		//The destination states.
		StaticArray<UI::ClickableInterface::State, 2> _DestinationStates;

		//The animation directions.
		StaticArray<AnimationDirection, 2> _AnimationDirections{ AnimationDirection::RIGHT, AnimationDirection::RIGHT };

		//The values type.
		ValuesType _ValuesType{ ValuesType::NONE };

		struct
		{
			//The values.
			DynamicArray<DynamicString> _Values;

			//The current index.
			uint64 _CurrentIndex;
		} _ValuesIndexData;

		struct
		{
			//The minimum.
			int32 _Minimum;

			//The maximum.
			int32 _Maximum;

			//The current.
			int32 _Current;
		} _MinimumMaximumData;

		//The prefix.
		DynamicString _Prefix;

		//The postfix.
		DynamicString _Postfix;

		//The style.
		UI::Style _Style;

		//The text scale.
		float32 _TextScale;

		//The on index changed callback.
		OnIndexChangedCallback _OnIndexChangedCallback{ nullptr };

		//The on index changed callback user data.
		void *RESTRICT _OnIndexChangedCallbackUserData{ nullptr };

		//The on value changed callback.
		OnValueChangedCallback _OnValueChangedCallback{ nullptr };

		//The on value changed callback user data.
		void *RESTRICT _OnValueChangedCallbackUserData{ nullptr };

		/*
		*	Resets the animation.
		*/
		void ResetAnimation() NOEXCEPT;

		/*
		*	Returns the color for the given state.
		*/
		NO_DISCARD Vector4<float32> GetColor(const UI::ClickableInterface::State state) NOEXCEPT;

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