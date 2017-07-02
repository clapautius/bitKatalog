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
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "fs.h"
#include "misc.h"
#include "plugins.h"
#include "sha1.h"
#include "sha2.h"
#include "xfc.h"

using namespace std;

/// not used anymore
int sha1Callback(string fileName, string *pParam, string *pValue,
                 volatile const bool *pAbortFlag)
{
    int rc = -1;
    // the second string is the name of the binary
    std::string lSum = execChecksum(fileName, std::string("sha1sum"), pAbortFlag);
    if (!lSum.empty()) {
        *pParam = SHA1LABEL;
        *pValue = lSum;
        rc = 0;
    } else {
        rc = -1;
    }
    return rc;
}

/// not used anymore
int sha256Callback(string fileName, string *pParam, string *pValue,
                   volatile const bool *pAbortFlag)
{
    int rc = -1;
    // the second string is the name of the binary
    std::string lSum = execChecksum(fileName, std::string("sha256sum"), pAbortFlag);
    if (!lSum.empty()) {
        *pParam = SHA256LABEL;
        *pValue = lSum;
        rc = 0;
    } else {
        rc = -1;
    }
    return rc;
}

int sha1UsingBufCallback(const char *buf, unsigned bufLen, bool isFirstChunk,
                         bool isLastChunk, string *pParam, string *pValue,
                         volatile const bool *pAbortFlag __attribute__((unused)))
{
    static sha1_context ctx;
    if (isFirstChunk) {
        bzero(&ctx, sizeof(ctx));
        sha1_starts(&ctx);
    }
    if (bufLen > 0) {
        sha1_update(&ctx, (unsigned char *)buf, bufLen);
    }
    if (isLastChunk) {
        unsigned char hash[20];
        ostringstream ostr;
        ostr << hex << setfill('0');
        sha1_finish(&ctx, hash);
        for (unsigned int i = 0; i < 20; i++) ostr << setw(2) << (int)hash[i];
        *pParam = SHA1LABEL;
        *pValue = string(ostr.str());
    }
    return 0;
}

int sha256UsingBufCallback(const char *buf, unsigned bufLen, bool isFirstChunk,
                           bool isLastChunk, string *pParam, string *pValue,
                           volatile const bool *pAbortFlag __attribute__((unused)))
{
    static sha2_context ctx;
    if (isFirstChunk) {
        bzero(&ctx, sizeof(ctx));
        sha2_starts(&ctx, 0);
    }
    if (bufLen > 0) {
        sha2_update(&ctx, (unsigned char *)buf, bufLen);
    }
    if (isLastChunk) {
        unsigned char hash[32];
        ostringstream ostr;
        ostr << hex << setfill('0');
        sha2_finish(&ctx, hash);
        for (unsigned int i = 0; i < 32; i++) ostr << setw(2) << (int)hash[i];
        *pParam = SHA256LABEL;
        *pValue = string(ostr.str());
    }
    return 0;
}
