cmake_minimum_required(VERSION 3.22)

project(linpicasso_sample)

set(CMAKE_EXE_LINKER_FLAGS "-L${DEVKITPRO}/libctru/lib -L${DEVKITPRO}/picaGL/lib -L${DEVKITPRO}/portlibs/3ds/lib -specs=3dsx.specs -Wl,--gc-sections")

include_directories(${DEVKITPRO}/libctru/include ${DEVKITPRO}/picaGL/include ${DEVKITPRO}/portlibs/3ds/include)
add_definitions("-D__3DS__")


set(APP_TITLE "${PROJECT_NAME}")
set(APP_DESCRIPTION "Example of Lib Picasso")
set(APP_AUTHOR "Tobi-D7, tobid7vx")

set(APP_ICON "/opt/devkitpro/libctru/default_icon.png")
set(APP_ROMFS "${PROJECT_SOURCE_DIR}/romfs")

enable_language(ASM)

set(BASE_CTR ON CACHE BOOL "Enable 3ds")
add_subdirectory(../ picasso)

add_executable(${PROJECT_NAME}.elf src/main.cpp)

target_include_directories(${PROJECT_NAME}.elf PRIVATE src ../include)

target_link_libraries(${PROJECT_NAME}.elf citro2d citro3d ctru m picasso)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.smdh
	COMMAND smdhtool --create "${APP_TITLE}" "${APP_DESCRIPTION}" "${APP_AUTHOR}"  "${APP_ICON}" ${PROJECT_NAME}.smdh
    DEPENDS ${PROJECT_NAME}.elf
)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.3dsx
	COMMAND 3dsxtool ${PROJECT_NAME}.elf ${PROJECT_NAME}.3dsx --romfs=${APP_ROMFS} --smdh=${PROJECT_NAME}.smdh
	DEPENDS ${PROJECT_NAME}.elf
)

add_custom_target( 3ds ALL
	DEPENDS ${PROJECT_NAME}.smdh ${PROJECT_NAME}.3dsx
)