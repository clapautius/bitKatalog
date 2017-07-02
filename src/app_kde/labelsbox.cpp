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
#include "labelsbox.h"

#include <klineedit.h>
#include <qlayout.h>
#include <qpainter.h>
#include <vector>

#include <kinputdialog.h>
#include <kmessagebox.h>

#include "main.h"
#include "misc.h"

using std::vector;

LabelsBox::LabelsBox() : KPageDialog()
{
    // :fixme: - do something
}

LabelsBox::LabelsBox(vector<QString> allLabels, vector<QString> selectedLabels,
                     bool addEnabled)
    : KPageDialog(), mAddEnabled(addEnabled)
{
    setCaption(QString("Labels"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setModal(true);

    mSelectedLabels = selectedLabels;
    layout(allLabels);
    connectButtons();
}

LabelsBox::~LabelsBox() {}

void LabelsBox::connectButtons()
{
    if (mAddEnabled) {
        connect(mpAddNewButton, SIGNAL(clicked()), this, SLOT(addNewLabel()));
    }
}

void LabelsBox::addLabelInList(QString labelText, bool checked)
{
    QTreeWidgetItem *pItem = new QTreeWidgetItem(QStringList(labelText));
    pItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    if (checked) {
        pItem->setCheckState(0, Qt::Checked);
    } else {
        pItem->setCheckState(0, Qt::Unchecked);
    }
    pItem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
    mpLabels->addTopLevelItem(pItem);
}

void LabelsBox::layout(vector<QString> allLabels)
{
    resize(500, 500);
    KVBox *pBox1 = new KVBox();
    KPageWidgetItem *pPage1 = addPage(pBox1, QString("Labels"));
    pPage1->setHeader(QString("Labels"));

    mpLabels = new QTreeWidget(pBox1);
    mpLabels->setColumnCount(1);
    mpLabels->setAlternatingRowColors(true);
    mpLabels->setHeaderLabels(QStringList("Available labels"));
    for (unsigned int i = 0; i < allLabels.size(); i++) {
        addLabelInList(allLabels[i], contains(mSelectedLabels, allLabels[i]));
    }
    mpLabels->sortByColumn(0, Qt::AscendingOrder);
    mpLabels->setSortingEnabled(true);

    if (mAddEnabled) {
        KHBox *pBox2 = new KHBox(pBox1);
        mpLabelEdit = new KLineEdit(pBox2);
        mpAddNewButton = new QPushButton("Add new label", pBox2);
    }
}

void LabelsBox::accept()
{
    QList<QTreeWidgetItem *> list;

    mSelectedLabels.clear();

    list = mpLabels->invisibleRootItem()->takeChildren();
    for (int i = 0; i < list.size(); i++) {
        if (Qt::Checked == list[i]->checkState(0)) {
            mSelectedLabels.push_back(list[i]->text(0));
        }
    }

    KPageDialog::accept();
}

void LabelsBox::reject() { KPageDialog::reject(); }

void LabelsBox::addNewLabel()
{
    if (mpLabelEdit->text().isEmpty()) {
        KMessageBox::information(this, "Nothing to add");
    } else {
        addLabelInList(mpLabelEdit->text(), true);
        mpLabelEdit->clear();
    }
}

vector<QString> LabelsBox::getSelectedLabels() const { return mSelectedLabels; }
