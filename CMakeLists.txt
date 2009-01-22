project(bitKatalog)

if (CLI_ONLY)
   message( STATUS "KDE app. will not be built !")
endif (CLI_ONLY)

add_subdirectory(src)
