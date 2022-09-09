cmake_minimum_required(VERSION 3.15)

project(lvvi LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

file(GLOB test_src RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework OpenGL")

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(glfw REQUIRED IMPORTED_TARGET glfw3)
    pkg_check_modules(glew REQUIRED IMPORTED_TARGET glew)

    foreach(mainfile IN LISTS test_src)
        get_filename_component(testname ${mainfile} NAME_WE)
        add_executable(${testname} ${mainfile})
        target_link_libraries(${testname}
            PkgConfig::glfw
            PkgConfig::glew
        )
    endforeach()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    include_directories(
        "./include"
        "./include/GL"
        "./include/GLFW"
        "./include/spdlog"
        "./include/mediapipe"
        "D:/opencv/build/include"
        "D:/opencv/build/include/opencv2"
    )
    link_directories(
        "./lib"
        "D:/onnxruntime/onnxruntime-win-x64-gpu-1.12.0/lib"
        "D:/opencv/build/x64/vc15/lib"
        "D:/opencv/build/x64/vc15/bin"
    )
    foreach(mainfile IN LISTS test_src)
        get_filename_component(testname ${mainfile} NAME_WE)
        add_executable(${testname} ${mainfile})
        target_link_libraries(${testname}
            glew32
            glfw3
            opengl32
            opencv_world3410
            onnxruntime
            onnxruntime_providers_cuda
            onnxruntime_providers_shared
            onnxruntime_providers_tensorrt
            mediapipe
        )
    endforeach()
endif()
