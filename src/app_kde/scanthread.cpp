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
#include "scanthread.h"

#include <kmessagebox.h>

ScanThread::ScanThread(Xfc *lpCatalog, std::string lPath, std::string lDiskName,
            bool dontComputeSha)
 : QThread()
{
    mpCatalog=lpCatalog;
    mPath=lPath;
    mDiskName=lDiskName;
    mStopNow=false;
    mDontComputeSha=dontComputeSha;
}


ScanThread::~ScanThread()
{
}


void ScanThread::run()
{
    try {
        mpCatalog->addPathToXmlTree(mPath, -1, mDiskName,
            "","","", mDontComputeSha);
    }
    catch(std::string e) {
        mReturnValue=1;
        mErrorMessage=e;
        return;
    }
    mReturnValue=0;
}


int ScanThread::returnValue()
{
    return mReturnValue;
} 


void ScanThread::stopThread()
{
    mStopNow=true;
} 


std::string ScanThread::getErrorMessage() const
{
    return mErrorMessage;
}
