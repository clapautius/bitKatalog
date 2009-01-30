/***************************************************************************
 *   Copyright (C) 2004 by Tudor Marian Pristavu                           *
 *   tudor.mp@home.ro                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "plugins.h"

#include <string>
#include <vector>

#if defined(XFC_DEBUG)
#include <iostream>
using namespace std;
#endif

#include "fs.h"
#include "xfc.h"
#include "misc.h"

int
sha1Callback(std::string fileName,
             std::string &xmlParam, std::string &xmlValue, std::vector<std::string> &xmlAttrs)
{
    int rc=-1;
    std::string lSum=sha1sum(fileName, std::string("sha1sum"));
    if (!lSum.empty()) {
        xmlParam="sum";
        xmlValue=lSum;
        xmlAttrs.clear();
        xmlAttrs.push_back("type");
        xmlAttrs.push_back("sha1");
        rc=0;
    }
    else {
        rc=-1;
    }
    return rc;
}


int
sha256Callback(std::string fileName,
             std::string &xmlParam, std::string &xmlValue, std::vector<std::string> &xmlAttrs)
{
    int rc=-1;
    std::string lSum=sha1sum(fileName, std::string("sha256sum"));
    if (!lSum.empty()) {
        xmlParam="sum";
        xmlValue=lSum;
        xmlAttrs.clear();
        xmlAttrs.push_back("type");
        xmlAttrs.push_back("sha256");
        rc=0;
    }
    else {
        rc=-1;
    }
    return rc;
}
