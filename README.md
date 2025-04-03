# system_monitor

A simple system monitor project in C++ and GTK / GTKmm

# Installation

## Dependencies

[gtk4](https://www.gtk.org/docs/installations/linux/)
[gtkmm-4.0](https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/chapter-installation.html)
[cmake](https://cmake.org/)
[ninja](https://ninja-build.org/)
[gcc/g++](https://gcc.gnu.org/)

### You can allso use your faovorite package manager

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
cd /system_monitor
```

cmake command

```
cmake --fresh -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

cd to build

```
cd /build
```

run the programm

```
./system_monitor
```

## Binary

You can get a binary [here](https://github.com/Helland369/system_monitor/releases), but you need the [gtk4](https://www.gtk.org/docs/installations/linux/) and [gtkmm-4.0](https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/chapter-installation.html) dependencies.
