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
#ifndef SCANTHREAD_H
#define SCANTHREAD_H

#include <qthread.h>
#include <string>

#include "xfc.h"

/**
@author Tudor Pristavu
*/
class ScanThread : public QThread
{
private:
    Xfc *mpCatalog;
    
    std::string mPath, mDiskName;

    std::string mErrorMessage;

    int mReturnValue;
    
    bool mStopNow;

    bool mDontComputeSha;
    
public:
    ScanThread(Xfc*, std::string, std::string,
        bool dontComputeSha=false);

    ~ScanThread();
    
    virtual void run();
    
    int returnValue();
    
    void stopThread();

    std::string getErrorMessage() const;

};

#endif
