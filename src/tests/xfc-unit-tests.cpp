#include <iostream>
#include <string>

#include "xfclib/xfc.h"
#include "xfclib/xfcEntity.h"

#define BOOST_TEST_MODULE XfcTest

#include <boost/test/unit_test.hpp>

using std::string;

BOOST_AUTO_TEST_CASE(xfc_test)
{
    Xfc xfc_catalog;
    xmlBufferPtr xmlBuf = xmlBufferCreateSize(1024);
    int len;
    const char *p = NULL;
    const char *q = NULL;

    // test 1
    xfc_catalog.createNew("tst");
    len = xmlNodeDump(xmlBuf, xfc_catalog.getXmlDocPtr(), xfc_catalog.getNodeForPath("/"),
                      0, 0);
    xmlBuf->content[len] = 0;
    p = (const char *)xmlBuf->content;
    q = "<catalog name=\"tst\"/>";
    BOOST_TEST(p == q);

    XfcEntity root = xfc_catalog.getEntityFromPath("/");

    // test 2
    xmlBufferEmpty(xmlBuf);
    root.setComment("test comment");
    len = xmlNodeDump(xmlBuf, xfc_catalog.getXmlDocPtr(), root.getXmlNode(), 0, 0);
    xmlBuf->content[len] = 0;
    p = (const char *)xmlBuf->content;
    q = "<catalog name=\"tst\"><comment>test comment</comment></catalog>";
    BOOST_TEST(p == q);

    // test 3
    xmlBufferEmpty(xmlBuf);
    xfc_catalog.addNewDiskToXmlTree("test_disk", "", "descr");
    len = xmlNodeDump(xmlBuf, xfc_catalog.getXmlDocPtr(), root.getXmlNode(), 0, 0);
    xmlBuf->content[len] = 0;
    p = (const char *)xmlBuf->content;
    q = "<catalog name=\"tst\"><comment>test comment</comment>"
        "<disk><name>test_disk</name><description>descr</description></disk></catalog>";
    BOOST_TEST(p == q);

    // test 4
    xmlBufferEmpty(xmlBuf);
    XfcEntity ent = xfc_catalog.getEntityFromPath("/test_disk");
    ent.setStorageDev("hda1");
    len = xmlNodeDump(xmlBuf, xfc_catalog.getXmlDocPtr(), root.getXmlNode(), 0, 0);
    xmlBuf->content[len] = 0;
    p = (const char *)xmlBuf->content;
    q = "<catalog name=\"tst\"><comment>test comment</comment>"
        "<disk><name>test_disk</name><storage_dev>hda1</storage_dev>"
        "<description>descr</description></disk></catalog>";
    BOOST_TEST(p == q);
}
