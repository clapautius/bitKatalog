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
#include "main.h"

class VerifyThread : public QThread
{
public:
    VerifyThread(Xfc*, string catalogPath, string diskPath,
                 volatile const bool *pAbortFlag, vector<EntityDiff> &);

    ~VerifyThread();
    
    virtual void run();
    
    int returnValue();
    
    std::string getCurrentFile();
    
    void setCurrentFile(std::string);

    int getResultCode() const;
    
private:

    int verifyDirectory(string catalogPath, string diskPath);

    EntityDiff compareItems(string catalogName, XfcEntity &rEnt, string diskPath,
                            bool &rShaWasMissing);

    Xfc* mpCatalog;
    
    std::string mCatalogPath, mDiskPath;

    vector<EntityDiff> mrDifferences;

    std::string mCurrentFile;
    
    QMutex mMutex;
    
    int mResultCode;

    volatile const bool *mpAbortFlag;
};

#endif
