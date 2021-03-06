/***************************************************************************
 *   Copyright (C) 2005 by Tudor Pristavu                                  *
 *   me@clapautius                                                         *
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
#include <kmessagebox.h>

#include "plugins.h"
#include "scanthread.h"

ScanThread::ScanThread(Xfc *lpCatalog, string lPath, string lDiskName,
                       ScanThreadParams &scanParams)
    : QThread()
{
    mpCatalog = lpCatalog;
    mPath = lPath;
    mDiskName = lDiskName;
    mStopNow = false;
    mComputeSha1 = scanParams.computeSha1;
    mComputeSha256 = scanParams.computeSha256;
    mpAbortFlag = scanParams.pAbortFlag;
}

ScanThread::~ScanThread() {}

void ScanThread::run()
{
    vector<Xfc::XmlParamForFileCallback> cbList1;
    vector<Xfc::XmlParamForFileChunkCallback> cbList2;

    if (mComputeSha1) {
        // cbList1.push_back(sha1Callback); // - used for testing
        cbList2.push_back(sha1UsingBufCallback);
    }
    if (mComputeSha256) {
        // cbList1.push_back(sha256Callback); // - used for testing
        cbList2.push_back(sha256UsingBufCallback);
    }
    try {
        mpCatalog->addPathToXmlTree(mPath, -1, mpAbortFlag, cbList1, cbList2, mDiskName,
                                    "", "", "");
    } catch (std::string e) {
        mReturnValue = 1;
        mErrorMessage = e;
        return;
    }
    mReturnValue = 0;
}

int ScanThread::returnValue() { return mReturnValue; }

void ScanThread::stopThread() { mStopNow = true; }

std::string ScanThread::getErrorMessage() const { return mErrorMessage; }

ScanThread::ScanThreadParams::ScanThreadParams()
{
    computeSha1 = computeSha256 = false;
    pAbortFlag = NULL;
}
