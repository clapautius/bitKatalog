add_library(xfc xfclib.cpp fs.cpp misc.cpp plugins.cpp sha1.c sha2.c xfc.cpp xfcEntity.cpp)
include_directories(${LIBXML_INCLUDE} ${bitKatalog_SOURCE_DIR}/src)
target_link_libraries(xfc xml2)
