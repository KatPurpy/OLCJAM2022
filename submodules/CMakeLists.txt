add_subdirectory(box2d-2.4.1)
target_link_libraries(main box2d)
target_include_directories(main PRIVATE box2d-2.4.1/include)