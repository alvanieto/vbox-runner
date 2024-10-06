cmake_minimum_required(VERSION 3.16)
project(VBoxRunners)

set(QT_MIN_VERSION "5.15.0")
set(KF_MIN_VERSION "5.91")
find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH})

# Find the required Libaries
include(QtVersionOption)
find_package(Qt${QT_MAJOR_VERSION} 5.12 REQUIRED CONFIG COMPONENTS Widgets Core Xml)
find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS KIO I18n Runner)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

include(ECMDeprecationSettings)
ecm_set_disabled_deprecation_versions(
	KF ${KF_MIN_VERSION}
	QT ${QT_MIN_VERSION}
)

remove_definitions(-DQT_NO_CAST_TO_ASCII -DQT_NO_CAST_FROM_ASCII)

add_subdirectory(vbox)
