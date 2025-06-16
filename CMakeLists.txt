cmake_minimum_required(VERSION 3.22)
project(movie_booking)

set(CMAKE_CXX_STANDARD 20)  
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# I need to include threads because asio depend on them
find_package(Boost 1.85 REQUIRED COMPONENTS json system thread)
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

# --- Include Directories ---
include_directories(include ${Boost_INCLUDE_DIRS})

# --- Source Files (excluding main.cpp for library) ---
file(GLOB_RECURSE SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# --- Create a library with your business logic ---
add_library(movie_booking_lib ${SOURCES})
target_link_libraries(movie_booking_lib
    PUBLIC
    Boost::json
    Boost::system
    Threads::Threads
)

# --- Main Executable Target ---
add_executable(movie_booking src/main.cpp)
target_link_libraries(movie_booking
    PRIVATE
    movie_booking_lib
)

# --- Enable Testing ---
enable_testing()

# --- Unit Tests Target (business logic only) ---
add_executable(unit_tests tests/unit_test.cpp)

target_include_directories(unit_tests 
    PRIVATE 
    include 
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(unit_tests
    PRIVATE
    movie_booking_lib
    GTest::gtest_main
    Boost::json
    Boost::system
    Threads::Threads
)

# --- Functional Tests Target (full application) ---
add_executable(functional_tests tests/functional_test_tcp.cpp)

target_include_directories(functional_tests 
    PRIVATE 
    include 
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(functional_tests
    PRIVATE
    movie_booking_lib
    GTest::gtest_main
    Boost::json
    Boost::system
    Threads::Threads
)

# Add test-specific compile definitions
target_compile_definitions(functional_tests PRIVATE 
    TESTING_MODE=1
    TEST_PORT_BASE=13000)

# --- Register Tests with CTest ---
include(GoogleTest)
gtest_discover_tests(unit_tests)
gtest_discover_tests(functional_tests)

# Add timeout for test discovery (TARGET property, not test property)
set_property(TARGET functional_tests PROPERTY 
    GTEST_DISCOVER_TESTS_TIMEOUT 30)

# --- Custom Test Targets for Convenience ---
add_custom_target(run_unit_tests
    COMMAND unit_tests
    DEPENDS unit_tests
    COMMENT "Running unit tests"
)

add_custom_target(run_functional_tests
    COMMAND functional_tests
    DEPENDS functional_tests
    COMMENT "Running functional tests"
)
