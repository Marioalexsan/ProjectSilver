# Project Silver CMakeLists.txt
cmake_minimum_required(VERSION 3.26)

project(ProjectSilver)
include(FetchContent)

# Fetch SFML 3.0 dependency
FetchContent_Declare(SFML
	GIT_REPOSITORY https://github.com/SFML/SFML
	GIT_TAG a71d6bc078d653c719e7ad96b3812028a81749b2)
FetchContent_MakeAvailable(SFML)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Enable all warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
	add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wold-style-cast)
elseif(MSVC)
	add_compile_options(/W4 /permissive- /wd4251 /wd4201)
endif()

add_subdirectory(ProjectSilver)