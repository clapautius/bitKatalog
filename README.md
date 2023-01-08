Synopsis
=========

Qt app. for managing collections of files / disks (back-ups on external hdds, blurays, dvds, etc.).

Features:
 * Qt (cross-platform)
 * support for tags / labels (faster way to locate relevant information)
 * integrity support (sha1 / sha256 checksum support)

Screenshot
==========

![Screenshot](https://github.com/clapautius/bitKatalog/blob/master/doc/bitkatalog-qt-screenshot.png)

Dev. issues
===========

XML issues
----------

  In the initial version of the XFC (Xml File Catalog) DTD, items were disks and
files and files had different types: 'file' or 'dir'. That was unintuitive
(even illogical) and not very efficient.
  The new version has disks, dirs and files.

  In order to convert old xml files to the new structure, an xslt file is
provided (xfc0ToXfc.xsl).

Checksums
----------

The project uses sources from PolarSSL library (http://polarssl.org) for sha1 &
sha256 checksums.
