project(bitKatalog)
cmake_minimum_required(VERSION 2.4)
cmake_policy(SET CMP0003 NEW)

# :release:
#set (CMAKE_VERBOSE_MAKEFILE 1)

if (CLI_ONLY)
   message( STATUS "KDE app. will not be built !")
endif (CLI_ONLY)

add_subdirectory(src)
