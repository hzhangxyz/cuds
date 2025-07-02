cmake_minimum_required(VERSION 3.30)
project(ds LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/*.cc")
file(GLOB EXECUTABLE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

foreach(EXEC_FILE ${EXECUTABLE_SOURCES})
  get_filename_component(EXEC_NAME ${EXEC_FILE} NAME_WE)
  add_executable(${EXEC_NAME} ${EXEC_FILE})
  target_link_libraries(${EXEC_NAME} PRIVATE ${PROJECT_NAME})
endforeach()
