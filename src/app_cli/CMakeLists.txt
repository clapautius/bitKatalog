find_path(READLINE_INCLUDE readline/readline.h)
if (NOT READLINE_INCLUDE)
  message (FATAL_ERROR "readline headers not found.")
endif (NOT READLINE_INCLUDE)

find_library(READLINE_LIB_DIR readline)
if (NOT READLINE_LIB_DIR)
  message (FATAL_ERROR "readline library not found.")
endif (NOT READLINE_LIB_DIR)


add_executable(bitKatalog_cli xfcapp_cli.cpp interface.cpp ../fs.cpp ../misc.cpp ../plugins.cpp)
include_directories(${bitKatalog_SOURCE_DIR}/src ${bitKatalog_SOURCE_DIR}/src/xfclib ${READLINE_INCLUDE} ${LIBXML_INCLUDE})
link_directories(${bitKatalog_BINARY_DIR}/xfclib ${READLINE_LIB_DIR} ${LIBXML_LIB_DIR})
target_link_libraries(bitKatalog_cli xfc xml2 readline)
