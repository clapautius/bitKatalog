add_library(xfc xfclib.cpp xfc.cpp xfcEntity.cpp)
include_directories(${LIBXML_INCLUDE} ${bitKatalog_SOURCE_DIR}/src)
target_link_libraries(xfc xml2)
