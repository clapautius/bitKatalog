find_path(LIBXML_INCLUDE libxml/parser.h PATHS /usr/include/libxml2)
if (NOT LIBXML_INCLUDE)
  message (FATAL_ERROR "libxml2 headers not found.")
endif (NOT LIBXML_INCLUDE)

find_library(LIBXML_LIB_DIR xml2)
if (NOT LIBXML_LIB_DIR)
  message (FATAL_ERROR "libxml2 library not found.")
endif (NOT LIBXML_LIB_DIR)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LARGEFILE_SOURCE ")

add_subdirectory(xfclib)
add_subdirectory(app_cli)

find_path(BOOST_UNITTEST_INCLUDE boost/test/unit_test.hpp)
if (NOT BOOST_UNITTEST_INCLUDE)
  message (WARNING "boost unit_test not found, tests won't be compiled")
else()
  add_subdirectory(tests)
endif()

if (NOT CLI_ONLY)
   add_subdirectory(app_kde)
endif (NOT CLI_ONLY)
