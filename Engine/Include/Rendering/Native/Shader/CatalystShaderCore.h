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
	using vec3 = Vector3<float>;
	using vec4 = Vector4<float>;

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
	*	Defines the beginning of a namespace.
	*/
	#define CATALYST_SHADER_NAMESPACE_BEGIN(NAMESPACE)

	/*
	*	Defines the end of a namespace.
	*/
	#define CATALYST_SHADER_NAMESPACE_END()

#endif

#endif