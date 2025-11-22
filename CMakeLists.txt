cmake_minimum_required(VERSION 3.22)

project(picasso VERSION 0.5.2 LANGUAGES CXX DESCRIPTION "Picasso Shadercompiler on the Nintendo 3ds")

if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS")
message(STATUS "Not building for 3ds. Assembler included!")
set(PICA_BUILD_ASSEMBLER ON)
endif()

set(HEADER_FILES
        include/pica.hpp
        include/picasso/picasso.h
        include/picasso/types.h
        include/picasso/FileClass.h
        include/picasso/maestro_opcodes.h)

set(SOURCE_FILES
        source/picasso_assembler.cpp
        source/picasso_library.cpp)

add_library(pica
        ${HEADER_FILES}
        ${SOURCE_FILES})

add_library(pica::pica ALIAS pica)

set(PROJECT_PREFIX pica-${picasso_VERSION})

target_include_directories(pica PUBLIC include)

if(${PICA_BUILD_ASSEMBLER})
add_executable(picasso source/picasso_frontend.cpp)
target_link_libraries(picasso PRIVATE pica)
target_compile_definitions(picasso PRIVATE PACKAGE_STRING="picasso ${picasso_VERSION}")
endif()
