cmake_minimum_required(VERSION 3.5)
project(movie_booking_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Boost paths
set(BOOST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../boost/boost_1_85_0")
set(BOOST_LIBRARYDIR "${CMAKE_CURRENT_SOURCE_DIR}/../boost/boost_1_85_0/stage/lib")
find_package(Boost 1.85 REQUIRED COMPONENTS json system thread)
find_package(Threads REQUIRED)

# --- Include Directories ---
include_directories(${Boost_INCLUDE_DIRS})

# --- Source Files ---
file(GLOB_RECURSE CLIENT_SOURCES "Client.cpp")

# --- Executable Target ---
add_executable(movie_booking_client ${CLIENT_SOURCES})

# --- Linking Libraries ---
target_link_libraries(movie_booking_client
    PRIVATE
    Boost::json
    Boost::system
    Threads::Threads
)
