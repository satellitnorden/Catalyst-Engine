cmake_minimum_required(VERSION 3.4.1)

file( GLOB_RECURSE ENGINESOURCE ../../../../Engine/Source/*.c ../../../../Engine/Source/*.cpp )
file( GLOB_RECURSE GAMESOURCE ../../Code/Source/*.cpp )

add_library(	catalyst-engine-lib
		        SHARED
		        ${ENGINESOURCE}
		        ${GAMESOURCE} )

include_directories( ../../../../Engine/Include/ ../../Code/Include/ )


target_link_libraries( catalyst-engine-lib android log vulkan )
add_definitions( -DCATALYST_ANDROID )

#target_link_libraries( catalyst-engine-lib android vulkan )
#add_definitions( -DCATALYST_ANDROID -DCATALYST_FINAL -Ofast )