add_subdirectory(xfclib)
add_subdirectory(app_cli)

if (NOT CLI_ONLY)
   add_subdirectory(app_kde)
endif (NOT CLI_ONLY)

