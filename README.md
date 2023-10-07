# Supergoon Sound
A C wrapper around openal1.1 that works with SDL2 for all platforms, that allows for looping music at points (IntroLoop), so that you can play a sound file and it loops and doesn't play the full song again.

## Development
- Mostly developed on macos apple silicon
- Going to build on Mac, Linux, Windows, Emscripten currently

## Goals
- Better at C
- Better at cross-platform building (Macos, Linux, Windows)

## Current State
- Tested on Mac and Emscripten builds

## Requirements
- Cmake will install all of the required libraries with all of the rebuild commands in make except "rebuild".

## Components
- CMake: The actual cross-platform build system.
- Make: Streamlining building with one command
- Emscripten: Build for web
- MojoAL: OpenAL 1.1 wrapper for SDL

## Licenses
- MIT

Libraries:
- [MojoAL](https://github.com/icculus/mojoAL/blob/main/LICENSE.txt) - ZLIB - OpenAL Implementation