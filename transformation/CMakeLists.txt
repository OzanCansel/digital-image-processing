cmake_minimum_required(VERSION 3.0.0)
project(transformation VERSION 0.1.0)

find_package(OpenCV REQUIRED)

include(CTest)
enable_testing()

add_executable(transformation main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(transformation ${OpenCV_LIBS})

add_custom_command(TARGET transformation POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/../resources/transformation_darker.jpg" ${CMAKE_BINARY_DIR}
)
