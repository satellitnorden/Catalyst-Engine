cmake_minimum_required(VERSION 3.4.1)

file( GLOB_RECURSE ENGINESOURCE ../../../../Engine/Include/*.h ../../../../Engine/Source/*.c ../../../../Engine/Source/*.cpp )
file( GLOB_RECURSE GAMESOURCE ../../Code/Include/*.h ../../Code/Source/*.cpp )

add_library(	catalyst-engine-lib
		        SHARED
		        ${ENGINESOURCE}
		        ${GAMESOURCE} )

include_directories( ../../../../Engine/Include/ ../../Code/Include/ )

find_library( Android-Library android )
find_library( Log-Library log )
find_library( Vulkan-Library vulkan )

target_link_libraries( catalyst-engine-lib ${Android-Library} ${Log-Library} ${Vulkan-Library} )

add_definitions( -DCATALYST_ANDROID )