add_executable(bitKatalog bitkatalog.cpp ../fs.cpp ../misc.cpp ../plugins.cpp
  bitkatalogview.cpp pref.cpp)

include_directories(/usr/include/libxml2 ${bitKatalog_SOURCE_DIR}/src ${bitKatalog_SOURCE_DIR}/src/xfclib)
link_directories(${bitKatalog_BINARY_DIR}/xfclib)
target_link_libraries(bitKatalog xfc xml2)
