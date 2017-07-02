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
#include <QHeaderView>

#include "main.h"
#include "outputwindow.h"

using namespace std;

OutputWindow::OutputWindow() : KPageDialog()
{
    setCaption("Result");
    setButtons(KDialog::Close);
    setModal(true);
    layout();
}

OutputWindow::~OutputWindow() {}

void OutputWindow::layout()
{
    resize(800, 450);
    KVBox *pBox1 = new KVBox();
    KPageWidgetItem *pPage1 = addPage(pBox1, QString("Results"));
    pPage1->setHeader(QString("Results"));

    mpEditArea = new KTextEdit(pBox1);
    mpEditArea->setReadOnly(true);
}

void OutputWindow::addText(std::string lText)
{
    mpEditArea->insertPlainText(str2qstr(lText));
    mpEditArea->insertPlainText("\n");
}

DiffOutputWindow::DiffOutputWindow(int rows) : KPageDialog(), mRows(rows)
{
    setCaption("Result");
    setButtons(KDialog::Close);
    setModal(true);
    mCurrentRow = 0;
    layout();
    gkLog << xfcInfo << "Created new diff output window. No. of rows=" << rows << eol;
}

DiffOutputWindow::~DiffOutputWindow() {}

void DiffOutputWindow::layout()
{
    resize(800, 450);
    KVBox *pBox1 = new KVBox();
    KPageWidgetItem *pPage1 = addPage(pBox1, QString("Results"));
    pPage1->setHeader(QString("Results"));
    mpList = new QTableWidget(mRows, 3, pBox1);
    mpList->setShowGrid(false);
    QStringList headers;
    headers << "Catalog"
            << ""
            << "Disk";
    mpList->setHorizontalHeaderLabels(headers);
    mpList->setStyleSheet("QTableWidgetItem { border-style: none solid; }");
}

void DiffOutputWindow::addText(string s1, string s2, string s3, unsigned int lines)
{
    gkLog << xfcDebug << "Adding text to current row: " << s1 << ", " << s2 << ", " << s3
          << eol;
    gkLog << "Current row is " << mCurrentRow << eol;
    string verticalHeader;
    if (lines > 1) {
        for (unsigned int i = 1; i < lines; i++) verticalHeader += "\n";
    }
    mpList->setVerticalHeaderItem(mCurrentRow,
                                  new QTableWidgetItem(verticalHeader.c_str()));
    QColor background;
    if (mCurrentRow % 2)
        background = QApplication::palette().color(QPalette::AlternateBase);
    else
        background = QApplication::palette().color(QPalette::Base);
    QTableWidgetItem *pItem = new QTableWidgetItem(str2qstr(s1));
    pItem->setFlags(Qt::ItemIsEnabled);
    pItem->setBackgroundColor(background);
    mpList->setItem(mCurrentRow, 0, pItem);
    pItem = new QTableWidgetItem(str2qstr(s2));
    pItem->setFlags(Qt::ItemIsEnabled);
    pItem->setBackgroundColor(background);
    mpList->setItem(mCurrentRow, 1, pItem);
    pItem = new QTableWidgetItem(str2qstr(s3));
    pItem->setFlags(Qt::ItemIsEnabled);
    pItem->setBackgroundColor(background);
    mpList->setItem(mCurrentRow, 2, pItem);
    mCurrentRow++;
}

void DiffOutputWindow::finishedText()
{
    mpList->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
}
