cmake_minimum_required (VERSION 2.8)

cmake_policy(SET CMP0048 NEW) 
project(emeocv VERSION 0.0.1 LANGUAGES CXX)
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

add_compile_options("-Wno-write-strings")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(dependencies)


file(GLOB EMEOCV_SRCS *.cpp)
add_executable(emeocv ${EMEOCV_SRCS})
target_link_libraries(emeocv ${EMEOCV_LINKER_LIBS})


