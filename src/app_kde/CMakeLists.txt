find_package (KDE4 REQUIRED)

set (bitKatalog_srcs bitkatalog.cpp bitkatalogview.cpp pref.cpp main.cpp adddiskbox.cpp detailsbox.cpp newcatalogbox.cpp outputwindow.cpp scanthread.cpp searchbox.cpp verifythread.cpp xmlentityitem.cpp labelsbox.cpp localfiles.cpp)

include (KDE4Defaults)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")

kde4_add_executable (bitKatalog ${bitKatalog_srcs})

include_directories (/usr/include/libxml2 /usr/include/qt4 ${KDE4_INCLUDES} 
${bitKatalog_SOURCE_DIR}/src ${bitKatalog_SOURCE_DIR}/src/xfclib)

link_directories (${bitKatalog_BINARY_DIR}/xfclib)

target_link_libraries (bitKatalog xfc xml2 ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBRARY} ${KDE4_KPARTS_LIBRARY} ${KDE4_KDE3SUPPORT_LIBRARY})
