cmake_minimum_required(VERSION 3.5)
project(movie_booking)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# I need to include threads because asio depend on them
#set(BOOST_ROOT "/opt/boost_1_85_0")
set(BOOST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/boost/boost_1_85_0")
set(BOOST_LIBRARYDIR "${CMAKE_CURRENT_SOURCE_DIR}/boost/boost_1_85_0/stage/lib")
find_package(Boost 1.85 REQUIRED COMPONENTS json system thread)
find_package(Threads REQUIRED)

# --- Include Directories ---
include_directories(include ${Boost_INCLUDE_DIRS})

# --- Source Files ---
file(GLOB_RECURSE SOURCES "src/*.cpp")

# --- Executable Target ---
add_executable(movie_booking ${SOURCES})

# --- Linking Libraries ---
target_link_libraries(movie_booking
    PRIVATE
    #${Boost_LIBRARIES}
    Boost::json
    Boost::system
    Threads::Threads
)
