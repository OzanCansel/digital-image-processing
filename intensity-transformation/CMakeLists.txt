cmake_minimum_required(VERSION 3.0.0)
project(intensity-transformation VERSION 0.1.0)

find_package(OpenCV REQUIRED)

include(CTest)
enable_testing()

add_executable(intensity-transformation main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(intensity-transformation ${OpenCV_LIBS})

install(TARGETS intensity-transformation
		RUNTIME DESTINATION bin)

install(FILES "${PROJECT_SOURCE_DIR}/../resources/intensity-transformation-dark.jpg" DESTINATION bin)

add_custom_command(TARGET intensity-transformation POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/../resources/intensity-transformation-dark.jpg" "${CMAKE_BINARY_DIR}/intensity-transformation"
)
 