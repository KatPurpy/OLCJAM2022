cmake_minimum_required(VERSION 3.17)
project(OLCJAM2022)



set(BZZRE_BASE_DIR C:\\_PurpsStash\\PROGRAMMING\\BZZRE)
set(BZZRE_OUT_ASSETS ${CMAKE_BINARY_DIR}/assets)
add_subdirectory(C:\\_PurpsStash\\PROGRAMMING\\BZZRE BZZRE)

FILE(GLOB_RECURSE ASSETS "assets/**")

foreach(a ${ASSETS})
    file(RELATIVE_PATH relpath "${CMAKE_CURRENT_SOURCE_DIR}/assets" ${a})
    configure_file("assets/${relpath}" "assets/${relpath}" COPYONLY)
endforeach()


FILE(GLOB_RECURSE src src/**.cpp src/**.c)
add_executable(main ${src})
BZZRIZE(main)
add_subdirectory(submodules)
