
cmake_minimum_required(VERSION 2.8)
project( CompressImage )

# The version number.
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lboost_system -lboost_filesystem -std=c++11 -Wall")
find_package( OpenCV REQUIRED )
add_executable( CompressImage try_boost.cpp)
target_link_libraries( CompressImage ${OpenCV_LIBS} boost_system boost_filesystem)
