cmake_minimum_required(VERSION 3.11)
project(main)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Settings C/C++ standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# find libs
find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)


# Include any headers and c files to add to the executable
include_directories(
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/src
)

file(GLOB main_sources
	"${PROJECT_SOURCE_DIR}/headers/*.h"
	"${PROJECT_SOURCE_DIR}/headers/*.hpp"
	"${PROJECT_SOURCE_DIR}/src/*.c"
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${PROJECT_NAME} ${main_sources})

target_link_libraries(${PROJECT_NAME} 
	SDL2::SDL2 
	SDL2::SDL2main
	SDL2::SDL2-static
	SDL2::SDL2_image
	
	yaml-cpp
	
	$<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
)