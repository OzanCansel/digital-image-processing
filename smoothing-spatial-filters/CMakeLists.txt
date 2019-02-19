cmake_minimum_required(VERSION 3.0.0)
project(smoothing-spatial-filters VERSION 0.1.0)

find_package(OpenCV REQUIRED)

include(CTest)
enable_testing()

include_directories("../utility")

add_executable(smoothing-spatial-filters main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(smoothing-spatial-filters ${OpenCV_LIBS} utility)

install(TARGETS smoothing-spatial-filters
		RUNTIME DESTINATION bin)

install(FILES "${PROJECT_SOURCE_DIR}/../resources/smoothing-spatial-filter.jpg" DESTINATION bin)

add_custom_command(TARGET smoothing-spatial-filters POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/../resources/smoothing-spatial-filter.jpg" "${CMAKE_BINARY_DIR}/smoothing-spatial-filters"
) 
 