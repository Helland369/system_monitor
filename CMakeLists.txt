cmake_minimum_required(VERSION 3.10)
project(SystemMonitor)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)

# Automatically find all .cpp files
file(GLOB SRC_FILES src/*.cpp)

# Create the executable
add_executable(system_monitor ${SRC_FILES})

# Include directories
target_include_directories(system_monitor PRIVATE ${GTKMM_INCLUDE_DIRS} src/include)

# Link GTKMM
target_link_libraries(system_monitor PRIVATE ${GTKMM_LIBRARIES})
