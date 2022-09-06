cmake_minimum_required(VERSION 3.15)

project(lvvi LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

find_package(PkgConfig REQUIRED)

pkg_check_modules(glfw REQUIRED IMPORTED_TARGET glfw3)
pkg_check_modules(glew REQUIRED IMPORTED_TARGET glew)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework OpenGL")

file(GLOB test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")
foreach(mainfile IN LISTS test_src)
    get_filename_component(testname ${mainfile} NAME_WE)
    add_executable(${testname} ${mainfile})
    target_link_libraries(${testname}
        PkgConfig::glfw
        PkgConfig::glew
    )
endforeach()
