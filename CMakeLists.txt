cmake_minimum_required(VERSION 3.30)
project(ds LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")
file(GLOB EXECUTABLE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cc")

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION True)

foreach(EXEC_FILE ${EXECUTABLE_SOURCES})
  get_filename_component(EXEC_NAME ${EXEC_FILE} NAME_WE)
  add_executable(${EXEC_NAME} ${EXEC_FILE})
  target_link_libraries(${EXEC_NAME} PRIVATE ${PROJECT_NAME})
  set_property(TARGET ${EXEC_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
endforeach()
