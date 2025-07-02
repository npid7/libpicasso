cmake_minimum_required(VERSION 3.22)

project(picasso VERSION 0.5.2 LANGUAGES CXX DESCRIPTION "Picasso Shadercompiler on the Nintendo 3ds")

include(CMakePackageConfigHelpers)

if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release CACHE STRING
                "Choose the type of build, options are: None Debug Release."
                FORCE)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE STRING
                "The install location"
                FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

include_directories(include)

set(HEADER_FILES
        include/pica.hpp
        include/picasso/picasso.h
        include/picasso/types.h
        include/picasso/FileClass.h
        include/picasso/maestro_opcodes.h)

set(SOURCE_FILES
        source/picasso_assembler.cpp
        source/picasso_library.cpp)

add_library(${PROJECT_NAME}
        ${HEADER_FILES}
        ${SOURCE_FILES})

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

set(PROJECT_PREFIX ${PROJECT_NAME}-${picasso_VERSION})

target_include_directories(${PROJECT_NAME} PUBLIC include)
