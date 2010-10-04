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

#include <qpainter.h>
#include <qlayout.h>
#include <vector>
#include <string>
#include <klineedit.h>

#include <kmessagebox.h>
#include <kinputdialog.h>

#if defined(MTP_DEBUG)
  #include <iostream>

  using namespace std;
#endif

LabelsBox::LabelsBox()
    : KPageDialog()
{
    // :fixme: - do something
} 


LabelsBox::LabelsBox(std::vector<std::string> allLabels, std::vector<std::string> selectedLabels)
    : KPageDialog()
{
    setCaption(QString("Labels"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setModal(true);

    mSelectedLabels=selectedLabels;
    layout(allLabels);
    connectButtons();
}


LabelsBox::~LabelsBox()
{
}




void
LabelsBox::connectButtons()
{
    connect(mpAddNewButton, SIGNAL(clicked()), this, SLOT(addNewLabel()));
} 


void
LabelsBox::addLabelInList(QString labelText, bool checked)
{
    QTreeWidgetItem *pItem=new QTreeWidgetItem(QStringList(labelText));
    pItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    if (checked) {
        pItem->setCheckState(0, Qt::Checked);
    }
    else {
        pItem->setCheckState(0, Qt::Unchecked);
    }
    pItem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
    mpLabels->addTopLevelItem(pItem);
}


void LabelsBox::layout(std::vector<std::string> allLabels)
{
    std::string str;

    resize(500,250);

    KVBox *pBox1= new KVBox();
    KPageWidgetItem *pPage1=addPage(pBox1, QString("Labels"));
    pPage1->setHeader(QString("Labels"));

    mpLabels=new QTreeWidget(pBox1);
    mpLabels->setColumnCount(1);
    mpLabels->setHeaderLabels(QStringList("Available labels"));
    for (unsigned int i=0; i<allLabels.size(); i++) {
        addLabelInList(allLabels[i].c_str(), contains(mSelectedLabels, allLabels[i]));
    }
    mpLabels->sortByColumn(0, Qt::AscendingOrder);
    mpLabels->setSortingEnabled(true);
    
    KHBox *pBox2=new KHBox(pBox1);
    mpLabelEdit=new KLineEdit(pBox2);
    mpAddNewButton=new QPushButton("Add new label", pBox2);
}


void LabelsBox::accept()
{
    QList<QTreeWidgetItem*> list;

    mSelectedLabels.clear();
    
    list=mpLabels->invisibleRootItem()->takeChildren();
    for (int i=0; i<list.size(); i++) {
        if (Qt::Checked == list[i]->checkState(0)) {
            mSelectedLabels.push_back(list[i]->text(0).toStdString());
        }
    }

    KPageDialog::accept();
} 


void LabelsBox::reject()
{
    KPageDialog::reject();
} 


void
LabelsBox::addNewLabel()
{
    if (mpLabelEdit->text().isEmpty()) {
        KMessageBox::information(this, "Nothing to add");
    }
    else {
        addLabelInList(mpLabelEdit->text(), true);
        mpLabelEdit->clear();
    }
}


bool
LabelsBox::contains(std::vector<std::string> vect, std::string elt)
{
    for (unsigned int i=0; i<vect.size(); i++)
        if (vect[i]==elt)
            return true;
    return false;
}

