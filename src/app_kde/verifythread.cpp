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


VerifyThread::VerifyThread(
    Xfc* pCatalog, string catalogPath, string diskPath, vector<EntityDiff> *pDifferences)
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
    mResultCode=verifyDirectory(mCatalogPath, mDiskPath);
}


/**
 * helper function
 **/
EntityDiff
VerifyThread::compareItems(string catalogName, XfcEntity &rEnt, string diskPath,
                           bool &rShaWasMissing)
{
    EntityDiff diff;
    string str;
    map<string, string> details;
    details=rEnt.getDetails();
    ostringstream ostr;
    msgDebug("comparing items with name ", catalogName);

    if (!isDirectory(diskPath)) {
        // first check size
        ostr<<getFileSize(diskPath);
        if (ostr.str() != details["size"]) {
            diff.type=eDiffSize;
            diff.name=catalogName;
            diff.catalogValue=details["size"];
            diff.diskValue=ostr.str();
            msgDebug("different sizes for ", catalogName);
            msgDebug("catalog value=", details["size"], ", disk value=", ostr.str());
            return diff;
        }

        if (details[SHA256LABEL].empty() && details[SHA1LABEL].empty())
            rShaWasMissing=true;

        // sha256
        if (!details[SHA256LABEL].empty()) {
            try {
                setCurrentFile(diskPath);
                str=execChecksum(diskPath, std::string("sha256sum"), &mAbortFlag);
                if (str!=details[SHA256LABEL]) {
                    diff.type=eDiffSha256Sum;
                    diff.name=catalogName;
                    diff.catalogValue=details[SHA256LABEL];
                    diff.diskValue=str;
                    msgDebug("different sha256 for ", catalogName);
                    msgDebug("catalog value=", details[SHA256LABEL], ", disk value=", str);
                    return diff;
                }
            }
            catch(std::string e) {
                diff.type=eDiffError;
                diff.name=catalogName;
            }
        }

        // sha1
        if (!details[SHA1LABEL].empty()) {
            try {
                setCurrentFile(diskPath);
                str=execChecksum(diskPath, std::string("sha1sum"), &mAbortFlag);
                if (str!=details[SHA1LABEL]) {
                    diff.type=eDiffSha1Sum;
                    diff.name=catalogName;
                    diff.catalogValue=details[SHA1LABEL];
                    diff.diskValue=str;
                    msgDebug("different sha1 for ", catalogName);
                    msgDebug("catalog value=", details[SHA1LABEL], ", disk value=", str);
                    return diff;
                }
            }
            catch(std::string e) {
                diff.type=eDiffError;
                diff.name=catalogName;
            }
        }
    } // end file compare

    return diff;
}


/**
 * @retval < 0 - failure
 * @retval 0 - stopped by user
 * @retval 1 - ok, checksums were present
 * @retval 2 - ok, some checksums were missing
 **/
int
VerifyThread::verifyDirectory(string catalogPath, string diskPath)
{
    vector<string> namesInCatalog;
    map<string, string> details;
    vector<string> lShaSumsInCatalog;
    vector<string> namesOnDisk;
    XfcEntity ent;
    EntityDiff diff;
    bool stopNow;
    bool shaWasMissing=false;
    
    mMutex.lock();
    stopNow=mAbortFlag;
    mMutex.unlock();

    msgDebug("Verifying dir: ", diskPath);
    msgDebug("Catalog path is: ", catalogPath);

    if (stopNow) {
        msgInfo(__FUNCTION__, ": stopped by user");
        return 0;
    }
    
    EntityIterator entIterator(*mpCatalog, catalogPath);
    while (entIterator.hasMoreChildren()) {
        ent=entIterator.getNextChild();
        namesInCatalog.push_back(ent.getName());
        details=ent.getDetails();
    }
    
    try {
        namesOnDisk=getFileListInDir(diskPath);
    }
    catch(std::string e) {
        // do something (add this to error list?) :fixme:
        msgWarn(__FUNCTION__, ": error in getFileListInDir()");
        return -1;
    }
    
    for (unsigned int i=0;i<namesOnDisk.size();i++)
        if (isDirectory(diskPath+"/"+namesOnDisk[i])) {
            bool found=true;
            // check if it exists in catalog
            for (unsigned int j=0;j<namesInCatalog.size();j++)
                if (namesInCatalog[j]==namesOnDisk[i]) {
                    found=true;
                    break;
                }
            if (found) {
                verifyDirectory(catalogPath+"/"+namesOnDisk[i],
                                diskPath+"/"+namesOnDisk[i]);
            }
            else {
                diff.type=eDiffOnlyOnDisk;
                diff.name=namesOnDisk[i];
                mpDifferences->push_back(diff);
                msgInfo("Difference (only on disk) for: ", namesOnDisk[i]);
                namesOnDisk.erase(namesOnDisk.begin()+i);
                i--;
            }
        }

    // :tmp:
    //for (unsigned int i=0; i<namesOnDisk.size(); i++) {
    //    msgDebug(__FUNCTION__, ": a name on disk: ", namesOnDisk[i]);
    //}
    
    for (unsigned int i=0;i<namesInCatalog.size();i++) {
        msgDebug(__FUNCTION__, ": checking (catalog name): ", namesInCatalog[i]);
        bool found=false;
        for (unsigned int j=0;j<namesOnDisk.size();j++) {
            mMutex.lock();
            stopNow=mAbortFlag;
            mMutex.unlock();
            if (stopNow) {
                msgInfo(__FUNCTION__, ": stopped by user");
                return 0;
            }
            if (namesInCatalog[i]==namesOnDisk[j]) {
                msgDebug("verifyDirectory(): comparing with (disk name): ", namesOnDisk[j]);
                found=true;
                std::string diskPathCurrent=diskPath+"/"+namesOnDisk[j];
                diff=compareItems(namesInCatalog[i], ent, diskPathCurrent, shaWasMissing);
                if (eDiffIdentical != diff.type) {
                    if (eDiffError == diff.type) {
                        throw std::string("Error comparing")+namesInCatalog[i];
                    }
                    else {
                        gkLog<<xfcInfo<<"Difference (type="<<(int)diff.type<<") for: "<<diff.name<<eol;
                        gkLog<<"  catalog value="<<diff.catalogValue<<", disk value="<<diff.diskValue<<eol;
                        mpDifferences->push_back(diff);
                    }
                }
                namesOnDisk.erase(namesOnDisk.begin()+j);
                break;
            }
        }
        if (!found) { // only in catalog
            diff.type=eDiffOnlyInCatalog;
            diff.name=catalogPath+"/"+namesInCatalog[i];
            msgInfo("Difference (only in catalog) for: ", diff.name);
            mpDifferences->push_back(diff);
        }
    }

    // the remaining elements are only on disk
    diff.type=eDiffOnlyOnDisk;
    for (unsigned int i=0;i<namesOnDisk.size();i++)  {
        diff.name=diskPath+"/"+namesOnDisk[i];
        msgInfo("Difference (only on disk) for: ", diff.name);
        mpDifferences->push_back(diff);
    }

    if (shaWasMissing)
        return 2;
    else
        return 1;
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
