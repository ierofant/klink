project(klink CXX)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS -std=c++11)

add_subdirectory(geser)
include_directories(${PROJECT_SOURCE_DIR} geser ${LIBXML++_INCLUDE_DIRS} ${RSVG_INCLUDE_DIRS} ${GTKMM3_INCLUDE_DIRS})

file(GLOB SOURCE src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} geser)
target_link_libraries(${PROJECT_NAME}	${LIBXML++_LIBRARIES}
					${RSVG_LIBRARIES}
					${GTKMM3_LIBRARIES})

