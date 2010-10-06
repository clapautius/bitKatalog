/***************************************************************************
 *   Copyright (C) 2010 by Tudor Marian Pristavu                           *
 *   clapautiusAtGmailDotCom                                               *
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
#ifndef LOCALFILES_H
#define LOCALFILES_H

#include <vector>

#include <kpagedialog.h>

#include <k3buttonbox.h>
#include <kpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <Qt3Support/q3vgroupbox.h>
#include <Qt3Support/q3hbox.h>
#include <qtreewidget.h>
#include <kvbox.h>
#include <QFileInfo>

#include "xfcapp.h"
#include "xmlentityitem.h"


class LocalFilesBox : public KPageDialog
{
    Q_OBJECT
            
public:

    LocalFilesBox(std::vector<QFileInfo> files);

    virtual ~LocalFilesBox();

public slots:

protected slots:  
    
    virtual void accept();
        
private:
    
    void connectButtons();
    
    void layout();

    std::vector<QFileInfo> mFiles;
    
};

#endif
