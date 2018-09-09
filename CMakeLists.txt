cmake_minimum_required(VERSION 2.6)
project(samviewer)

set(CMAKE_CXX_STANDARD 11)

set(PROJECT_NAME_CAPITALIZED "SAM-Viewer")
set(PROJECT_LINK_URL "https://github.com/stujones11/SAM-Viewer")
set(PROJECT_LINK_TEXT "github.com/stujones11/SAM-Viewer")

set(VERSION_MAJOR 0)
set(VERSION_MINOR 2)
set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}")

add_definitions(-DUSE_CMAKE_CONFIG_H)

configure_file (
	"${PROJECT_SOURCE_DIR}/src/cmake_config.h.in"
	"${PROJECT_SOURCE_DIR}/src/cmake_config.h"
)

if(NOT IRRLICHT_INCLUDE_DIR)
	find_path(IRRLICHT_INCLUDE_DIR NAMES irrlicht.h
		PATHS
		/usr/local/include/irrlicht
		/usr/include/irrlicht
		/system/develop/headers/irrlicht #Haiku
	)
endif()

if(NOT IRRLICHT_LIBRARY)
	find_library(IRRLICHT_LIBRARY NAMES libIrrlicht.so libIrrlicht.a Irrlicht
		PATHS
		/usr/local/lib
		/usr/lib
		/system/develop/lib # Haiku
	)
endif()

include_directories(
   ${IRRLICHT_INCLUDE_DIR}
   ${PROJECT_SOURCE_DIR}/src
)
file(GLOB SRCS src/*.cpp src/*.c)
file(MAKE_DIRECTORY "bin")

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pthread")

add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} ${IRRLICHT_LIBRARY})

