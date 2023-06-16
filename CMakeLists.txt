# CMakeLists.txt
cmake_minimum_required(VERSION 3.0)
project(test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# find_package(unofficial-sqlite3 CONFIG REQUIRED)
# target_link_libraries(main PRIVATE unofficial::sqlite3::sqlite3)

# this is heuristically generated, and may not be correct
find_package(TBB CONFIG REQUIRED)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE TBB::tbb TBB::tbbmalloc)

