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
#include "verifythread.h"
#include "fs.h"
#include "misc.h"
#include "xfcEntity.h"
#include "main.h"


VerifyThread::VerifyThread(
    Xfc* lpCatalog, std::string lCatalogPath, std::string lDiskPath,
    std::vector<std::string> &lOnlyInCatalog, std::vector<std::string> &lOnlyOnDisk,
    std::vector<std::string> &lDifferent, std::vector<std::string> &lWrongSum)        
    : QThread(),
      mrOnlyInCatalog(lOnlyInCatalog),
      mrOnlyOnDisk(lOnlyOnDisk),
      mrWrongSum(lWrongSum),
      mrDifferent(lDifferent)
{
    mpCatalog=lpCatalog;
    mCatalogPath=lCatalogPath;
    mDiskPath=lDiskPath;
    mStopNow=false;
}


VerifyThread::~VerifyThread()
{
}


void
VerifyThread::run()
{
  mResultCode=verifyDirectory(mCatalogPath, mDiskPath, mrOnlyInCatalog,
                              mrOnlyOnDisk, mrDifferent, mrWrongSum);
}


void
VerifyThread::stopThread()
{
    mMutex.lock();
    mStopNow=true;
    mMutex.unlock();
}


/**
 * @param[in] lDifferent - different size or smth. else (no sha)
 * @param[in] lWrongSum - different sha
 *
 * @retval < 0 - failure
 * @retval 0 - stopped by user
 * @retval 1 - no differences, sha was present
 * @retval 2 - no differences, there were files without sha
 * @retval 3 - there were differences, sha was present
 * @retval 4 - there were differences, there were files without sha
 **/
int
VerifyThread::verifyDirectory(
    std::string lCatalogPath, std::string lDiskPath, std::vector<std::string> &lOnlyInCatalog,
    std::vector<std::string> &lOnlyOnDisk, std::vector<std::string> &lDifferent,
    std::vector<std::string> &lWrongSum)
{
    std::vector<std::string> lNamesInCatalog;
    std::vector<std::string> lShaSumsInCatalog;
    std::vector<std::string> lNamesOnDisk;
    map<string, string> details;
    XfcEntity lEnt;
    std::string lSum="";    
    bool lStopNow;

    bool shaWasMissing=false;
    
    mMutex.lock();
    lStopNow=mStopNow;
    mMutex.unlock();

    msgDebug("Verifying dir: ", lDiskPath);
    msgDebug("Catalog path is: ", lCatalogPath);
    
    if(lStopNow)
        return 0;
        
    EntityIterator lEntIterator(*mpCatalog, lCatalogPath);
    while (lEntIterator.hasMoreChildren()) {
        lEnt=lEntIterator.getNextChild();
        lNamesInCatalog.push_back(lEnt.getName());
        details=lEnt.getDetails();
        lShaSumsInCatalog.push_back(details["sha256"]); // :fixme: - what about sha1?
    }
    
    try {
        lNamesOnDisk=getFileListInDir(lDiskPath);
    }
    catch(std::string e) {
        // do something (add this to error list?) :fixme:
        return -1;
    }
    
    for (unsigned int i=0;i<lNamesOnDisk.size();i++)
        if (isDirectory(lDiskPath+"/"+lNamesOnDisk[i])) {
            bool lFound=true;
            // check if it exists in catalog
            for (unsigned int j=0;j<lNamesInCatalog.size();j++)
                if (lNamesInCatalog[j]==lNamesOnDisk[i]) {
                    lFound=true;
                    break;
                }
            if (lFound) {
              verifyDirectory(lCatalogPath+"/"+lNamesOnDisk[i],
                              lDiskPath+"/"+lNamesOnDisk[i],
                              lOnlyInCatalog, lOnlyOnDisk,
                              lDifferent, lWrongSum);
            }
            else {
                lOnlyOnDisk.push_back(lNamesOnDisk[i]);
                lNamesOnDisk.erase(lNamesOnDisk.begin()+i);
                i--;
            }
        }

    for (unsigned int i=0;i<lNamesInCatalog.size();i++) {
        msgDebug("verifyDirectory(): checking (catalog name): ", lNamesInCatalog[i]);
        bool lFound=false;
        bool lEqual=true;
        bool wrongSumFlag=false;
        for (unsigned int j=0;j<lNamesOnDisk.size();j++) {
            mMutex.lock();
            lStopNow=mStopNow;
            mMutex.unlock();
            if (lStopNow)
                return 0;
            if (lNamesInCatalog[i]==lNamesOnDisk[j]) {
                msgDebug("verifyDirectory(): comparing with (disk name): ", lNamesOnDisk[j]);
                lFound=true;
                wrongSumFlag=false;
                std::string lDiskPathCurrent=lDiskPath+"/"+lNamesOnDisk[j];
                if (!isDirectory(lDiskPathCurrent)) {
                    // first check size
                    // :todo:
                    
                    if (lEqual) {
                        if (lShaSumsInCatalog[i]!="") {
                            try {
                                setCurrentFile( lDiskPathCurrent);
                                lSum=sha1sum(lDiskPathCurrent, std::string("sha1sum"));
                            }
                            catch(std::string e) {
                                lSum="invalid file"; // :fixme: - do something
                            }
                        }
                        else {
                            shaWasMissing=true;
                            lSum="";
                        }
                        if (lSum!=lShaSumsInCatalog[i]) {
                            lEqual=false;
                            wrongSumFlag=true;
                        }
                    }
                } // end file compare
                lNamesOnDisk.erase(lNamesOnDisk.begin()+j);
                break;
            }
        }
        if (!lFound) { // only in catalog
            lOnlyInCatalog.push_back(lCatalogPath+"/"+lNamesInCatalog[i]);
        }
        if (lFound && !lEqual) { // wrong sum, size, etc.
          if (wrongSumFlag) {
              lWrongSum.push_back(lCatalogPath+"/"+lNamesInCatalog[i]);
              msgInfo("wrong sum for: ", lNamesInCatalog[i]);
              msgInfo(lSum, " / ", lShaSumsInCatalog[i]);
          }
          else {
              lDifferent.push_back(lCatalogPath+"/"+lNamesInCatalog[i]);
              msgInfo("wrong something for: ", lNamesInCatalog[i]);
          }
        }
    }

    // the remaining elements are only on disk
    for(unsigned int i=0;i<lNamesOnDisk.size();i++) 
      lOnlyOnDisk.push_back(lDiskPath+"/"+lNamesOnDisk[i]);
        
#if defined(MTP_DEBUG)
    cout<<"verifyDirectory(): final report"<<endl;
    cout<<"  only in catalog"<<endl;
    for(int i=0;i<lOnlyInCatalog.size();i++)
        cout<<lOnlyInCatalog[i]<<endl;
    cout<<"  only on disk"<<endl;
    for(int i=0;i<lOnlyOnDisk.size();i++)
        cout<<lOnlyOnDisk[i]<<endl;
    cout<<"  wrong sum"<<endl;
    for(int i=0;i<lWrongSum.size();i++)
        cout<<lWrongSum[i]<<endl;
    cout<<"  wrong something"<<endl;
    for(int i=0;i<lDifferent.size();i++)
      cout<<lDifferent[i]<<endl;
#endif

    int s1,s2,s3,s4;
    s1=lOnlyInCatalog.size();
    s2=lOnlyOnDisk.size();
    s3=lWrongSum.size();
    s4=lDifferent.size();
    if (0 == s1 && 0 == s2 && 0 == s3 && 0 == s4) {
      if(shaWasMissing)
        return 2;
      else
        return 1;
    }
    else {
      if(shaWasMissing)
        return 4;
      else
        return 3;
    }      
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
