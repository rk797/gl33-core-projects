cmake_minimum_required(VERSION 3.15.0)
project(453-skeleton)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules/" ${CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Use modern C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(OpenGL_GL_PREFERENCE GLVND)

#-------------------------------------------------------------------------------
# https://github.com/adishavit/argh/releases/tag/v1.3.1
include_directories(SYSTEM thirdparty/argh-1.3.1/)

#-------------------------------------------------------------------------------
# https://glad.dav1d.de/
add_subdirectory(thirdparty/glad-opengl-3.3-core)
# This is required to access the optional shader stages (Possibly won't work on MacOS) You don't need it for this assignment.
#add_subdirectory(thirdparty/glad-opengl-4.6-core) 
set(LIBRARIES ${LIBRARIES} glad)

#-------------------------------------------------------------------------------
# https://www.glfw.org/

# Turn off building their docs/tests/examples.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(thirdparty/glfw-3.4)
set(LIBRARIES ${LIBRARIES} glfw)

#-------------------------------------------------------------------------------
# https://glm.g-truc.net/0.9.9/index.html
include_directories(SYSTEM thirdparty/glm-0.9.9.7/)

#-------------------------------------------------------------------------------
# https://fmt.dev/latest/index.html
set(FMT_TES OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/fmt-11.2.0)
set(LIBRARIES ${LIBRARIES} fmt::fmt)
include_directories(SYSTEM thirdparty/fmt-11.2.0/include)

#-------------------------------------------------------------------------------
include_directories(SYSTEM thirdparty/stb-2.26)

#-------------------------------------------------------------------------------
include_directories(SYSTEM thirdparty/imgui-commit-7b365fa)
include_directories(SYSTEM thirdparty/imgui-commit-7b365fa/backends)

#-------------------------------------------------------------------------------
find_package(OpenGL REQUIRED)
set(LIBRARIES ${LIBRARIES} ${OPENGL_gl_LIBRARY})


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	list(APPEND _453_CMAKE_CXX_FLAGS ${_453_CMAKE_CXX_FLAGS} "-Wall" "-pedantic")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	# using GCC
	list(APPEND _453_CMAKE_CXX_FLAGS ${_453_CMAKE_CXX_FLAGS} "-fdiagnostics-color=always" "-Wall" "-pedantic")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
	# using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	add_compile_options("/permissive-")
	add_compile_options("/Zc:preprocessor")
	# Remove the default warning level.
	# https://stackoverflow.com/questions/58708772/cmake-project-in-visual-studio-gives-flag-override-warnings-command-line-warnin
	string(REGEX REPLACE "/W[3|4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	 # The above preprocessor warns about certain things in windows headers.
	 # Turn those off: https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/c5105?view=vs-2019
	add_compile_options("/wd5105")

	set(_453_CMAKE_CXX_FLAGS ${_453_CMAKE_CXX_FLAGS} /W3)

endif()

if(APPLE)
	set(LIBRARIES ${LIBRARIES} pthread dl)
elseif(UNIX)
	set(LIBRARIES ${LIBRARIES} pthread GL dl)
elseif(WIN32)
endif()

add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD=ON)
set(IMGUI_CPP_FILES
	thirdparty/imgui-commit-7b365fa/*.cpp							# All main files need to be compiled
	thirdparty/imgui-commit-7b365fa/backends/imgui_impl_glfw.cpp	# Window Library comp.
	thirdparty/imgui-commit-7b365fa/backends/imgui_impl_opengl3.cpp # OpenGL API >=3.0 
)

# Compile our main application
file(GLOB SOURCES
    453-skeleton/*
    453-skeleton/game/*
    453-skeleton/time/*
	${IMGUI_CPP_FILES}
)
set(INCLUDES ${INCLUDES})
set(APP_NAME "453-skeleton")

add_compile_definitions("ASSET_DIR=${CMAKE_SOURCE_DIR}/assets")

add_executable(${APP_NAME} ${SOURCES}  "453-skeleton/usr/Globals.h" "453-skeleton/usr/Transformation.h" "453-skeleton/usr/Transormation.cpp" "453-skeleton/usr/LocalPlayer.h" "453-skeleton/usr/Line.h" "453-skeleton/usr/Cannonball.h" "453-skeleton/usr/LocalPlayer.cpp" "453-skeleton/usr/Cannonball.cpp" "453-skeleton/usr/EnemyShip.h" "453-skeleton/usr/EnemyShip.cpp")
target_include_directories(${APP_NAME} PRIVATE ${INCLUDES})
target_link_libraries(${APP_NAME} ${LIBRARIES})
target_compile_definitions(${APP_NAME} PRIVATE ${DEFINITIONS})
target_compile_options(${APP_NAME} PRIVATE ${_453_CMAKE_CXX_FLAGS})
set_target_properties(${APP_NAME} PROPERTIES INSTALL_RPATH "./" BUILD_RPATH "./")
