cmake_minimum_required(VERSION 3.11)
project(mtSoundboard)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Settings C/C++ standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

# find libs
add_subdirectory(../mtEngine3 mtEngine3)

# find_package(mtEngine3 CONFIG REQUIRED)

# Include any headers and c files to add to the executable
include_directories(
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/src
)
link_directories(
	${PROJECT_SOURCE_DIR}/src
)

file(GLOB_RECURSE soundboard_sources
	${PROJECT_SOURCE_DIR}/src/*.h
	${PROJECT_SOURCE_DIR}/src/*.hpp
	${PROJECT_SOURCE_DIR}/src/*.c
	${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_executable(${PROJECT_NAME} ${soundboard_sources} ${engine_sources})

target_include_directories(${PROJECT_NAME} PUBLIC ${engine_sources})
target_link_libraries(${PROJECT_NAME} PRIVATE
	mtEngine3
)
