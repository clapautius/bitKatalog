project(bitKatalog)
cmake_minimum_required(VERSION 2.6)

# :release:
#set (CMAKE_VERBOSE_MAKEFILE 1)

# In case you don't want the KDE file dialogs, but the simpler Qt ones.
# This is off by default, but on when building an AppImage (using 'build-appimage' script).
#add_definitions(-DUSE_QT_FILEDLG)

if (CLI_ONLY)
   message( STATUS "KDE app. will not be built !")
endif (CLI_ONLY)

add_subdirectory(src)
