cmake_minimum_required(VERSION 3.22)

project(picasso VERSION 0.6.0 LANGUAGES CXX DESCRIPTION "Picasso Shader Compiler on the Nintendo 3ds")

if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS")
message(STATUS "Not building for 3ds. Assembler included!")
set(PICA_BUILD_ASSEMBLER ON)
endif()

set(SOURCE_FILES
        source/picasso_assembler.cpp
        source/picasso_library.cpp
)

add_library(pica ${SOURCE_FILES})

add_library(pica::pica ALIAS pica)

set(PROJECT_PREFIX pica-${picasso_VERSION})

target_include_directories(pica
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_include_directories(pica
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source/private
)

if(${PICA_BUILD_ASSEMBLER})
add_executable(picasso source/picasso_frontend.cpp)
target_link_libraries(picasso PRIVATE pica)
target_compile_definitions(picasso PRIVATE PACKAGE_STRING="picasso ${picasso_VERSION}" PICA_FRONTEND)
endif()
