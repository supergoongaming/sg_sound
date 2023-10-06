# #########################################
# Initial config
# #########################################
cmake_minimum_required(VERSION 3.16)
message(STATUS "Starting Supergoon Sount Initialization")
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# This exports a file that clangd can use so that we know what compile/include flags so that we can use clangd language server properly with our cmake attributes.
message(STATUS "Project initializing")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
include(FindPkgConfig)
project(supergoonSound
    VERSION 0.1.0
    LANGUAGES C)

# #########################################
# File globbing
# #########################################
message(STATUS "Getting all files for cmake..")

# file(GLOB_RECURSE FILES src/*.c* src/*.h* external/mojoAL/mojoal.c)
file(GLOB_RECURSE FILES src/*.c* src/*.h*)

# #########################################
# Platform specific Project setup
# #########################################
message(STATUS "Changing the platform specific settings")

if(EMSCRIPTEN)
    message(STATUS "Configuring Emscripten")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

elseif(APPLE)
    # MACOS
    if(NOT IOS)
        message(STATUS "Configuring Macos")
        set(MACOS ON)
        list(FILTER FILES EXCLUDE REGEX ".*platform/Windows.*$")

        if(CMAKE_GENERATOR STREQUAL "Xcode")
            message(STATUS "Setting Build to Universal Binary")
            set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "" FORCE)
        endif(CMAKE_GENERATOR STREQUAL "Xcode")
    endif(NOT IOS)
elseif(WIN32)
    message(STATUS "Configuring Windows")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /c/cmake)
    set(GOON_ENABLE_TESTING OFF)
    list(FILTER FILES EXCLUDE REGEX ".*platform/Mac.*$")
elseif(UNIX AND NOT APPLE)
    message(STATUS "Configuring Linux")
    set(LINUX ON)
endif(EMSCRIPTEN)

# #########################################
# Initialize
# #########################################
message(STATUS "Initializing the project")
add_executable(supergoonSound
    ${FILES})

# Using 11 instead of 99 for forward declaring typedef
set_property(TARGET supergoonSound PROPERTY C_STANDARD 11)
find_package(PkgConfig REQUIRED)

# #########################################
# Options
# #########################################
option(CMAKE_DEBUG_VARIABLES "Runs a debug on all variables for troubleshooting" OFF)
option(GOON_BUILD_PCH "Uses a PCH file to try and speed up compilation" ON)

# option(GOON_FULL_MACOS_BUILD "Full builds of all libraries, used for runners mostly, and passed in to override." OFF)
option(GOON_BUILD_TESTING "Should we enable testing" OFF)
set(BUILD_TESTING GOON_BUILD_TESTING)

# #########################################
# Packages
# #########################################
# Most dev is done here, so to speed up builds don't rebuild all packages, must have them installed.
# if(MACOS AND NOT GOON_FULL_MACOS_BUILD)
    # 1. Look for a SDL2 package, 2. look for the SDL2 component and 3. fail if none can be found
    find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

    find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

    find_library(LIB_VORBIS
        REQUIRED
        PATH_SUFFIXES .a
        NAMES libvorbis vorbis vorbis.a vorbis.dll libvorbis.a
        HINTS /usr/local/lib /c/cmake/lib)

    find_library(LIB_VORBISFILE
        REQUIRED
        PATH_SUFFIXES .a
        NAMES libvorbisfile vorbisfile vorbisfile.a vorbisfile.dll
        HINTS /usr/local/lib)

    find_package(
        OGG
        NAMES ogg Ogg
        PATH_SUFFIXES .a
        REQUIRED)

    # Build everything, useful for runners, and builds we don't care take a little longer.
    # else()
    # find_package(Git REQUIRED QUIET)
    # message(STATUS "Cloning and updating submodules")
    # if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
    # WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    # RESULT_VARIABLE GIT_SUBMOD_RESULT)

    # if(NOT GIT_SUBMOD_RESULT EQUAL "0")
    # message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    # endif()

    # add_subdirectory(./external/SDL EXCLUDE_FROM_ALL)
    # add_subdirectory(./external/SDL_image EXCLUDE_FROM_ALL)
    # add_subdirectory(./external/Lua EXCLUDE_FROM_ALL)

    # target_link_libraries(SupergoonDash PRIVATE lua_static)
    # else()
    # message(STATUS "Could not init git for some reason")
    # endif()

    # Mojoal
    target_sources(supergoonSound PRIVATE external/mojoAL/mojoal.c)
    set_source_files_properties(external/mojoAL/mojoal.c PROPERTIES COMPILE_FLAGS -w) # Hide warnings
# endif(MACOS AND NOT GOON_FULL_MACOS_BUILD)

# #########################################
# Compile Definitions
# #########################################
if(APPLE)
    target_compile_definitions(supergoonSound PRIVATE -DGN_PLATFORM_MACOS)
elseif(WIN32)
    target_compile_definitions(supergoonSound PRIVATE -DGN_PLATFORM_WINDOWS)
elseif(LINUX)
    target_compile_definitions(supergoonSound PRIVATE -DGN_PLATFORM_LINUX)
endif(APPLE)

if(GOON_DEBUG_LUA)
    target_compile_definitions(supergoonSound PRIVATE -DGN_DEBUG_LUA)
endif(GOON_DEBUG_LUA)

# #########################################
# Precompiled headers
# #########################################
if(GOON_BUILD_PCH)
    message(STATUS "Setting up PCH, mac needs objc")
    target_precompile_headers(supergoonSound PRIVATE
        "$<$<COMPILE_LANGUAGE:C>:${CMAKE_CURRENT_SOURCE_DIR}/src/SupergoonSound/gnpch.h>")
endif(GOON_BUILD_PCH)

# #########################################
# Debug
# #########################################
if(CMAKE_DEBUG_VARIABLES)
    message(STATUS "Exporting debug")
    get_cmake_property(_variableNames VARIABLES)
    list(SORT _variableNames)

    foreach(_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endif(CMAKE_DEBUG_VARIABLES)

if(EMSCRIPTEN)
    set_target_properties(supergoonSound PROPERTIES LINK_FLAGS ${LINK_FLAGS} "-O0 -g -s SAFE_HEAP=1 -s STACK_OVERFLOW_CHECK=1 --preload-file ${CMAKE_SOURCE_DIR}/assets@/assets")
endif(EMSCRIPTEN)

# #########################################
# Compile Options
# #########################################
if(WIN32)
    target_compile_options(supergoonSound PRIVATE
        /std:c11
    )
else()
    target_compile_options(supergoonSound PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wdeprecated
        -Wno-newline-eof
    )
endif(WIN32)

# #########################################
# Linking Options
# #########################################

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(supergoonSound PRIVATE SDL2::SDL2main)
endif()

# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(supergoonSound
    PUBLIC
    PRIVATE
    SDL2::SDL2-static
    ${LIB_VORBISFILE}
    ${LIB_VORBIS}
    ${OGG_LIBRARIES}
)

target_include_directories(supergoonSound
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/external/mojoAL
)

# #########################################
# Install
# #########################################
set(FOLDERS_TO_COPY assets)

if(MSVC OR CMAKE_GENERATOR STREQUAL "Xcode")
    set(INSTALL_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>)
else()
    set(INSTALL_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif(MSVC OR CMAKE_GENERATOR STREQUAL "Xcode")

install(DIRECTORY ${FOLDERS_TO_COPY}
    DESTINATION ${INSTALL_DIR}
    PATTERN "*.tsx" EXCLUDE
    PATTERN "*.tmx" EXCLUDE
    PATTERN "*.tiled-project" EXCLUDE
)