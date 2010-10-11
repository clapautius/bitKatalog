/***************************************************************************
 *   Copyright (C) 2009 by Tudor Pristavu   *
 *   clapautiuAtGmaliDotCom   *
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


#ifndef _BITKATALOGPREF_H_
#define _BITKATALOGPREF_H_

#include <kpagedialog.h>
#include <klineedit.h>
#include <Qt/qframe.h>
#include <QRadioButton>


class bitKatalogPreferences : public KPageDialog
{
    Q_OBJECT
public:

    bitKatalogPreferences();

protected slots:  
    
    virtual void accept();
    virtual void reject();

private slots:

    virtual void setSearchPath();

private:

    void layout();

    KLineEdit *mpSearchPathEdit;

    QRadioButton *mpButton1, *mpButton2;
};


#endif // _BITKATALOGPREF_H_
