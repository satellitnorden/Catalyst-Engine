#if !defined(CATALYST_SHADER_CORE)
#define CATALYST_SHADER_CORE

#if defined(CATALYST_SHADER_LANGUAGE_CXX)
	
	//Core.
	#include <Core/Essential/CatalystEssential.h>

	//Math.
	#include <Math/General/Matrix.h>
	#include <Math/General/Vector.h>
	
	//Type aliases.
	using mat3 = Matrix3x3;
	using mat4 = Matrix4x4;
	using uint = unsigned int;
	using vec2 = Vector2<float>;
	using vec3 = Vector3<float>;
	using vec4 = Vector4<float>;
	using uvec2 = Vector2<uint32>;
	using uvec3 = Vector3<uint32>;

	/*
	*	Cross product helper function.
	*/
	template <typename TYPE>
	FORCE_INLINE static NO_DISCARD TYPE CrossProductHelper(const TYPE value_1, const TYPE value_2) NOEXCEPT
	{
		return TYPE::CrossProduct(value_1, value_2);
	}

	/*
	*	Dot product helper function.
	*/
	template <typename TYPE>
	FORCE_INLINE static NO_DISCARD float32 DotProductHelper(const TYPE value_1, const TYPE value_2) NOEXCEPT
	{
		return TYPE::DotProduct(value_1, value_2);
	}

	/*
	*	Floor helper function.
	*/
	template <typename TYPE>
	FORCE_INLINE static NO_DISCARD TYPE FloorHelper(const TYPE value) NOEXCEPT
	{
		return TYPE::Floor(value);
	}

	/*
	*	Floor helper function specialization for float32.
	*/
	template <>
	FORCE_INLINE static NO_DISCARD float32 FloorHelper<float32>(const float32 value) NOEXCEPT
	{
		return CatalystBaseMath::Floor<float32>(value);
	}

	/*
	*	Length helper function.
	*/
	template <typename TYPE>
	FORCE_INLINE static NO_DISCARD float32 LengthHelper(const TYPE &value) NOEXCEPT
	{
		return TYPE::Length(value);
	}

	/*
	*	Normalize helper function.
	*/
	template <typename TYPE>
	FORCE_INLINE static NO_DISCARD TYPE NormalizeHelper(const TYPE value) NOEXCEPT
	{
		return TYPE::Normalize(value);
	}

	/*
	*	Reflect helper function.
	*/
	template <typename TYPE>
	FORCE_INLINE static NO_DISCARD TYPE ReflectHelper(const TYPE value_1, const TYPE value_2) NOEXCEPT
	{
		return TYPE::Reflect(value_1, value_2);
	}

	/*
	*	Defines a constant value of the given type. with the given name and value.
	*/
	#define CATALYST_SHADER_CONSTANT(TYPE, NAME, VALUE)	\
	constexpr TYPE NAME{ VALUE };

	/*
	*	Calls the clamp function.
	*/
	#define CATALYST_SHADER_FUNCTION_CLAMP(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3) \
	CatalystBaseMath::Clamp(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3)

	/*
	*	Calls the cross product function.
	*/
	#define CATALYST_SHADER_FUNCTION_CROSS_PRODUCT(ARGUMENT_1, ARGUMENT_2) \
	CrossProductHelper(ARGUMENT_1, ARGUMENT_2)

	/*
	*	Calls the dot product function.
	*/
	#define CATALYST_SHADER_FUNCTION_DOT_PRODUCT(ARGUMENT_1, ARGUMENT_2) \
	DotProductHelper(ARGUMENT_1, ARGUMENT_2)

	/*
	*	Calls the floor function.
	*/
	#define CATALYST_SHADER_FUNCTION_FLOOR(ARGUMENT_1) \
	FloorHelper(ARGUMENT_1)

	/*
	*	Calls the fractional function.
	*/
	#define CATALYST_SHADER_FUNCTION_FRACTIONAL(ARGUMENT) \
	CatalystBaseMath::Fractional(ARGUMENT)

	/*
	*	Calls the length function.
	*/
	#define CATALYST_SHADER_FUNCTION_LENGTH(ARGUMENT_1) \
	LengthHelper(ARGUMENT_1)

	/*
	*	Calls the linear interpolation function.
	*/
	#define CATALYST_SHADER_FUNCTION_LINEAR_INTERPOLATION(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3) \
	CatalystBaseMath::LinearlyInterpolate(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3)

	/*
	*	Calls the minimum function.
	*/
	#define CATALYST_SHADER_FUNCTION_MINIMUM(ARGUMENT_1, ARGUMENT_2) \
	CatalystBaseMath::Minimum(ARGUMENT_1, ARGUMENT_2)

	/*
	*	Calls the maximum function.
	*/
	#define CATALYST_SHADER_FUNCTION_MAXIMUM(ARGUMENT_1, ARGUMENT_2) \
	CatalystBaseMath::Maximum(ARGUMENT_1, ARGUMENT_2)

	/*
	*	Calls the normalize function.
	*/
	#define CATALYST_SHADER_FUNCTION_NORMALIZE(ARGUMENT_1) \
	NormalizeHelper(ARGUMENT_1)

	/*
	*	Calls the power function.
	*/
	#define CATALYST_SHADER_FUNCTION_POWER(ARGUMENT_1, ARGUMENT_2) \
	powf(ARGUMENT_1, ARGUMENT_2)

	/*
	*	Calls the reflect function.
	*/
	#define CATALYST_SHADER_FUNCTION_REFLECT(ARGUMENT_1, ARGUMENT_2) \
	ReflectHelper(ARGUMENT_1, ARGUMENT_2)

	/*
	*	Calls the sine function.
	*/
	#define CATALYST_SHADER_FUNCTION_SINE(ARGUMENT) \
	CatalystBaseMath::Sine(ARGUMENT)

	/*
	*	Calls the smoothstep function.
	*/
	#define CATALYST_SHADER_FUNCTION_SMOOTHSTEP(ARGUMENT) \
	CatalystBaseMath::SmoothStep<1>(ARGUMENT)

	/*
	*	Calls the square function.
	*/
	#define CATALYST_SHADER_FUNCTION_SQUARE(ARGUMENT) \
	CatalystBaseMath::Square(ARGUMENT)

	/*
	*	Calls the squareroot function.
	*/
	#define CATALYST_SHADER_FUNCTION_SQUAREROOT(ARGUMENT) \
	CatalystBaseMath::SquareRoot(ARGUMENT)

	/*
	*	Defines a function with a return value and one argument.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_ONE_ARGUMENT(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1) \
	FORCE_INLINE NO_DISCARD RETURN_TYPE FUNCTION_NAME(ARGUMENT_1) NOEXCEPT

	/*
	*	Defines a function with a return value and two arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_TWO_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1, ARGUMENT_2) \
	FORCE_INLINE NO_DISCARD RETURN_TYPE FUNCTION_NAME(ARGUMENT_1, ARGUMENT_2) NOEXCEPT

	/*
	*	Defines a function with a return value and three arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_THREE_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1, ARGUMENT_2, ARGUMENT_3) \
	FORCE_INLINE NO_DISCARD RETURN_TYPE FUNCTION_NAME(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3) NOEXCEPT

	/*
	*	Defines a function with a return value and four arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_FOUR_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4) \
	FORCE_INLINE NO_DISCARD RETURN_TYPE FUNCTION_NAME(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4) NOEXCEPT

	/*
	*	Defines a function with a return value and five arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_FIVE_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5) \
	FORCE_INLINE NO_DISCARD RETURN_TYPE FUNCTION_NAME(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5) NOEXCEPT

	/*
	*	Defines a function with a return value and six arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_SIX_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6) \
	FORCE_INLINE NO_DISCARD RETURN_TYPE FUNCTION_NAME(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6) NOEXCEPT

	/*
	*	Defines a function with a return value and seven arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_SEVEN_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6, ARGUMENT_7) \
	FORCE_INLINE NO_DISCARD RETURN_TYPE FUNCTION_NAME(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6, ARGUMENT_7) NOEXCEPT

	/*
	*	Defines a function with a return value and eight arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_EIGHT_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6, ARGUMENT_7, ARGUMENT_8) \
	FORCE_INLINE NO_DISCARD RETURN_TYPE FUNCTION_NAME(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6, ARGUMENT_7, ARGUMENT_8) NOEXCEPT

	/*
	*	Defines a function with a return value and nine arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6, ARGUMENT_7, ARGUMENT_8, ARGUMENT_9) \
	FORCE_INLINE NO_DISCARD RETURN_TYPE FUNCTION_NAME(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6, ARGUMENT_7, ARGUMENT_8, ARGUMENT_9) NOEXCEPT

	/*
	*	Defines a function with a return value and ten arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_TEN_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6, ARGUMENT_7, ARGUMENT_8, ARGUMENT_9, ARGUMENT_10) \
	FORCE_INLINE NO_DISCARD RETURN_TYPE FUNCTION_NAME(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6, ARGUMENT_7, ARGUMENT_8, ARGUMENT_9, ARGUMENT_10) NOEXCEPT

	/*
	*	Defines the beginning of a namespace.
	*/
	#define CATALYST_SHADER_NAMESPACE_BEGIN(NAMESPACE)	\
	namespace NAMESPACE									\
	{

	/*
	*	Defines the end of a namespace.
	*/
	#define CATALYST_SHADER_NAMESPACE_END()	\
	}

	/*
	*	Defines an output argument.
	*/
	#define CATALYST_SHADER_OUTPUT_ARGUMENT(ARGUMENT_TYPE, ARGUMENT_NAME) ARGUMENT_TYPE &ARGUMENT_NAME

	/*
	*	Defines the shader function prefix.
	*/
	#define CATALYST_SHADER_FUNCTION_PREFIX FORCE_INLINE static

#endif

#if defined(CATALYST_SHADER_LANGUAGE_GLSL)

	/*
	*	Defines a constant value of the given type. with the given name and value.
	*/
	#define CATALYST_SHADER_CONSTANT(TYPE, NAME, VALUE)	\
	const TYPE NAME = VALUE;

	/*
	*	Calls the clamp function.
	*/
	#define CATALYST_SHADER_FUNCTION_CLAMP(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3) \
	clamp(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3)

	/*
	*	Calls the cross product function.
	*/
	#define CATALYST_SHADER_FUNCTION_CROSS_PRODUCT(ARGUMENT_1, ARGUMENT_2) \
	cross(ARGUMENT_1, ARGUMENT_2)

	/*
	*	Calls the dot product function.
	*/
	#define CATALYST_SHADER_FUNCTION_DOT_PRODUCT(ARGUMENT_1, ARGUMENT_2) \
	dot(ARGUMENT_1, ARGUMENT_2)

	/*
	*	Calls the floor function.
	*/
	#define CATALYST_SHADER_FUNCTION_FLOOR(ARGUMENT_1) \
	floor(ARGUMENT_1)

	/*
	*	Calls the fractional function.
	*/
	#define CATALYST_SHADER_FUNCTION_FRACTIONAL(ARGUMENT) \
	fract(ARGUMENT)

	/*
	*	Calls the length function.
	*/
	#define CATALYST_SHADER_FUNCTION_LENGTH(ARGUMENT_1) \
	length(ARGUMENT_1)

	/*
	*	Calls the linear interpolation function.
	*/
	#define CATALYST_SHADER_FUNCTION_LINEAR_INTERPOLATION(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3) \
	mix(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3)

	/*
	*	Calls the minimum function.
	*/
	#define CATALYST_SHADER_FUNCTION_MINIMUM(ARGUMENT_1, ARGUMENT_2) \
	min(ARGUMENT_1, ARGUMENT_2)

	/*
	*	Calls the maximum function.
	*/
	#define CATALYST_SHADER_FUNCTION_MAXIMUM(ARGUMENT_1, ARGUMENT_2) \
	max(ARGUMENT_1, ARGUMENT_2)

	/*
	*	Calls the normalize function.
	*/
	#define CATALYST_SHADER_FUNCTION_NORMALIZE(ARGUMENT_1) \
	normalize(ARGUMENT_1)

	/*
	*	Calls the power function.
	*/
	#define CATALYST_SHADER_FUNCTION_POWER(ARGUMENT_1, ARGUMENT_2) \
	pow(ARGUMENT_1, ARGUMENT_2)

	/*
	*	Calls the reflect function.
	*/
	#define CATALYST_SHADER_FUNCTION_REFLECT(ARGUMENT_1, ARGUMENT_2) \
	reflect(ARGUMENT_1, ARGUMENT_2)

	/*
	*	Calls the sine function.
	*/
	#define CATALYST_SHADER_FUNCTION_SINE(ARGUMENT) \
	sin(ARGUMENT)

	/*
	*	Calls the smoothstep function.
	*/
	#define CATALYST_SHADER_FUNCTION_SMOOTHSTEP(ARGUMENT) \
	SmoothStep(ARGUMENT)

	/*
	*	Calls the square function.
	*/
	#define CATALYST_SHADER_FUNCTION_SQUARE(ARGUMENT) \
	Square(ARGUMENT)

	/*
	*	Calls the squareroot function.
	*/
	#define CATALYST_SHADER_FUNCTION_SQUAREROOT(ARGUMENT) \
	sqrt(ARGUMENT)

	/*
	*	Defines a function with a return value and one argument.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_ONE_ARGUMENT(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1) \
	RETURN_TYPE FUNCTION_NAME(ARGUMENT_1)

	/*
	*	Defines a function with a return value and two arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_TWO_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1, ARGUMENT_2) \
	RETURN_TYPE FUNCTION_NAME(ARGUMENT_1, ARGUMENT_2)

	/*
	*	Defines a function with a return value and three arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_THREE_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1, ARGUMENT_2, ARGUMENT_3) \
	RETURN_TYPE FUNCTION_NAME(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3)

	/*
	*	Defines a function with a return value and four arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_FOUR_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4) \
	RETURN_TYPE FUNCTION_NAME(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4)

	/*
	*	Defines a function with a return value and five arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_FIVE_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5) \
	RETURN_TYPE FUNCTION_NAME(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5)

	/*
	*	Defines a function with a return value and six arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_SIX_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6) \
	RETURN_TYPE FUNCTION_NAME(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6)

	/*
	*	Defines a function with a return value and seven arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_SEVEN_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6, ARGUMENT_7) \
	RETURN_TYPE FUNCTION_NAME(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6, ARGUMENT_7)

	/*
	*	Defines a function with a return value and eight arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_EIGHT_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6, ARGUMENT_7, ARGUMENT_8) \
	RETURN_TYPE FUNCTION_NAME(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6, ARGUMENT_7, ARGUMENT_8)

	/*
	*	Defines a function with a return value and nine arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6, ARGUMENT_7, ARGUMENT_8, ARGUMENT_9) \
	RETURN_TYPE FUNCTION_NAME(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6, ARGUMENT_7, ARGUMENT_8, ARGUMENT_9)

	/*
	*	Defines a function with a return value and ten arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_TEN_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6, ARGUMENT_7, ARGUMENT_8, ARGUMENT_9, ARGUMENT_10) \
	RETURN_TYPE FUNCTION_NAME(ARGUMENT_1, ARGUMENT_2, ARGUMENT_3, ARGUMENT_4, ARGUMENT_5, ARGUMENT_6, ARGUMENT_7, ARGUMENT_8, ARGUMENT_9, ARGUMENT_10)

	/*
	*	Defines the beginning of a namespace.
	*/
	#define CATALYST_SHADER_NAMESPACE_BEGIN(NAMESPACE)

	/*
	*	Defines the end of a namespace.
	*/
	#define CATALYST_SHADER_NAMESPACE_END()

	/*
	*	Defines an output argument.
	*/
	#define CATALYST_SHADER_OUTPUT_ARGUMENT(ARGUMENT_TYPE, ARGUMENT_NAME) out ARGUMENT_TYPE ARGUMENT_NAME

	/*
	*	Defines the shader function prefix.
	*/
	#define CATALYST_SHADER_FUNCTION_PREFIX

#endif

///////////////////////
// Shared functions. //
///////////////////////

/*
*	Converts float bits to uint bits.
*/
CATALYST_SHADER_FUNCTION_PREFIX uint CatalystShaderFloatBitsToUint(float value)
{
#if defined(CATALYST_SHADER_LANGUAGE_CXX)
	uint output;

	Memory::Copy(&output, &value, sizeof(uint));

	return output;
#endif

#if defined(CATALYST_SHADER_LANGUAGE_GLSL)
	return floatBitsToUint(value);
#endif
}

/*
*	Returns the hash of the given seed.
*/
CATALYST_SHADER_FUNCTION_PREFIX uint CatalystShaderHash1(uint seed)
{
	seed = (seed ^ 61u) ^ (seed >> 16u);
	seed *= 9u;
	seed = seed ^ (seed >> 4u);
	seed *= 0x27d4eb2du;
	seed = seed ^ (seed >> 15u);
	return seed;
}

/*
*   Returns the inverse square of the given number.
*/
CATALYST_SHADER_FUNCTION_PREFIX float CatalystShaderInverseSquare(float number)
{
	return 1.0f - ((1.0f - number) * (1.0f - number));
}

/*
*	Returns a random float with the given coordinate.
*/
CATALYST_SHADER_FUNCTION_PREFIX float CatalystShaderRandomFloat(vec2 coordinate)
{
	uint hash = CatalystShaderHash1(CatalystShaderFloatBitsToUint(coordinate.x)) ^ CatalystShaderHash1(CatalystShaderFloatBitsToUint(coordinate.y)) ^ CatalystShaderHash1(CatalystShaderFloatBitsToUint(coordinate.x + coordinate.y));

	return float(hash) * (2.328306437080797e-10f);
}

/*
*   Returns a smoothed number in the range 0.0f-1.0f.
*/
CATALYST_SHADER_FUNCTION_PREFIX float CatalystShaderSmoothStep(float number)
{
	return number * number * (3.0f - 2.0f * number);
}

/*
*   Returns a smoothed number in the range 0.0f-1.0f.
*/
CATALYST_SHADER_FUNCTION_PREFIX float CatalystShaderSmootherStep(float number)
{
	return number * number * number * (number * (number * 6.0f - 15.0f) + 10.0f);
}

/*
*   Returns a smoothed number in the given range.
*/
CATALYST_SHADER_FUNCTION_PREFIX float CatalystShaderRealSmoothStep(float number, float minimum, float maximum)
{
	number = CATALYST_SHADER_FUNCTION_CLAMP((number - minimum) / (maximum - minimum), 0.0f, 1.0f);

	return number * number * (3.0f - 2.0f * number);
}

#endif