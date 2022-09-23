cmake_minimum_required(VERSION 3.11)
project(main)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Settings C/C++ standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

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
link_directories(
	${PROJECT_SOURCE_DIR}/src
)

file(GLOB_RECURSE main_sources
	${PROJECT_SOURCE_DIR}/src/*.h
	${PROJECT_SOURCE_DIR}/src/*.hpp
	${PROJECT_SOURCE_DIR}/src/*.c
	${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_executable(${PROJECT_NAME} ${main_sources})

if(UNIX)
	target_link_libraries(${PROJECT_NAME}
		SDL2::SDL2
		SDL2::SDL2main
		SDL2::SDL2-static
		SDL2::SDL2_image
		
		yaml-cpp
		
		$<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
		
	)
else()
	target_link_libraries(${PROJECT_NAME}
		SDL2::SDL2
		SDL2::SDL2main
		SDL2::SDL2_image
		
		yaml-cpp
		
		$<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
		
	)
endif()
