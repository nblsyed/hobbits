#
# CMake Setup
#

cmake_minimum_required(VERSION 3.15)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/findFFTW")


#
# Project Config
#

set(LINUX FALSE)
if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if (EXISTS "${CMAKE_SOURCE_DIR}/.version")
    message("Getting build version from .version file")
    file (STRINGS "${CMAKE_SOURCE_DIR}/.version" BUILD_VERSION)
    string(REGEX REPLACE "\n$" "" BUILD_VERSION "${BUILD_VERSION}")
    string(STRIP "${BUILD_VERSION}" BUILD_VERSION)
    set(VERSION_STRING "${BUILD_VERSION}")
else()
    message("Attempting to get build version from Git")
    include(gitversion)
    gitversion(BUILD_VERSION)
    set(VERSION_STRING "0.0.0")
endif()

message("Building hobbits version: ${BUILD_VERSION}")

project("hobbits"
        VERSION "${VERSION_STRING}"
        HOMEPAGE_URL "https://github.com/Mahlet-Inc/hobbits")

include(GNUInstallDirs)
include(CheckCCompilerFlag)
include(CPackComponent)
include(Hardening)
include(passthroughvars)
include(FindPCAP)

#
# Compile and Link Config
#

set(CXX_STANDARD 20)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (LINUX)
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5)
        SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fno-sized-deallocation")
    endif()
    add_link_options("LINKER:--disable-new-dtags")
endif()


#
# Dependencies
#

find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Network CONFIG REQUIRED)
find_package(FFTW)
if (NOT WIN32)
    find_package(PCAP REQUIRED)
endif()


#
# Dynamic Config
#

set(MANUAL_PYTHON_PATH "" CACHE STRING "User-specified python root path")

option(DEVELOPMENT_BUILD "Runnable in-place when building" OFF)
option(SELF_CONTAINED_APP "Package in a self-contained format with relative linking" OFF)

if (SELF_CONTAINED_APP OR APPLE)
    SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    SET(CMAKE_INSTALL_RPATH "")
    set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
endif()


#
# Python Config
#

if ( MANUAL_PYTHON_PATH )
    message("Setting manual Python path")
    message("${MANUAL_PYTHON_PATH}")
    set ( Python3_ROOT_DIR "${MANUAL_PYTHON_PATH}" )

endif()
find_package (Python3 3.9 EXACT COMPONENTS Interpreter Development REQUIRED)


#
# Plugin installing and loading
#

if (SELF_CONTAINED_APP AND LINUX)
    set(HOBBITS_PLUGINS_INSTALL_PATH "${CMAKE_INSTALL_BINDIR}/../plugins")
    list(APPEND HOBBITS_PLUGINS_PATHS "../plugins")
elseif(WIN32)
    set(HOBBITS_PLUGINS_INSTALL_PATH "plugins")
    list(APPEND HOBBITS_PLUGINS_PATHS "plugins")
elseif(APPLE)
    set(HOBBITS_PLUGINS_INSTALL_PATH "hobbits.app/Contents/PlugIns/hobbits")
    list(APPEND HOBBITS_PLUGINS_PATHS "../PlugIns/hobbits")
else()
    set(HOBBITS_PLUGINS_INSTALL_PATH "${CMAKE_INSTALL_LIBDIR}/hobbits/plugins")
    list(APPEND HOBBITS_PLUGINS_PATHS "${CMAKE_INSTALL_FULL_LIBDIR}/hobbits/plugins")
    list(APPEND HOBBITS_PLUGINS_PATHS "~/.local/share/hobbits/plugins")
endif()

# in dev mode, install and search in same absolute dir
if(DEVELOPMENT_BUILD)
    set(HOBBITS_PLUGINS_INSTALL_PATH "${CMAKE_CURRENT_BINARY_DIR}/plugins")
    set(HOBBITS_PLUGINS_PATHS "${CMAKE_CURRENT_BINARY_DIR}/src/hobbits-plugins")
endif()

string(REPLACE ";" ":" HOBBITS_PLUGINS_PATH "${HOBBITS_PLUGINS_PATHS}")


#
# CPack
#

if (NOT DEFINED CPACK_GENERATOR)
    if(WIN32)
        set(CPACK_GENERATOR "ZIP")
    elseif(SELF_CONTAINED_APP OR APPLE)
        set(CPACK_GENERATOR "TXZ")
    else()
        set(CPACK_GENERATOR "RPM;DEB")
    endif()
endif()

set(CPACK_PACKAGE_NAME "hobbits")
set(CPACK_PACKAGE_VENDOR "Hobbits Team")
set(CPACK_PACKAGE_DESCRIPTION "A multi-platform GUI for bit-based analysis, processing, and visualization.")
set(CPACK_HELP_LINK "https://mahlet-inc.github.io/")
set(CPACK_PACKAGE_CONTACT "adam@smr.llc")
set(CPACK_PACKAGE_CHECKSUM "SHA512")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/src/hobbits-gui/images/icons/HobbitsRingSmall.png")
set(CPACK_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
set(CPACK_CREATE_DESKTOP_LINKS ON)

set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")

set(CPACK_DEBIAN_PACKAGE_NAME "hobbits")
set(CPACK_DEBIAN_COMPRESSION_TYPE "xz")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_DEBUGINFO_PACKAGE ON)
set(CPACK_DEB_COMPONENT_INSTALL OFF)

set(CPACK_RPM_COMPRESSION_TYPE "xz")
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_RPM_COMPONENT_INSTALL OFF)

set(CPACK_ARCHIVE_COMPONENT_INSTALL OFF)

cpack_add_component("runtime")
set(CPACK_DEBIAN_RUNTIME_PACKAGE_NAME "hobbits")
set(CPACK_DEBIAN_RUNTIME_PACKAGE_DEPENDS "libqt5core5a, libqt5widgets5, libqt5network5, libfftw3-3, libpcap0.8")
set(CPACK_RPM_RUNTIME_PACKAGE_NAME "hobbits")
set(CPACK_RPM_RUNTIME_PACKAGE_REQUIRES "qt5-qtbase, qt5-qtbase-gui, fftw, libpcap")

cpack_add_component("dev")
set(CPACK_DEBIAN_DEV_PACKAGE_NAME "hobbits-dev")
set(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS "hobbits, qt5-default")
set(CPACK_RPM_DEV_PACKAGE_NAME "hobbits-devel")
set(CPACK_RPM_DEV_PACKAGE_REQUIRES "hobbits, qt5-qtbase-devel")

set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "Hobbits Runtime")
set(CPACK_COMPONENT_DEV_DISPLAY_NAME "Hobbits Development Tools")

set(CPACK_COMPONENTS_ALL runtime dev)

set(CPACK_RPM_PACKAGE_REQUIRES "qt5-qtbase-devel, qt5-qtbase-gui, fftw, libpcap")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "qt5-default, libfftw3-3, libpcap0.8")
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)

#
# Add Files
#

add_subdirectory(src)
add_subdirectory(wizards)


#
# Install Additional Files when Necessary
#

include(PackExternalDeps)
if (SELF_CONTAINED_APP OR APPLE OR WIN32)
    pack_qt_libs()
    pack_python()
    pack_fftw()
    pack_pcap()
endif()

include(CPack)
