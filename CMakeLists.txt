cmake_minimum_required(VERSION 3.10)
project(SystemMonitor)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)

#Find NVML
find_library(NVML_LIBRARY nvidia-ml)
find_path(NVML_INCLUDE_DIR nvml.h PATHS /usr/include /opt/cuda/include)

# Find rocm_smi
#find_library(ROCM_SMI_LIBRARY lamd_smi)
find_library(ROCM_SMI_LIBRARY rocm_smi64 PATH /opt/rocm/lib /opt/rocm/lib64)
find_path(ROCM_SMI_INCLUDE_DIR rocm_smi/rocm_smi.h PATH /opt/rocm/include)

# Automatically find all .cpp files
file(GLOB SRC_FILES src/*.cpp)

# Create the executable
add_executable(system_monitor ${SRC_FILES})

# Include directories
target_include_directories(system_monitor PRIVATE ${GTKMM_INCLUDE_DIRS} src/include)

# Link GTKMM and NVML
target_link_libraries(system_monitor PRIVATE ${GTKMM_LIBRARIES})

#if amd include it
if (ROCM_SMI_LIBRARY AND ROCM_SMI_INCLUDE_DIR)
  target_include_directories(system_monitor PRIVATE ${ROCM_SMI_INCLUDE_DIR})
  target_link_libraries(system_monitor PRIVATE ${ROCM_SMI_LIBRARY})
  target_compile_definitions(system_monitor PRIVATE HAVE_ROCM_SMI=1)
  message(STATUS "ROCM_SMI foud, building with AMD GPU support!")
else()
  message(STATUS "ROCM_SMI not found, building without AMD GPU support!")
endif()

# if nvidia include it
if(NVML_LIBRARY AND NVML_INCLUDE_DIR)
  target_include_directories(system_monitor PRIVATE ${NVML_INCLUDE_DIR})
  target_link_libraries(system_monitor PRIVATE ${NVML_LIBRARY})
  target_compile_definitions(system_monitor PRIVATE HAVE_NVML=1)
  message(STATUS "NVML found, building with nvidia support!")
else()
  message(STATUS "NVML not found, building without NVIDIA support!")
endif()
