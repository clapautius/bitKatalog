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
#include <sstream>

#include "verifythread.h"
#include "fs.h"
#include "misc.h"
#include "xfcEntity.h"
#include "main.h"

using std::string;
using std::vector;
using std::map;


VerifyThread::VerifyThread(Xfc* pCatalog, string catalogPath, string diskPath,
                           vector<EntityDiff> *pDifferences)
    : QThread(),
      mpDifferences(pDifferences)
{
    mpCatalog=pCatalog;
    mCatalogPath=catalogPath;
    mDiskPath=diskPath;
    mAbortFlag=false;
}


VerifyThread::~VerifyThread()
{
}


void
VerifyThread::run()
{
    mResultCode=mpCatalog->verifyDirectory(mCatalogPath, mDiskPath, mDiskPath.length(),
                                           mpDifferences, &mAbortFlag);
}




int
VerifyThread::getResultCode() const
{
  return mResultCode;
}


std::string
VerifyThread::getCurrentFile()
{
    std::string lS;
    mMutex.lock();
    lS=mCurrentFile;
    mMutex.unlock();
    return lS;
}


void
VerifyThread::setCurrentFile(std::string lFile)
{
    mMutex.lock();
    mCurrentFile=lFile;
    mMutex.unlock();
}


void
VerifyThread::stopThread()
{
    mMutex.lock();
    mAbortFlag=true;
    mMutex.unlock();
}
