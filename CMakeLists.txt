cmake_minimum_required(VERSION 3.10)
project(apis_mt76xx)

set(PROJECT_INCLUDE_DIR 
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/libs/cJSON)

file(GLOB PROJECT_SOURCE_FILES 
    ${CMAKE_SOURCE_DIR}/src/*.c)

file(GLOB CJSON_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/libs/cJSON/cJSON.c)

add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/src/main.c
    ${PROJECT_SOURCE_FILES}
    ${CJSON_SOURCE_FILES})

add_compile_options(-Wall -Wextra -pedantic -Werror)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_INCLUDE_DIR})
