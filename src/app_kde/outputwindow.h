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
#ifndef OUTPUTWINDOW_H
#define OUTPUTWINDOW_H

#include <string>

#include <kpagedialog.h>
#include <ktextedit.h>
#include <QTableWidget>

class OutputWindow : public KPageDialog
{
   public:
    OutputWindow();

    ~OutputWindow();

    void addText(std::string);

   private:
    void layout();

    KTextEdit *mpEditArea;
};

class DiffOutputWindow : public KPageDialog
{
   public:
    DiffOutputWindow(int);

    ~DiffOutputWindow();

    void addText(std::string, std::string, std::string, unsigned int lines = 1);

    void finishedText();

   private:
    void layout();

    unsigned int mRows;

    unsigned int mCurrentRow;

    QTableWidget *mpList;
};

#endif
