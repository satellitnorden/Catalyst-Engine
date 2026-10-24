//Header file.
#include <UI/Widgets/PianoRollWidget.h>

//UI.
#include <UI/Core/Scene.h>

namespace UI
{

    /*
    *	Default constructor.
    */
    PianoRollWidget::PianoRollWidget() NOEXCEPT
    {
        //Set up the clickable interfaces.
        uint8 clickable_interface_index{ 0 };

        //Add clickable interfaces for the white keys first.
        for (uint8 key_index{ 0 }; key_index < 128; ++key_index)
        {
            if (IsKeyBlack(key_index))
            {
                continue;
            }

            UI::ClickableInterface &clickable_interface{ _ClickableInterfaces[clickable_interface_index++] };
            KeyInformation &key_information{ _KeyInformation[key_index] };

            clickable_interface.SetUserData(&_KeyInformation[key_index]);
            key_information._ClickableInterface = &clickable_interface;
        }

        //Add clickable interfaces for the black keys next.
        for (uint8 key_index{ 0 }; key_index < 128; ++key_index)
        {
            if (!IsKeyBlack(key_index))
            {
                continue;
            }

            UI::ClickableInterface &clickable_interface{ _ClickableInterfaces[clickable_interface_index++] };
            KeyInformation &key_information{ _KeyInformation[key_index] };

            clickable_interface.SetUserData(&_KeyInformation[key_index]);
            key_information._ClickableInterface = &clickable_interface;
        }

        //Set up the clickable interfaces.
        for (uint8 key_index{ 0 }; key_index < 128; ++key_index)
        {
            _ClickableInterfaces[key_index]._IsInside = [](const Widget *const RESTRICT widget, ClickableInterface *const RESTRICT clickable_interface, const Vector2<float32> position)
            {
                const KeyInformation *const RESTRICT key_information{ static_cast<const KeyInformation *const RESTRICT>(clickable_interface->GetUserData()) };
                return key_information->_BoundingBox.IsInside(position);
            };
            _ClickableInterfaces[key_index].SetOnStartPressCallback
            (
                [](UI::Widget *const RESTRICT widget, void *const RESTRICT user_data)
                {
                    static_cast<PianoRollWidget *const RESTRICT>(widget)->OnKeyStartPress(*static_cast<const KeyInformation *const RESTRICT>(user_data));
                }
            );
            _ClickableInterfaces[key_index].SetOnEndPressCallback
            (
                [](UI::Widget *const RESTRICT widget, void *const RESTRICT user_data)
                {
                    static_cast<PianoRollWidget *const RESTRICT>(widget)->OnKeyEndPress(*static_cast<const KeyInformation *const RESTRICT>(user_data));
                }
            );
        }

        //Set up the clickable interface pointers.
        for (uint8 key_index{ 0 }; key_index < 128; ++key_index)
        {
            _ClickableInterfacePointers[key_index] = &_ClickableInterfaces[key_index];
        }
    }

	/*
	*	Renders this widget.
	*/
	void PianoRollWidget::Render(const UI::RenderContext &context) NOEXCEPT
	{
        //Set up the key information.
        SetupKeyInformation();

        //Render all the white keys.
        for (const KeyInformation &key_information : _KeyInformation)
        {
            //Skip if black.
            if (key_information._IsBlack)
            {
                continue;
            }

            //Retrieve the key state.
            const UI::ClickableInterface::State key_state{ GetKeyState(key_information) };

            //Cache the color.
            const Vector4<float32> color{ GetKeyColor(key_state, false) };

            //Render the base box.
            {
                //Calculate the bounding box.
                AxisAlignedBoundingBox2D bounding_box{ key_information._BoundingBox };

                //Render!
                RenderSolidBox(context, bounding_box, color, Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
            }
        
            //Add a smaller box at the bottom to simulate some form of lighting.
            {
                //Calculate the bounding box.
                AxisAlignedBoundingBox2D bounding_box{ key_information._BoundingBox };
                bounding_box._Maximum._Y = bounding_box._Minimum._Y + bounding_box.Dimensions()._Y * 0.075f * (key_state == UI::ClickableInterface::State::PRESSED ? 0.625f : 1.0f);

                //Calculate the color.
                const Vector4<float32> _color{ color * Vector4<float32>(0.5f, 0.5f, 0.5f, 1.0f) };

                //Render!
                RenderSolidBox(context, bounding_box, _color, Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
            }
        }

        //Render all the black keys.
        for (const KeyInformation &key_information : _KeyInformation)
        {
            //Skip if not black.
            if (!key_information._IsBlack)
            {
                continue;
            }

            //Retrieve the key state.
            const UI::ClickableInterface::State key_state{ GetKeyState(key_information) };

            //Cache the color.
            const Vector4<float32> color{ GetKeyColor(key_state, true) };

            //Render the base box.
            {
                //Calculate the bounding box.
                AxisAlignedBoundingBox2D bounding_box{ key_information._BoundingBox };

                //Render!
                RenderSolidBox(context, bounding_box, color, Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
            }

            //Add a smaller box at the bottom to simulate some form of lighting.
            {
                //Calculate the bounding box.
                AxisAlignedBoundingBox2D bounding_box{ key_information._BoundingBox };
                bounding_box._Maximum._Y = bounding_box._Minimum._Y + bounding_box.Dimensions()._Y * 0.125f * (key_state == UI::ClickableInterface::State::PRESSED ? 0.625f : 1.0f);

                //Calculate the color.
                const Vector4<float32> _color{ color * Vector4<float32>(1.5f, 1.5f, 1.5f, 1.0f) };

                //Render!
                RenderSolidBox(context, bounding_box, _color, Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
            }
        }
	}

    /*
    *	Returns if the given key is black.
    */
    NO_DISCARD bool PianoRollWidget::IsKeyBlack(const uint8 note) const NOEXCEPT
    {
        //Define constants.
        constexpr bool IS_BLACK[]
        {
            false,
            true,
            false,
            true,
            false,

            false,
            true,
            false,
            true,
            false,
            true,
            false
        };

        //Return if the key is black.
        return IS_BLACK[note % ARRAY_LENGTH(IS_BLACK)];
    }

	/*
	*	Sets up the key information.
	*/
	void PianoRollWidget::SetupKeyInformation() NOEXCEPT
	{
		//Define constants.
		constexpr uint8 TOTAL_NUMBER_OF_WHITE_BUTTONS{ 75 };
        constexpr float32 WHITE_BUTTON_PADDING{ 1.0f };
		
        //Calculate the black button width.
        const float32 black_button_width{ BaseMath::Round<float32>(_AxisAlignedBoundingBox.Dimensions()._X / static_cast<float32>(TOTAL_NUMBER_OF_WHITE_BUTTONS) * 0.75f) };

		//TODO: See if we can do this only once.
        const KeyInformation *RESTRICT last_white_key_information{ nullptr };
        uint8 number_of_white_buttons{ 0 };

        for (uint8 i{ 0 }; i < 128; ++i)
        {
            //Cache the key information.
            KeyInformation &key_information{ _KeyInformation[i] };

            //Cache if this key is black.
            const bool is_black{ IsKeyBlack(i) };

            //Calculate the bounds.
            if (is_black)
            {
                const KeyInformation &previous_key_information{ _KeyInformation[i - 1] };
                const float32 center{ previous_key_information._BoundingBox._Maximum._X + WHITE_BUTTON_PADDING * 0.5f };

                key_information._BoundingBox._Minimum._X = center - black_button_width * 0.5f;
                key_information._BoundingBox._Minimum._Y = _AxisAlignedBoundingBox._Minimum._Y + _AxisAlignedBoundingBox.Dimensions()._Y * 0.4f;

                key_information._BoundingBox._Maximum._X = center + black_button_width * 0.5f;
                key_information._BoundingBox._Maximum._Y = _AxisAlignedBoundingBox._Maximum._Y;
            }

            else
            {
                const float32 minimum{ static_cast<float32>(number_of_white_buttons) / static_cast<float32>(TOTAL_NUMBER_OF_WHITE_BUTTONS) };
                const float32 maximum{ static_cast<float32>(number_of_white_buttons + 1) / static_cast<float32>(TOTAL_NUMBER_OF_WHITE_BUTTONS) };

                if (last_white_key_information)
                {
                    key_information._BoundingBox._Minimum._X = last_white_key_information->_BoundingBox._Maximum._X + WHITE_BUTTON_PADDING;
                }

                else
                {
                    key_information._BoundingBox._Minimum._X = BaseMath::LinearlyInterpolate(_AxisAlignedBoundingBox._Minimum._X, _AxisAlignedBoundingBox._Maximum._X, minimum);
                }
                
                key_information._BoundingBox._Minimum._Y = _AxisAlignedBoundingBox._Minimum._Y;

                key_information._BoundingBox._Maximum._X = BaseMath::Round<float32>(BaseMath::LinearlyInterpolate(_AxisAlignedBoundingBox._Minimum._X, _AxisAlignedBoundingBox._Maximum._X, maximum));
                key_information._BoundingBox._Maximum._Y = _AxisAlignedBoundingBox._Maximum._Y;

                last_white_key_information = &key_information;
            }

            //Set if this key is black.
            key_information._IsBlack = is_black;

            //Set the note.
            key_information._Note = i;

            //Update the number of white buttons.
            if (!is_black)
            {
                ++number_of_white_buttons;
            }
        }
	}

    /*
    *	Returns the state for the given key.
    */
    NO_DISCARD UI::ClickableInterface::State PianoRollWidget::GetKeyState(const KeyInformation &key_information) NOEXCEPT
    {
        //First get the state from the key's clickable interface.
        UI::ClickableInterface::State state{ key_information._ClickableInterface->GetState() };

        //If we have an audio plugin, and the note state says it's pressed, use that instead.
        if (_AudioPlugin)
        {
            const Audio::Plugin::MIDINoteState &note_state{ _AudioPlugin->GetMIDINoteState(key_information._Note) };

            if (note_state._Pressed.Load())
            {
                state = UI::ClickableInterface::State::PRESSED;
            }
        }

        //Return the state.
        return state;
    }

    /*
    *	Returns the key color.
    */
    NO_DISCARD Vector4<float32> PianoRollWidget::GetKeyColor(const UI::ClickableInterface::State state, const bool is_black) NOEXCEPT
    {
        //Define constants.
        constexpr float32 BLACK_IDLE_COLOR{ 32.0f / 255.0f };
        constexpr float32 BLACK_HOVERED_COLOR{ 48.0f / 255.0f };
        constexpr float32 BLACK_PRESSED_COLOR{ 64.0f / 255.0f };
        constexpr float32 WHITE_IDLE_COLOR{ 200.0f / 255.0f };
        constexpr float32 WHITE_HOVERED_COLOR{ 225.0f / 255.0f };
        constexpr float32 WHITE_PRESSED_COLOR{ 250.0f / 255.0f };

        if (is_black)
        {
            switch (state)
            {
                case UI::ClickableInterface::State::IDLE:
                {
                    return Vector4<float32>(BLACK_IDLE_COLOR, BLACK_IDLE_COLOR, BLACK_IDLE_COLOR, 1.0f);
                }

                case UI::ClickableInterface::State::HOVERED:
                {
                    return Vector4<float32>(BLACK_HOVERED_COLOR, BLACK_HOVERED_COLOR, BLACK_HOVERED_COLOR, 1.0f);
                }

                case UI::ClickableInterface::State::PRESSED:
                {
                    return Vector4<float32>(BLACK_PRESSED_COLOR, BLACK_PRESSED_COLOR, BLACK_PRESSED_COLOR, 1.0f);
                }

                default:
                {
                    ASSERT(false, "Invalid case!");

                    return Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
                }
            }
        }

        else
        {
            switch (state)
            {
                case UI::ClickableInterface::State::IDLE:
                {
                    return Vector4<float32>(WHITE_IDLE_COLOR, WHITE_IDLE_COLOR, WHITE_IDLE_COLOR, 1.0f);
                }

                case UI::ClickableInterface::State::HOVERED:
                {
                    return Vector4<float32>(WHITE_HOVERED_COLOR, WHITE_HOVERED_COLOR, WHITE_HOVERED_COLOR, 1.0f);
                }

                case UI::ClickableInterface::State::PRESSED:
                {
                    return Vector4<float32>(WHITE_PRESSED_COLOR, WHITE_PRESSED_COLOR, WHITE_PRESSED_COLOR, 1.0f);
                }

                default:
                {
                    ASSERT(false, "Invalid case!");

                    return Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
                }
            }
        }
    }

    /*
    *	Callback for when a key starts being pressed.
    */
    void PianoRollWidget::OnKeyStartPress(const KeyInformation &key_information) NOEXCEPT
    {
        //Only valid if we have a plugin right now.
        if (!_AudioPlugin)
        {
            return;
        }

        //Construct the message.
        MIDIMessage message;

        message._Type = MIDIMessage::Type::NOTE_ON;
        message._NoteOnChannel = 0;
        message._NoteOnNote = key_information._Note;
        message._NoteOnVelocity = 127; //TODO: Calculate this. (:

        //Add the message!
        _AudioPlugin->AddMIDIMessage(message);
    }

    /*
    *	Callback for when a key end being pressed.
    */
    void PianoRollWidget::OnKeyEndPress(const KeyInformation &key_information) NOEXCEPT
    {
        //Only valid if we have a plugin right now.
        if (!_AudioPlugin)
        {
            return;
        }

        //Construct the message.
        MIDIMessage message;

        message._Type = MIDIMessage::Type::NOTE_OFF;
        message._NoteOffChannel = 0;
        message._NoteOffNote = key_information._Note;

        //Add the message!
        _AudioPlugin->AddMIDIMessage(message);
    }

}