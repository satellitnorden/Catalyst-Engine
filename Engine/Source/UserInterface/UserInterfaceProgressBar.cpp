//Header file.
#include <UserInterface/UserInterfaceProgressBar.h>

//Systems.
#include <Systems/UserInterfaceSystem.h>

//User interface.
#include <UserInterface/ImageUserInterfacePrimitiveDescription.h>
#include <UserInterface/TextUserInterfacePrimitiveDescription.h>

/*
*	Default constructor.
*/
UserInterfaceProgressBar::UserInterfaceProgressBar(	const Vector2<float32> initial_minimum,
													const Vector2<float32> initial_maximum,
													const UserInterfaceMaterial &initial_bottom_material,
													const UserInterfaceMaterial &initial_top_material,
													const char *const RESTRICT text,
													const AssetPointer<FontAsset> font) NOEXCEPT
{
	//Set the type.
	SetType(UserInterfaceElementType::PROGRESS_BAR);

	//Set the font.
	_Font = font;

	//Create the bottom and top primitive.
	{
		ImageUserInterfacePrimitiveDescription description;

		description._Type = UserInterfacePrimitiveType::IMAGE;
		description._Minimum = initial_minimum;
		description._Maximum = initial_maximum;
		description._Opacity = 1.0f;
		description._Material = initial_bottom_material;

		_BottomPrimitive = static_cast<ImageUserInterfacePrimitive *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfacePrimitive(&description));
	}

	{
		ImageUserInterfacePrimitiveDescription description;

		description._Type = UserInterfacePrimitiveType::IMAGE;
		description._Minimum = initial_minimum;
		description._Maximum = Vector2<float32>(initial_minimum._X, initial_maximum._Y);
		description._Opacity = 1.0f;
		description._Material = initial_top_material;

		_TopPrimitive = static_cast<ImageUserInterfacePrimitive *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfacePrimitive(&description));
	}

	//Set the text.
	SetText(text);
}

/*
*	Default destructor.
*/
UserInterfaceProgressBar::~UserInterfaceProgressBar() NOEXCEPT
{
	//Destroy the primitives.
	UserInterfaceSystem::Instance->DestroyUserInterfacePrimitive(_BottomPrimitive);
	UserInterfaceSystem::Instance->DestroyUserInterfacePrimitive(_TopPrimitive);

	if (_TextPrimitive)
	{
		UserInterfaceSystem::Instance->DestroyUserInterfacePrimitive(_TextPrimitive);
	}
}

/*
*	Sets the current progress.
*/
void UserInterfaceProgressBar::SetCurrentProgress(const float32 value) NOEXCEPT
{
	//Set the current progress.
	_CurrentProgress = value;

	//Update the bounding box for the top primitive.
	_TopPrimitive->_Maximum._X = BaseMath::LinearlyInterpolate(_BottomPrimitive->_Minimum._X, _BottomPrimitive->_Maximum._X, _CurrentProgress);
}

/*
*	Sets the text.
*/
void UserInterfaceProgressBar::SetText(const char *const RESTRICT text) NOEXCEPT
{
	if (text && text != "")
	{
		if (!_TextPrimitive)
		{
			TextUserInterfacePrimitiveDescription description;

			description._Type = UserInterfacePrimitiveType::TEXT;
			description._Minimum = _BottomPrimitive->_Minimum;
			description._Maximum = _BottomPrimitive->_Maximum;
			description._Opacity = 1.0f;
			description._Font = _Font;
			description._Scale = 0.02'50f;
			description._HorizontalAlignment = TextHorizontalAlignment::CENTER;
			description._VerticalAlignment = TextVerticalAlignment::CENTER;
			description._Text = text;

			_TextPrimitive = static_cast<TextUserInterfacePrimitive *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfacePrimitive(&description));
		}

		else
		{
			_TextPrimitive->_Text = text;
		}
	}

	else
	{
		if (_TextPrimitive)
		{
			UserInterfaceSystem::Instance->DestroyUserInterfacePrimitive(_TextPrimitive);
		}

		_TextPrimitive = nullptr;
	}
}

/*
*	Returns the user interface primitives.
*/
void UserInterfaceProgressBar::RetrieveUserInterfacePrimitives(DynamicArray<UserInterfacePrimitive *RESTRICT> *const RESTRICT output) const NOEXCEPT
{
	output->Emplace(_BottomPrimitive);
	output->Emplace(_TopPrimitive);

	if (_TextPrimitive)
	{
		output->Emplace(_TextPrimitive);
	}
}