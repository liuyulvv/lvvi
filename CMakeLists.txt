cmake_minimum_required(VERSION 3.15)

project(lvvi LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(
    "./include/"
    "./include/GL/"
    "./include/GLFW/"
)

link_directories(
    "./lib"
)

file(GLOB test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")
foreach(mainfile IN LISTS test_src)
    get_filename_component(testname ${mainfile} NAME_WE)
    add_executable(${testname} ${mainfile})
    target_link_libraries(${testname}
        glew32
        glfw3
        opengl32
    )
endforeach()
