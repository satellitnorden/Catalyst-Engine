cmake_minimum_required(VERSION 3.4.1)

file( GLOB_RECURSE ENGINESOURCE ../../Engine/Include/*.h ../../Engine/Source/*.cpp )
file( GLOB_RECURSE ENGINELIBRARIES ../../Engine/Libraries/Static/*.lib )
file( GLOB_RECURSE GAMESOURCE ../../Game/Clairvoyant/Code/Include/*.h ../../Game/Clairvoyant/Code/Source/*.cpp )

add_library(	catalyst-engine-lib
		SHARED
		${ENGINESOURCE}
		${GAMESOURCE} )

include_directories( ../../Engine/Include/ ../../Game/Clairvoyant/Code/Include/ )

find_library( Vulkan-Library vulkan )

target_link_libraries( catalyst-engine-lib ${Vulkan-Library} )

add_definitions( -DCATALYST_ANDROID )