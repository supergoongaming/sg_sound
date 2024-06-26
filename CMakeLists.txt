# #########################################
# Initial config
# #########################################
cmake_minimum_required(VERSION 3.16)
message(STATUS "Starting Supergoon Sound Initialization")
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# This exports a file that clangd can use so that we know what compile/include flags so that we can use clangd language server properly with our cmake attributes.
message(STATUS "Project initializing")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
include(FindPkgConfig)
project(supergoonSound
    VERSION 1.0.0
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
option(INSTALL_SG_SOUND "Installs SG sound" ON)

# option(GOON_FULL_MACOS_BUILD "Full builds of all libraries, used for runners mostly, and passed in to override." OFF)

# #########################################
# Initialize
# #########################################
message(STATUS "Initializing the project")

add_library(supergoonSound
    STATIC
    ${FILES})

# Using 11 instead of 99 for forward declaring typedef
set_property(TARGET supergoonSound PROPERTY C_STANDARD 11)
# find_package(PkgConfig REQUIRED)

# #########################################
# Packages
# #########################################
# Most dev is done here, so to speed up builds don't rebuild all packages, must have them installed.
if(MACOS AND NOT GOON_FULL_MACOS_BUILD)
    find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
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
    find_library(LIB_OGG
        REQUIRED
        PATH_SUFFIXES .a .dll
        NAMES ogg Ogg
        HINTS /usr/local/lib)

# Build everything, useful for runners, and builds we don't care take a little longer.
else()
    find_package(Git REQUIRED QUIET)
    message(STATUS "Cloning and updating submodules")

    # Do not as emscripten
    if(NOT EMSCRIPTEN)
        add_subdirectory(./external/SDL)
        add_subdirectory(./external/ogg)
        add_subdirectory(./external/vorbis)
    endif(NOT EMSCRIPTEN)

    # Remove exclude from all, as this is needed for vorbis
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

# If we are building locally, we already have all the libraries.
if(MACOS AND NOT GOON_FULL_MACOS_BUILD)
    target_link_libraries(supergoonSound
        PUBLIC
        SDL2::SDL2-static
        ${LIB_VORBISFILE}
        ${LIB_VORBIS}
        ${LIB_OGG}
        ${OGG_LIBRARIES}
    )
else()
    # Use the names that are exported from building in subdirectories
    # target_link_libraries(supergoonSound
    #     PUBLIC
    #     vorbis vorbisenc vorbisfile ogg
    # )

    if(NOT EMSCRIPTEN)
        target_link_libraries(supergoonSound
            PUBLIC
            SDL2::SDL2-static
            vorbis vorbisenc vorbisfile ogg
        )
    endif(NOT EMSCRIPTEN)
endif(MACOS AND NOT GOON_FULL_MACOS_BUILD)

# This needs to be private when installing, and public when building in emscripten?
target_include_directories(supergoonSound
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/external/mojoAL

    # Need both ogg targets, ogg cmakelists is total trash bro.
    ${CMAKE_CURRENT_SOURCE_DIR}/external/vorbis/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/ogg/include
    ${CMAKE_BINARY_DIR}/external/ogg/include

    # Needed when building as library
    ${CMAKE_BINARY_DIR}/external/SDL/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/include
    /usr/local/include
)

# #########################################
# Install
# #########################################

if(INSTALL_SG_SOUND)
    set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
    set(INSTALL_INC_DIR include CACHE PATH "Installation directory for headers")

    # Check if the supergoonSound target already exists
    if(TARGET supergoonSound)
        # Get the existing include directories
        get_target_property(existing_include_dirs supergoonSound INTERFACE_INCLUDE_DIRECTORIES)

        # Remove problematic absolute paths
        foreach(dir IN LISTS existing_include_dirs)
            if(NOT IS_ABSOLUTE "${dir}")
                list(APPEND cleaned_include_dirs "${dir}")
            endif()
        endforeach()

        # Append the correct include directories
        list(APPEND cleaned_include_dirs
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
            $<INSTALL_INTERFACE:${INSTALL_INC_DIR}>
        )

        # Update the target properties
        set_target_properties(supergoonSound PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${cleaned_include_dirs}")

        # Install commands
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
    else()
        message(FATAL_ERROR "Target supergoonSound does not exist.")
    endif()
endif(INSTALL_SG_SOUND)
