#if !defined(CATALYST_SIMPLEX_NOISE)
#define CATALYST_SIMPLEX_NOISE

#if defined(CATALYST_SHADER_LANGUAGE_CXX)
	#include <Rendering/Native/Shader/CatalystShaderCore.h>
#endif

#if defined(CATALYST_SHADER_LANGUAGE_GLSL)
	#include "..\Include\Rendering\Native\Shader\CatalystShaderCore.h"
#endif

/*
*	Returns the permutation at the given index.
*/
CATALYST_SHADER_FUNCTION_RETURN_ONE_ARGUMENT(int, SimplexNoiseGetPermutation, int index)
{
#if defined(CATALYST_SHADER_LANGUAGE_CXX)
	constexpr uint8 PERMUTATIONS[256] 
	{
		151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};
#endif

#if defined(CATALYST_SHADER_LANGUAGE_GLSL)
	int PERMUTATIONS[256] = int[]
	(
		151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	);
#endif
	
	return PERMUTATIONS[index & 255];
}

/*
*	The gradient function.
*/
CATALYST_SHADER_FUNCTION_RETURN_THREE_ARGUMENTS(float, SimplexNoiseGradient, int hash, float x, float y)
{
	int h = hash & 7;
	float u = h < 4 ? x : y;
	float v = h < 4 ? y : x;

	return (bool(h & 1) ? -u : u) + (bool(h & 2) ? -2.0f * v : 2.0f * v);
}

/*
*	The gradient 2 function.
*/
CATALYST_SHADER_FUNCTION_RETURN_ONE_ARGUMENT(vec2, SimplexNoiseGradient2, int hash)
{
#if defined(CATALYST_SHADER_LANGUAGE_CXX)
	constexpr float32 GRADIENT_LOOKUP[8][2]
	{
		{ -1.0f, -1.0f }, { 1.0f, 0.0f } , { -1.0f, 0.0f } , { 1.0f, 1.0f } ,
		{ -1.0f, 1.0f } , { 0.0f, -1.0f } , { 0.0f, 1.0f } , { 1.0f, -1.0f }
	};
#endif

#if defined(CATALYST_SHADER_LANGUAGE_GLSL)

	vec2 GRADIENT_LOOKUP[8] = vec2[]
	(
		vec2(-1.0f, -1.0f), vec2(1.0f, 0.0f), vec2(-1.0f, 0.0f), vec2(1.0f, 1.0f),
		vec2(-1.0f, 1.0f), vec2(0.0f, -1.0f), vec2(0.0f, 1.0f), vec2(1.0f, -1.0f)
	);
#endif
	int h = hash & 7;

	return vec2(GRADIENT_LOOKUP[h][0], GRADIENT_LOOKUP[h][1]);
}

/*
*	Generates a value in the range [-1.0f, 1.0f] at the given 2D coordinates.
*/
CATALYST_SHADER_FUNCTION_RETURN_ONE_ARGUMENT(float, SimplexNoiseGenerate2D, vec2 coordinate)
{
	//Define constants.
	#define F2 (0.366025403f)
	#define G2 (0.211324865f)

	float n0, n1, n2;

	float s = (coordinate.x + coordinate.y) * F2;
	float xs = coordinate.x + s;
	float ys = coordinate.y + s;
	int i = int(CATALYST_SHADER_FUNCTION_FLOOR(xs));
	int j = int(CATALYST_SHADER_FUNCTION_FLOOR(ys));

	float t = float(i + j) * G2;
	float X0 = i - t;
	float Y0 = j - t;
	float x0 = coordinate.x - X0;
	float y0 = coordinate.y - Y0;

	int i1, j1;

	if (x0 > y0)
	{
		i1 = 1;
		j1 = 0;
	}

	else
	{
		i1 = 0;
		j1 = 1;
	}

	float x1 = x0 - i1 + G2;
	float y1 = y0 - j1 + G2;
	float x2 = x0 - 1.0f + 2.0f * G2;
	float y2 = y0 - 1.0f + 2.0f * G2;

	int ii = i & 0xff;
	int jj = j & 0xff;

	float t0 = 0.5f - x0 * x0 - y0 * y0;

	if (t0 < 0.0f)
	{
		n0 = 0.0f;
	}

	else
	{
		t0 *= t0;
		n0 = t0 * t0 * SimplexNoiseGradient(SimplexNoiseGetPermutation(ii + SimplexNoiseGetPermutation(jj)), x0, y0);
	}

	float t1 = 0.5f - x1 * x1 - y1 * y1;

	if (t1 < 0.0f)
	{
		n1 = 0.0f;
	}

	else
	{
		t1 *= t1;
		n1 = t1 * t1 * SimplexNoiseGradient(SimplexNoiseGetPermutation(ii + i1 + SimplexNoiseGetPermutation(jj + j1)), x1, y1);
	}

	float t2 = 0.5f - x2 * x2 - y2 * y2;

	if (t2 < 0.0f)
	{
		n2 = 0.0f;
	}

	else
	{
		t2 *= t2;
		n2 = t2 * t2 * SimplexNoiseGradient(SimplexNoiseGetPermutation(ii + 1 + SimplexNoiseGetPermutation(jj + 1)), x2, y2);
	}

	return 40.0f * (n0 + n1 + n2);

	//Undef constants.
	#undef F2
	#undef G2
}

/*
*	Generates a value with derivatives in the range [-1.0f, 1.0f] at the 2D given coordinates.
*/
CATALYST_SHADER_FUNCTION_RETURN_ONE_ARGUMENT(vec3, SimplexNoiseGenerateDerivatives2D, vec2 coordinate)
{
	//Define constants.
#define F2 (0.366025403f)
#define G2 (0.211324865f)

	float n0, n1, n2;

	float s = (coordinate.x + coordinate.y) * F2;
	float xs = coordinate.x + s;
	float ys = coordinate.y + s;
	int i = int(CATALYST_SHADER_FUNCTION_FLOOR(xs));
	int j = int(CATALYST_SHADER_FUNCTION_FLOOR(ys));

	float t = float(i + j) * G2;
	float X0 = i - t;
	float Y0 = j - t;
	float x0 = coordinate.x - X0;
	float y0 = coordinate.y - Y0;

	int i1 = int(x0 > y0);
	int j1 = int(x0 <= y0);

	float x1 = x0 - i1 + G2;
	float y1 = y0 - j1 + G2;
	float x2 = x0 - 1.0f + 2.0f * G2;
	float y2 = y0 - 1.0f + 2.0f * G2;

	int ii = i & 0xff;
	int jj = j & 0xff;

	float gx0, gy0, gx1, gy1, gx2, gy2;

	float t0 = 0.5f - x0 * x0 - y0 * y0;
	float t20, t40;

	if (t0 < 0.0f)
	{
		t40 = t20 = t0 = n0 = gx0 = gy0 = 0.0f;
	}

	else
	{
		vec2 gradient_2_result = SimplexNoiseGradient2(SimplexNoiseGetPermutation(ii + SimplexNoiseGetPermutation(jj)));
		gx0 = gradient_2_result.x;
		gy0 = gradient_2_result.y;

		t20 = t0 * t0;
		t40 = t20 * t20;
		n0 = t40 * (gx0 * x0 + gy0 * y0);
	}

	float t1 = 0.5f - x1 * x1 - y1 * y1;
	float t21, t41;

	if (t1 < 0.0f)
	{
		t21 = t41 = t1 = n1 = gx1 = gy1 = 0.0f;
	}

	else
	{
		vec2 gradient_2_result = SimplexNoiseGradient2(SimplexNoiseGetPermutation(ii + i1 + SimplexNoiseGetPermutation(jj + j1)));
		gx1 = gradient_2_result.x;
		gy1 = gradient_2_result.y;

		t21 = t1 * t1;
		t41 = t21 * t21;
		n1 = t41 * (gx1 * x1 + gy1 * y1);
	}

	float t2 = 0.5f - x2 * x2 - y2 * y2;
	float t22, t42;

	if (t2 < 0.0f)
	{
		t42 = t22 = t2 = n2 = gx2 = gy2 = 0.0f;
	}
	
	else
	{
		vec2 gradient_2_result = SimplexNoiseGradient2(SimplexNoiseGetPermutation(ii + 1 + SimplexNoiseGetPermutation(jj + 1)));
		gx2 = gradient_2_result.x;
		gy2 = gradient_2_result.y;

		t22 = t2 * t2;
		t42 = t22 * t22;
		n2 = t42 * (gx2 * x2 + gy2 * y2);
	}

	float temp0 = t20 * t0 * (gx0* x0 + gy0 * y0);
	float dnoise_dx = temp0 * x0;
	float dnoise_dy = temp0 * y0;
	float temp1 = t21 * t1 * (gx1 * x1 + gy1 * y1);
	dnoise_dx += temp1 * x1;
	dnoise_dy += temp1 * y1;
	float temp2 = t22 * t2 * (gx2* x2 + gy2 * y2);
	dnoise_dx += temp2 * x2;
	dnoise_dy += temp2 * y2;
	dnoise_dx *= -8.0f;
	dnoise_dy *= -8.0f;
	dnoise_dx += t40 * gx0 + t41 * gx1 + t42 * gx2;
	dnoise_dy += t40 * gy0 + t41 * gy1 + t42 * gy2;
	dnoise_dx *= 40.0f;
	dnoise_dy *= 40.0f;

	return vec3(70.175438596f * (n0 + n1 + n2), dnoise_dx, dnoise_dy);

	//Undef constants.
#undef F2
#undef G2
}

/*
*	Generates an octaved value in the range [-1.0f, 1.0f] at the given 2D coordinates.
*/
CATALYST_SHADER_FUNCTION_RETURN_TWO_ARGUMENTS(float, SimplexNoiseGenerateOctaved2D, vec2 coordinate, uint octaves)
{
	float final_noise = 0.0f;

	float total_noise = 0.0f;

	float current_amplitude = 1.0f;
	float current_frequency = 1.0f;

	for (uint i = 0; i < octaves; ++i)
	{
		final_noise += SimplexNoiseGenerate2D(coordinate * current_frequency) * current_amplitude;

		total_noise += current_amplitude;

		current_amplitude *= 0.5f;
		current_frequency *= 2.0f;
	}

	final_noise /= total_noise;

	return final_noise;
}

/*
*	Generates a value with derivatives in the range [-1.0f, 1.0f] at the given 2D coordinates, with octaves.
*/
CATALYST_SHADER_FUNCTION_RETURN_THREE_ARGUMENTS(float, SimplexNoiseGenerateDerivativesOctaved2D, vec2 coordinate, uint octaves, float derivatives_weight)
{
	float total = 0.0f;
	float maximum = 0.0f;
	float amplitude =  1.0f;
	float frequency = 1.0f;
	float derivative_x = 0.0f;
	float derivative_y = 0.0f;

	for (uint i = 0; i < octaves; ++i)
	{
		vec3 noise = SimplexNoiseGenerateDerivatives2D(coordinate * frequency);

		derivative_x += noise.y;
		derivative_y += noise.z;

		total += noise.x / CATALYST_SHADER_FUNCTION_LINEAR_INTERPOLATION(1.0f, 1.0f + derivative_x * derivative_x + derivative_y * derivative_y, derivatives_weight) * amplitude;

		maximum += amplitude;

		frequency *= 2.0f;
		amplitude *= 0.5f;
	}

	return total / maximum;
}

#endif