cmake_minimum_required(VERSION 3.10)
project(JlangTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

add_executable(run-tests
    Scanner/ScannerTests.cpp
    ../src/Scanner/Scanner.cpp
)

target_include_directories(run-tests PRIVATE
    ${GTEST_INCLUDE_DIRS}
    ../src
)

target_link_libraries(run-tests
    ${GTEST_LIBRARIES}
    ${GTEST_MAIN_LIBRARIES}
    pthread
)
