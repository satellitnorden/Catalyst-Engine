cmake_minimum_required(VERSION 3.4.1)

file( GLOB_RECURSE ENGINESOURCE ../../Engine/Source/*.cpp )

include_directories(	../../Engine/Include/
			../../Game/Clairvoyant/Code/Include/ )

add_library(	catalyst-engine-lib
		SHARED
		${ENGINESOURCE} )

set_target_properties(	catalyst-engine-lib
			PROPERTIES LINKER_LANGUAGE
			CXX )

file( GLOB_RECURSE GAMESOURCE ../../Game/Clairvoyant/Code/Source/*.cpp )

add_library(	game-lib
		SHARED
		${GAMESOURCE} )

set_target_properties(	game-lib
			PROPERTIES LINKER_LANGUAGE
			CXX )

add_definitions( -DCATALYST_ANDROID )