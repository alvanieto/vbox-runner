cmake_minimum_required(VERSION 3.5)
project(VBoxRunners)

set(KF5_MIN_VERSION "5.72")
find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH
     ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_MODULE_PATH}
)

# Find the required Libaries
find_package (Qt5 5.12 REQUIRED CONFIG COMPONENTS Widgets Core Xml)
find_package (KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS KIO I18n Service Runner Plasma)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

add_subdirectory(vbox)
