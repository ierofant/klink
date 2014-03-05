project(klink CXX)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS -std=c++11)

add_subdirectory(geser)
include_directories(${PROJECT_BINARY_DIR}/src geser ${LIBXML++_INCLUDE_DIRS} ${RSVG_INCLUDE_DIRS} ${GTKMM3_INCLUDE_DIRS})

if(KLINK_STANDALONE)
    message(STATUS "klink mode: standalone")
    set(KLINK_RESOURCE_DIR ${CMAKE_SOURCE_DIR}/resource)
else()
    message(STATUS "klink mode: system")
    set(KLINK_RESOURCE_DIR ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/resource)
endif()

file(GLOB SOURCE src/*.cpp)
configure_file(config/klink_config.hpp.in src/klink_config.hpp)

add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} geser)
target_link_libraries(${PROJECT_NAME}	${LIBXML++_LIBRARIES}
					${RSVG_LIBRARIES}
					${GTKMM3_LIBRARIES})

if(NOT KLINK_STANDOUT)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(DIRECTORY resource DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME})
endif()
