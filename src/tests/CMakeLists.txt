add_executable(xfctst xfc-unit-tests.cpp)
include_directories(${LIBXML_INCLUDE} ${BOOST_UNITTEST_INCLUDE} ${bitKatalog_SOURCE_DIR}/src ${bitKatalog_SOURCE_DIR}/src/xfclib)

find_library(BOOST_TEST_LIB_DIR boost_unit_test_framework)
if (NOT BOOST_TEST_LIB_DIR)
  message (FATAL_ERROR "boost_unit_test_framework library not found.")
endif ()

ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)
link_directories(${bitKatalog_BINARY_DIR}/xfclib ${LIBXML_LIB_DIR})
target_link_libraries(xfctst xfc xml2 boost_unit_test_framework)
