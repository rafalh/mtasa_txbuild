cmake_minimum_required(VERSION 3.22)

project(txbuild)

if (MSVC)
    add_compile_options(/W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_DEPRECATE)
else()
    add_compile_options(-Wall)
endif()

if (MINGW)
    add_link_options(-static $<$<CONFIG:Release>:-s>)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../bin")

add_subdirectory(tinyxml)
add_subdirectory(addutf8bom)
add_subdirectory(txbuild)
add_subdirectory(txprotect)
add_subdirectory(txstrlist)
