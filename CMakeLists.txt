cmake_minimum_required(VERSION 3.15)

project(lvvi LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

file(GLOB test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(opencv REQUIRED IMPORTED_TARGET opencv4)

    link_directories(
        "./lib/mac/"
    )

    include_directories(
        "./include"
        "./include/mediapipe"
    )

    foreach(mainfile IN LISTS test_src)
        get_filename_component(testname ${mainfile} NAME_WE)
        add_executable(${testname} ${mainfile})
        target_link_libraries(${testname}
            PkgConfig::opencv
            mediapipe
        )
    endforeach()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    include_directories(
        "./include"
        "./include/mediapipe"
        "D:/opencv/build/include"
        "D:/opencv/build/include/opencv2"
    )
    link_directories(
        "./lib/win"
        "D:/opencv/build/x64/vc15/lib"
        "D:/opencv/build/x64/vc15/bin"
    )
    foreach(mainfile IN LISTS test_src)
        get_filename_component(testname ${mainfile} NAME_WE)
        add_executable(${testname} ${mainfile})
        target_link_libraries(${testname}
            opencv_world3410
            mediapipe
        )
    endforeach()
endif()
