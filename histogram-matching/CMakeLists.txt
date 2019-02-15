cmake_minimum_required(VERSION 3.0.0)
project(histogram-matching VERSION 0.1.0)

find_package(OpenCV REQUIRED)

include(CTest)
enable_testing()

include_directories("../utility")
add_executable(histogram-matching main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(histogram-matching utility ${OpenCV_LIBS})

install(TARGETS histogram-matching
		RUNTIME DESTINATION bin)

install(FILES "${PROJECT_SOURCE_DIR}/../resources/histogram-matching-input.jpg" "${PROJECT_SOURCE_DIR}/../resources/histogram-matching-template.jpg" DESTINATION bin)

add_custom_command(TARGET histogram-matching POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/../resources/histogram-matching-input.jpg" "${CMAKE_BINARY_DIR}/histogram-matching"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/../resources/histogram-matching-template.jpg" "${CMAKE_BINARY_DIR}/histogram-matching"
) 
 