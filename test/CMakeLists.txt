cmake_minimum_required(VERSION 3.16.3 FATAL_ERROR)

project(tests)
add_compile_options(-c -Wall -Werror -Wextra -O3 -std=c++2a)

include_directories(../../json/include)

if (WIN32)
    message(STATUS "Building under Windows OS")
    # Set up correct path to Boost Library folder
    set(BOOST_ROOT "C:/boost_1_78_0/")
    set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
    set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")
    set(Boost_USE_STATIC_LIBS ON)
else(UNIX)
    message(STATUS "Building under NIX* OS")
endif()

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

if (NOT Boost_FOUND)
    message(FATAL "Boost not found")
else()
    message(STATUS "Boost version: Boost_VERSION=${Boost_VERSION}")
endif()

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
include_directories(${SRC_DIR})

add_executable(unit_tests
        unit_tests.cpp
        utils/TestUtils.cpp utils/TestUtils.h
        ${SRC_DIR}/Types.h
        ${SRC_DIR}/Utilities.h ${SRC_DIR}/Utilities.cpp
        ${SRC_DIR}/PricePointBook.h ${SRC_DIR}/PricePointBook.cpp
)

add_executable(benchmarks
        benchmarks.cpp
        utils/TestUtils.cpp utils/TestUtils.h
        ${SRC_DIR}/Types.h
        ${SRC_DIR}/Utilities.h ${SRC_DIR}/Utilities.cpp
        ${SRC_DIR}/PricePointBook.h ${SRC_DIR}/PricePointBook.cpp
        )

add_executable(memory_usage
        memory_usage.cpp
        utils/TestUtils.cpp utils/TestUtils.h
        ${SRC_DIR}/Types.h
        ${SRC_DIR}/Utilities.h ${SRC_DIR}/Utilities.cpp
        ${SRC_DIR}/PricePointBook.h ${SRC_DIR}/PricePointBook.cpp
        )

TARGET_LINK_LIBRARIES(unit_tests
        boost_unit_test_framework
)

TARGET_LINK_LIBRARIES(benchmarks
        boost_unit_test_framework
)

TARGET_LINK_LIBRARIES(memory_usage
        boost_unit_test_framework pthread
)