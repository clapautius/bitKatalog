add_executable(bitKatalog_cli xfcapp_cli.cpp interface.cpp ../fs.cpp ../misc.cpp ../plugins.cpp)
include_directories(/usr/include/libxml2 ${bitKatalog_SOURCE_DIR}/src ${bitKatalog_SOURCE_DIR}/src/xfclib)
link_directories(${bitKatalog_BINARY_DIR}/xfclib)
target_link_libraries(bitKatalog_cli xfc xml2 readline)
