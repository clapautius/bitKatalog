project(bitKatalog)
cmake_minimum_required(VERSION 2.4)

if (CMAKE_MINOR_VERSION GREATER 5)
  cmake_policy(SET CMP0003 NEW)
endif (CMAKE_MINOR_VERSION GREATER 5)

# :release:
#set (CMAKE_VERBOSE_MAKEFILE 1)

# In case you don't want the KDE file dialogs, but the simpler Qt ones
add_definitions(-DUSE_QT_FILEDLG)

if (CLI_ONLY)
   message( STATUS "KDE app. will not be built !")
endif (CLI_ONLY)

add_subdirectory(src)
