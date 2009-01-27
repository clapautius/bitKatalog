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
#ifndef VERIFYTHREAD_H
#define VERIFYTHREAD_H

#include <qthread.h>
#include <qmutex.h>

#include "xfcapp.h"
#include "xfc.h"

/**
	@author Tudor Pristavu <tudor.pristavu@gmail.com>
*/
class VerifyThread : public QThread
{
public:
    VerifyThread(Xfc*, 
                 std::string lCatalogPath, std::string lDiskPath,
                 std::vector<std::string> &lOnlyInCatalog,
                 std::vector<std::string> &lOnlyOnDisk,
                 std::vector<std::string> &lDifferent, 
                 std::vector<std::string> &lWrongSum);

    ~VerifyThread();
    
    virtual void run();
    
    int returnValue();
    
    void stopThread();

    std::string getCurrentFile();
    
    void setCurrentFile(std::string);

    int getResultCode() const;
    
private:
    int verifyDirectory(std::string lCatalogPath, std::string lDiskPath,
                        std::vector<std::string> &lOnlyInCatalog,
                        std::vector<std::string> &lOnlyOnDisk,
                        std::vector<std::string> &lDifferent, 
                        std::vector<std::string> &lWrongSum);

    Xfc* mpCatalog;
    
    std::string mCatalogPath, mDiskPath;
    
    std::vector<std::string> &mrOnlyInCatalog, &mrOnlyOnDisk,
      &mrWrongSum, &mrDifferent;
    
    std::string mCurrentFile;
    
    QMutex mMutex;
    
    bool mStopNow;

    int mResultCode;
};

#endif
