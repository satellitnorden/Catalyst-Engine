#pragma once

/*
*	Determine the compiler.
*/
#if defined(_MSC_VER)
	#define CATALYST_MSVC
#elif defined(__clang__)
	#define CATALYST_CLANG
#endif

/*
*	Determine which version of C++ is being compiled.
*/
#if __cplusplus >= 199711L
	#define CATALYST_CXX98
#endif

#if __cplusplus >= 201103L
	#define CATALYST_CXX11
#endif

#if __cplusplus >= 201402L
	#define CATALYST_CXX14
#endif

#if __cplusplus >= 201703L
	#define CATALYST_CXX17
#endif

#if __cplusplus >= 202000L //Temporary!
	#define CATALYST_CXX20
#endif