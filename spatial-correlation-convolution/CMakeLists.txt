cmake_minimum_required(VERSION 3.0.0)
project(spatial-correlation-convolution VERSION 0.1.0)

find_package(OpenCV REQUIRED)

include(CTest)
enable_testing()

include_directories("../utility")
add_executable(spatial-correlation-convolution main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(spatial-correlation-convolution ${OpenCV_LIBS})

install(TARGETS spatial-correlation-convolution
		RUNTIME DESTINATION bin)
 