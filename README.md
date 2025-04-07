# system_monitor

## Table of Contents

- [About](#About)
- [Image](#Image)
- [Installation](#Installation)
  - [Dependencies](##Dependencies)
- [Documentation](#Documentation)
  - [Overview](##Overview)
  - [How it works](##How-it-works)
  - [Code structure](##Code-structure)
  - [Running the program](##Running-the-program)
  - [Testing](##Testing) 
  - [Known Limitations](##Known-Limitations)

# About

A simple system monitor for Linux, using gtk4/gtkmm-4.0 and written in C++.
This project was created as part of a school assignment to learn system-level programming in C++ and explore GUI development in gtk.

This program displays real-time usages statistics for the CPU, GPU, memory, hard drives and internet (upload/download).
The program gets system information from the Linux `/proc` pseudo-filesystem and uses the Nvidia API to retrieve the GPU stats.

# Image

![system_monitor](https://github.com/Helland369/system_monitor/blob/main/img/2025-04-07_11-01.png)

# Installation

## Dependencies

 - [gtk4](https://www.gtk.org/docs/installations/linux/)
 - [gtkmm-4.0](https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/chapter-installation.html)
 - [cmake](https://cmake.org/)
 - [ninja](https://ninja-build.org/)
 - [gcc/g++](https://gcc.gnu.org/)

### You can also use your favorite package manager

On Arch you would use pacman:

```
sudo pacman -S gcc gtk4 gtkmm4.0 cmake ninja
```

## Using git

Clone the repository:

```
git clone https://github.com/Helland369/system_monitor.git
```

cd to project

```
cd system_monitor
```

cmake command

```
cmake --fresh -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

cd to build

```
cd build
```

run the program

```
./system_monitor
```

## Binary

You can get a binary [here](https://github.com/Helland369/system_monitor/releases), but you need the [gtk4](https://www.gtk.org/docs/installations/linux/) and [gtkmm-4.0](https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/chapter-installation.html) dependencies.


# Documentation

## Overview

This system monitor gathers system statistics by reading files from the `/proc` pseudo-filesystem and accessing the NVIDIA API for GPU stats. It uses GTK4 and gtkmm-4.0 to build a modern, native Linux GUI.

+ While running the program, you can press `1` through `5` to change the layout of the boxes.

## How it works

- **CPU & Memory Usage:**

  Retrieved from `/proc/stat`, `/proc/meminfo`, and other related files.

- **Disk Usage:**
  
  Read from `/proc/diskstats`.

- **Network Usage:**

  Calculated Using `/proc/net/dev`, tracking changes in RX/TX byte counts over time.
  
- **GPU Usage:**

  Fetched via the NVIDIA Management Library (NVML).
  
- **GUI:**

  Built with GTK4 widgets using the gtkmm C++ bindings, updated in real time using timers.
  
## Code structure

- `/src/main.cpp` Main entry point of the program.

- `/src/SystemMonitorWindow.cpp` GTK related UI and rendering.

- `/src/CpuUsage.cpp` Logic for gathering CPU usage info.

- `src/FileSystem.cpp` Logic for gathering Disk usage info.

- `src/MemInfo.cpp` Logic for gathering memory usage info.

- `src/NetInfo.cpp` Logic for gathering Network info.

- `src/NvidiaInfo.cpp` Logic for talking to the Nvidia api (NVML).
  
## Running the program

After building the project, you can run the binary from the `build/` folder:

```bash
./system_monitor
```

## Testing

Tested on Arch Linux in I3 and X11

## Known Limitations

- GPU monitoring only works with Nvidia GPUs via NVML.

- The program only supports Linux.

- GTK themes don't work.
