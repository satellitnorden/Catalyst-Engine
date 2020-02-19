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
	*	Defines a function with a return value and arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENTS) \
	FORCE_INLINE NO_DISCARD RETURN_TYPE FUNCTION_NAME(ARGUMENTS) NOEXCEPT

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
	*	Defines a function with a return value and arguments.
	*/
	#define CATALYST_SHADER_FUNCTION_RETURN_ARGUMENTS(RETURN_TYPE, FUNCTION_NAME, ARGUMENTS) \
	RETURN_TYPE FUNCTION_NAME(ARGUMENTS)

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