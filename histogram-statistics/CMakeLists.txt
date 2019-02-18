cmake_minimum_required(VERSION 3.0.0)
project(histogram-statistics VERSION 0.1.0)

find_package(OpenCV REQUIRED)

include(CTest)
enable_testing()

include_directories("../utility")
add_executable(histogram-statistics main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(histogram-statistics utility ${OpenCV_LIBS})

install(TARGETS histogram-statistics
		RUNTIME DESTINATION bin)

install(FILES "${PROJECT_SOURCE_DIR}/../resources/histogram-statistics.jpg" DESTINATION bin)

add_custom_command(TARGET histogram-statistics POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/../resources/histogram-statistics.jpg" "${CMAKE_BINARY_DIR}/histogram-statistics"
) 
 