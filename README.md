# vbox-runner
Allows you to start your VirtualBox virtual machines from Krunner (Alt-Space) in Plasma 5.

This is a fork from http://kde-apps.org/content/show.php/VBox+Runner?content=107926, because is
discontinued.

# Build instructions

## Prerequisites (archlinux)

    * Qt5
    * KF5
    * cmake. sudo pacman -S cmake
    * ECM modules. sudo pacman -S extra-cmake-modules
    * gcc compiler

## Build

    1. Configure. cmake -S . -B build
    2. Build. cmake --build build
    3. Install. sudo cmake --install build
