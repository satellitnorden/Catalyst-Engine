#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//UI.
#include <UI/Core/ColorOrTexture.h>
#include <UI/Core/Widget.h>

namespace UI
{

	/*
	*	Background widget.
	*	Simple widget that covers the container with a solid color.
	*/
	class BackgroundWidget final : public UI::Widget
	{

	public:

		/*
		*	Positions this widget. Usually called right after creation.
		*/
		void Position() NOEXCEPT override;

		/*
		*	Renders this widget.
		*/
		void Render(const UI::RenderContext &context) NOEXCEPT override;

		/*
		*	Sets the color.
		*/
		FORCE_INLINE BackgroundWidget *const RESTRICT SetColor(const Vector3<float32> &value) NOEXCEPT
		{
			_ColorOrTexture = Vector4<float32>(value, 1.0f);
			return this;
		}

		/*
		*	Sets the texture index.
		*/
		FORCE_INLINE BackgroundWidget *const RESTRICT SetTextureIndex(const uint32 value) NOEXCEPT
		{
			_ColorOrTexture = value;
			return this;
		}

		/*
		*	Sets the opacity.
		*/
		FORCE_INLINE BackgroundWidget *const RESTRICT SetOpacity(const float32 value) NOEXCEPT
		{
			_ColorOpacity._A = value;
			return this;
		}

	private:

		//The color or texture.
		ColorOrTexture _ColorOrTexture;

		//The color/opacity.
		Vector4<float32> _ColorOpacity{ 1.0f, 1.0f, 1.0f, 1.0f };

	};

}