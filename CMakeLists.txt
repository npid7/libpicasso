cmake_minimum_required(VERSION 3.22)

project(picasso)

set(CMAKE_EXE_LINKER_FLAGS "-L${DEVKITPRO}/libctru/lib -L${DEVKITPRO}/picaGL/lib -L${DEVKITPRO}/portlibs/3ds/lib -specs=3dsx.specs -Wl,--gc-sections")

include_directories(${DEVKITPRO}/libctru/include ${DEVKITPRO}/picaGL/include ${DEVKITPRO}/portlibs/3ds/include)
add_definitions("-D__3DS__")

enable_language(ASM)

add_library(${PROJECT_NAME} STATIC source/picasso_assembler.cpp source/picasso_library.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE include)