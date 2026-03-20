cmake_minimum_required(VERSION 3.10)
project(SystemMonitor)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)

#Find NVML
find_library(NVML_LIBRARY nvidia-ml)
find_path(NVML_INCLUDE_DIR nvml.h PATHS /usr/include /opt/cuda/include)

# Automatically find all .cpp files
file(GLOB SRC_FILES src/*.cpp)

# Create the executable
add_executable(system_monitor ${SRC_FILES})

# Include directories
target_include_directories(system_monitor PRIVATE ${GTKMM_INCLUDE_DIRS} src/include)

# Link GTKMM and NVML
target_link_libraries(system_monitor PRIVATE ${GTKMM_LIBRARIES})

# if nvidia include it
if(NVML_LIBRARY AND NVML_INCLUDE_DIR)
  target_include_directories(system_monitor PRIVATE ${NVML_INCLUDE_DIR})
  target_link_libraries(system_monitor PRIVATE ${NVML_LIBRARY})
  target_compile_definitions(system_monitor PRIVATE HAVE_NVML=1)
  message(STATUS "NVML found, building with nvidia support!")
else()
  message(STATUS "NVML not found, building without NVIDIA support!")
endif()
