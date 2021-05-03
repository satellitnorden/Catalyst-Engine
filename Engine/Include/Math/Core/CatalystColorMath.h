#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class CatalystColorMath
{

public:

	/*
	*	Converts an HSV value to an RGB value.
	*	The HSV value is expected to be in the Hue = [0, 360], Saturation = [0, 100], Value = [0, 100] range.
	*	The converted RGB value is in the [0.0f, 1.0f] range.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float32> HSVToRGB(const float32 hue, const float32 saturation, const float32 value) NOEXCEPT
	{
        ASSERT(hue >= 0.0f && hue <= 360.0f, "Hue is not in range!");
        ASSERT(saturation >= 0.0f && saturation <= 100.0f, "Saturation is not in range!");
        ASSERT(value >= 0.0f && value <= 100.0f, "Value is not in range!");

        const float32 s{ saturation / 100.0f };
        const float32 v{ value / 100.0f };
        const float32 C{ s * v };
        const float32 X{ C * (1.0f - fabs(static_cast<float32>(fmod(hue / 60.0, 2)) - 1.0f)) };
        const float32 m{ v - C };
        
        float32 r;
        float32 g;
        float32 b;

        if(hue >= 0.0f && hue < 60.0f)
        {
            r = C, g = X, b = 0.0f;
        }

        else if(hue >= 60.0f && hue < 120.0f)
        {
            r = X, g = C, b = 0.0f;
        }

        else if(hue >= 120.0f && hue < 180.0f)
        {
            r = 0.0f, g = C, b = X;
        }

        else if(hue >= 180.0f && hue < 240.0f)
        {
            r = 0.0f, g = X, b = C;
        }

        else if(hue >= 240.0f && hue < 300.0f)
        {
            r = X, g = 0.0f, b = C;
        }

        else
        {
            r = C, g = 0.0f, b = X;
        }

        Vector3<float32> output;

        output._R = r + m;
        output._G = g + m;
        output._B = b + m;

        return output;
	}

};