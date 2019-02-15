cmake_minimum_required(VERSION 3.0.0)
project(histogram-equalization VERSION 0.1.0)

find_package(OpenCV REQUIRED)

include(CTest)
enable_testing()

include_directories("../utility")

add_executable(histogram-equalization main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(histogram-equalization ${OpenCV_LIBS} utility)

install(TARGETS histogram-equalization
		RUNTIME DESTINATION bin)

install(FILES "${PROJECT_SOURCE_DIR}/../resources/histogram-equalization.png" DESTINATION bin)

add_custom_command(TARGET histogram-equalization POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/../resources/histogram-equalization.png" "${CMAKE_BINARY_DIR}/histogram-equalization"
) 
 