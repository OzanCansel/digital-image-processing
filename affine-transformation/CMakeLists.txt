cmake_minimum_required(VERSION 3.0.0)
project(affine-transformation VERSION 0.1.0)

find_package(OpenCV REQUIRED)

include_directories(../utility)

include(CTest)
enable_testing()

add_executable(affine-transformation main.cpp)
target_link_libraries(affine-transformation ${OpenCV_LIBS} utility)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
 
install(TARGETS affine-transformation
		RUNTIME DESTINATION bin)

install(FILES "${PROJECT_SOURCE_DIR}/../resources/affine-transformation.jpg" DESTINATION bin)

add_custom_command(TARGET affine-transformation POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/../resources/affine-transformation.jpg" "${CMAKE_BINARY_DIR}/affine-transformation"
)
