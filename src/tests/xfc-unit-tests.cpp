#include <iostream>
#include <string>

#include "xfclib/xfc.h"
#include "xfclib/xfcEntity.h"

#define BOOST_TEST_MODULE XfcTest

#include <boost/test/unit_test.hpp>

using std::string;

BOOST_AUTO_TEST_CASE( xfc_test )
{
    Xfc xfc_catalog;
    xmlBufferPtr xmlBuf = xmlBufferCreateSize(1024);
    int len;
    const char *p = NULL;
    const char *q = NULL;

    // test 1
    xfc_catalog.createNew("tst");
    len = xmlNodeDump(xmlBuf, xfc_catalog.getXmlDocPtr(),
                      xfc_catalog.getNodeForPath("/"), 0, 0);
    xmlBuf->content[len] = 0;
    p = (const char*)xmlBuf->content;
    q = "<catalog name=\"tst\"/>";
    BOOST_TEST(p == q);

    // test 2
    xmlBufferEmpty(xmlBuf);
    XfcEntity ent = xfc_catalog.getEntityFromPath("/");
    ent.setOrAddParam("comment", "test comment");
    len = xmlNodeDump(xmlBuf, xfc_catalog.getXmlDocPtr(),
                      xfc_catalog.getNodeForPath("/"), 0, 0);
    xmlBuf->content[len] = 0;
    p = (const char*)xmlBuf->content;
    q = "<catalog name=\"tst\"><comment>test comment</comment></catalog>";
    BOOST_TEST(p == q);
}
