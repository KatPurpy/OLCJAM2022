cmake_minimum_required(VERSION 3.17)
project(OLCJAM2022)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(BZZRE_BASE_DIR C:\\_PurpsStash\\PROGRAMMING\\BZZRE)
set(BZZRE_OUT_ASSETS ${CMAKE_BINARY_DIR}/assets)
add_subdirectory(C:\\_PurpsStash\\PROGRAMMING\\BZZRE BZZRE)

FILE(GLOB_RECURSE ASSETS "assets/**")

foreach(a ${ASSETS})
    file(RELATIVE_PATH relpath "${CMAKE_CURRENT_SOURCE_DIR}/assets" ${a})
    configure_file("assets/${relpath}" "assets/${relpath}" COPYONLY)
endforeach()


FILE(GLOB_RECURSE src src/**.cpp src/**.c)

if(EMSCRIPTEN)
set(CMAKE_EXECUTABLE_SUFFIX .html)
endif()
add_executable(main ${src})

if(EMSCRIPTEN)
target_link_options(main PRIVATE -s TOTAL_MEMORY=128MB --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/shell.html
        -sEXPORTED_FUNCTIONS=['_main'] --preload-file "${CMAKE_CURRENT_BINARY_DIR}/assets@./assets")
endif()

target_include_directories(main PRIVATE 3rdparty)
BZZRIZE(main)
add_subdirectory(submodules)

configure_file(README.txt README.txt)
configure_file(LICENSE.txt LICENSE.txt)

