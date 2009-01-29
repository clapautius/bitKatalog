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
#include <kvbox.h>

#include "outputwindow.h"


OutputWindow::OutputWindow()
    : KPageDialog()
{
    setCaption("Result");
    setButtons(KDialog::Close);
    setModal(true);
    layout();
}


OutputWindow::~OutputWindow()
{
}


void OutputWindow::layout()
{
    resize(800,450);
    KVBox *pBox1=new KVBox();
    KPageWidgetItem *pPage1=addPage(pBox1, QString("Results"));
    pPage1->setHeader(QString("Results"));

    mpEditArea=new KTextEdit(pBox1);
    mpEditArea->setReadOnly(true);
}


void OutputWindow::addText( std::string lText)
{
    mpEditArea->insertPlainText(lText.c_str() );
    mpEditArea->insertPlainText("\n");
}

