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
# Options
# #########################################
option(CMAKE_DEBUG_VARIABLES "Runs a debug on all variables for troubleshooting" ON)
option(GOON_BUILD_PCH "Uses a PCH file to try and speed up compilation" ON)
option(BUILD_SDL_TEST "Builds a test suite instead of a library" OFF)

# option(GOON_FULL_MACOS_BUILD "Full builds of all libraries, used for runners mostly, and passed in to override." OFF)
option(GOON_BUILD_TESTING "Should we enable testing" OFF)
set(BUILD_TESTING GOON_BUILD_TESTING)

# #########################################
# Initialize
# #########################################
message(STATUS "Initializing the project")

if(BUILD_SDL_TEST)
    add_executable(supergoonSound
        ${FILES})
else()
    add_library(supergoonSound
        STATIC
        ${FILES})
endif(BUILD_SDL_TEST)

# Using 11 instead of 99 for forward declaring typedef
set_property(TARGET supergoonSound PROPERTY C_STANDARD 11)
find_package(PkgConfig REQUIRED)

# #########################################
# Packages
# #########################################
# Most dev is done here, so to speed up builds don't rebuild all packages, must have them installed.
if(MACOS AND NOT GOON_FULL_MACOS_BUILD)
    find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

# Build everything, useful for runners, and builds we don't care take a little longer.
else()
    # find_package(Git REQUIRED QUIET)
    message(STATUS "Cloning and updating submodules")

    # if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
    # WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    # RESULT_VARIABLE GIT_SUBMOD_RESULT)

    # if(NOT GIT_SUBMOD_RESULT EQUAL "0")
    # message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    # endif()

    # # Do not as emscripten
    # if(BUILD_SDL_TEST AND NOT EMSCRIPTEN)
    # add_subdirectory(./external/SDL)
    # endif(BUILD_SDL_TEST AND NOT EMSCRIPTEN)

    # Remove exclude from all, as this is needed for vorbis
    # add_subdirectory(./external/ogg)
    # add_subdirectory(./external/vorbis)

    # else()
    # message(STATUS "Could not init git for some reason")
    # endif()
endif(MACOS AND NOT GOON_FULL_MACOS_BUILD)

# Mojoal
target_sources(supergoonSound PRIVATE external/mojoAL/mojoal.c)
set_source_files_properties(external/mojoAL/mojoal.c PROPERTIES COMPILE_FLAGS -w) # Hide warnings

# #########################################
# Compile Definitions
# #########################################
if(APPLE)
    target_compile_definitions(supergoonSound PRIVATE -DGN_PLATFORM_MACOS -DAL_LIBTYPE_STATIC)
elseif(WIN32)
    target_compile_definitions(supergoonSound PRIVATE -DGN_PLATFORM_WINDOWS -DAL_LIBTYPE_STATIC)
elseif(LINUX)
    target_compile_definitions(supergoonSound PRIVATE -DGN_PLATFORM_LINUX -DAL_LIBTYPE_STATIC)
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

if(BUILD_SDL_TEST AND EMSCRIPTEN)
    # If we want to build SDL2, remove use_sdl
    set_target_properties(supergoonSound PROPERTIES LINK_FLAGS ${LINK_FLAGS} "-O0 -g -s SAFE_HEAP=1 -sUSE_SDL=2 -s STACK_OVERFLOW_CHECK=1 --preload-file ${CMAKE_SOURCE_DIR}/assets@/assets")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -sUSE_SDL=2 -s 'DEFAULT_LIBRARY_FUNCS_TO_INCLUDE=[\"$autoResumeAudioContext\",\"$dynCall\"]'")
endif(BUILD_SDL_TEST AND EMSCRIPTEN)

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
        -Wno-sign-compare
    )
endif(WIN32)

# #########################################
# Linking Options
# #########################################

# We include sdl here, and make it public so others can use it too.
if(MACOS AND NOT GOON_FULL_MACOS_BUILD)
    target_link_libraries(supergoonSound PUBLIC SDL2::SDL2-static)
endif(MACOS AND NOT GOON_FULL_MACOS_BUILD)

target_include_directories(supergoonSound
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/external/mojoAL

    # Needed when building as library
    ${CMAKE_BINARY_DIR}/external/SDL/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/include
)

# #########################################
# Install
# #########################################
if(BUILD_SDL_TEST)
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
endif(BUILD_SDL_TEST)

# Should install this somewhere
if(INSTALL_SG_SOUND)
    set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
    set(INSTALL_INC_DIR include CACHE PATH "Installation directory for headers")
    install(TARGETS supergoonSound
        EXPORT supergoonSoundTargets
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
        INCLUDES DESTINATION ${INSTALL_INC_DIR})
    install(EXPORT supergoonSoundTargets
        FILE supergoonSoundTargets.cmake
        NAMESPACE supergoonSound::
        DESTINATION lib/cmake/supergoonSound)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/
        DESTINATION ${INSTALL_INC_DIR}
        FILES_MATCHING PATTERN "*.h*")
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/supergoonSoundConfigVersion.cmake
        VERSION 0.1.0
        COMPATIBILITY SameMajorVersion
    )
    configure_package_config_file(
        ${CMAKE_CURRENT_LIST_DIR}/supergoonSoundConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/supergoonSoundConfig.cmake
        INSTALL_DESTINATION lib/cmake/supergoonSound
    )
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/supergoonSoundConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/supergoonSoundConfigVersion.cmake
        DESTINATION lib/cmake/supergoonSound
    )
endif(INSTALL_SG_SOUND)
