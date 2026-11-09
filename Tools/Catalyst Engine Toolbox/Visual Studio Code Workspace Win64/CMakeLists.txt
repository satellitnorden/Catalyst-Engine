# Set the minimum required version.
cmake_minimum_required(VERSION 4.4.5)

# Add the the project.
project("Catalyst Engine Toolbox" LANGUAGES C CXX)

# Set the C++ standard.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the configuration types.
set(CMAKE_CONFIGURATION_TYPES
	Debug
	Release
)

# Set the source files.
file(GLOB_RECURSE SOURCE_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/../Code/*.cpp"
)

# Add the the executable.
add_executable(CatalystEngineToolbox ${SOURCE_FILES})

# Add the include paths.
target_include_directories(CatalystEngineToolbox PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Code")

# Set MSVC-specific flags.
if (MSVC)
	# Set compiler flags.
	set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od")
	# Set linker flags.
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG /INCREMENTAL")

	# Set compiler flags.
	set(CMAKE_CXX_FLAGS_RELEASE "/Zi /O2 /GL")
	# Set linker flags.
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/DEBUG /LTCG:incremental /INCREMENTAL:NO")
endif()