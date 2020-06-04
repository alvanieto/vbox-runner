# vbox-runner
Allows you to start your VirtualBox virtual machines from Krunner (Alt-Space) in Plasma 5.

This is a fork from http://kde-apps.org/content/show.php/VBox+Runner?content=107926, because it is
discontinued.

# Build instructions

## Prerequisites 
    * Qt5
    * KF5

### Archlinux
    sudo pacman -S cmake extra-cmake-modules

### Debian/Ubuntu
    sudo apt install cmake extra-cmake-modules build-essential libkf5runner-dev libkf5kio-dev libkf5textwidgets-dev qtdeclarative5-dev gettext
    
### openSUSE
    sudo zypper install cmake extra-cmake-modules libQt5Widgets5 libQt5Core5 libqt5-qtlocation-devel ki18n-devel ktextwidgets-devel kservice-devel krunner-devel gettext-tools

### Fedora
    sudo dnf install cmake extra-cmake-modules kf5-ki18n-devel kf5-kservice-devel kf5-krunner-devel kf5-ktextwidgets-devel gettext

## Build
    1. Configure -> cmake .
    2. Build -> make
    3. Install -> sudo make install
    4. Restart Krunner. kquitapp5 krunner; kstart5 --windowclass krunner krunner

## Screenshots

#### Overview of virtual machines, they get sorted by the number of launches
![Overview of virtual machines](https://raw.githubusercontent.com/alex1701c/Screenshots/master/vbox-runner/vm_overview.png)

#### Search for virtual machine
![Search for virtual machine](https://raw.githubusercontent.com/alex1701c/Screenshots/master/vbox-runner/vm_search.png)
