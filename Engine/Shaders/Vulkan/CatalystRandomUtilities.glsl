#if !defined(CATALYST_RANDOM_UTILITIES)
#define CATALYST_RANDOM_UTILITIES

/*
*   Returns a random noise texture offset.
*/
vec2 GetRandomNoiseTextureOffset(int index)
{
    switch (index)
    {
        case 0:
        {
            return vec2(0.0f, 0.0f);
        }

        case 1:
        {
            return vec2(-2.0f, -2.0f) / 64.0f;
        }

        case 2:
        {
            return vec2(-2.0f, -1.0f) / 64.0f;
        }

        case 3:
        {
            return vec2(-2.0f, 0.0f) / 64.0f;
        }

        case 4:
        {
            return vec2(-2.0f, 1.0f) / 64.0f;
        }
        
        case 5:
        {
            return vec2(-2.0f, 2.0f) / 64.0f;
        }

        case 6:
        {
            return vec2(-1.0f, -2.0f) / 64.0f;
        }

        case 7:
        {
            return vec2(-1.0f, -1.0f) / 64.0f;
        }

        case 8:
        {
            return vec2(-1.0f, 0.0f) / 64.0f;
        }

        case 9:
        {
            return vec2(-1.0f, 1.0f) / 64.0f;
        }

        case 10:
        {
            return vec2(-1.0f, 2.0f) / 64.0f;
        }

        case 11:
        {
            return vec2(0.0f, -2.0f) / 64.0f;
        }

        case 12:
        {
            return vec2(0.0f, -1.0f) / 64.0f;
        }

        case 13:
        {
            return vec2(0.0f, 1.0f) / 64.0f;
        }

        case 14:
        {
            return vec2(0.0f, 2.0f) / 64.0f;
        }

        case 15:
        {
            return vec2(1.0f, -2.0f) / 64.0f;
        }

        case 16:
        {
            return vec2(1.0f, -1.0f) / 64.0f;
        }

        case 17:
        {
            return vec2(1.0f, 0.0f) / 64.0f;
        }

        case 18:
        {
            return vec2(1.0f, 1.0f) / 64.0f;
        }

        case 19:
        {
            return vec2(1.0f, 2.0f) / 64.0f;
        }

        case 20:
        {
            return vec2(2.0f, -2.0f) / 64.0f;
        }

        case 21:
        {
            return vec2(2.0f, -1.0f) / 64.0f;
        }

        case 22:
        {
            return vec2(2.0f, 0.0f) / 64.0f;
        }

        case 23:
        {
            return vec2(2.0f, 1.0f) / 64.0f;
        }

        case 24:
        {
            return vec2(2.0f, 2.0f) / 64.0f;
        }
    }
}
#endif