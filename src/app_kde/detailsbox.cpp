/***************************************************************************
 *   Copyright (C) 2005 by Tudor Marian Pristavu                           *
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
#include "detailsbox.h"

#include <klineedit.h>
#include <qlayout.h>
#include <qpainter.h>
#include <QHeaderView>
#include <string>
#include <vector>

#include <kinputdialog.h>
#include <kmessagebox.h>

#include "bitkatalogview.h"
#include "labelsbox.h"
#include "main.h"
#include "misc.h"
#include "xfcEntity.h"

#if defined(XFC_DEBUG)
#include <iostream>
using std::cout;
using std::endl;
#endif

using std::string;
using std::vector;
using std::map;

DetailsBox::DetailsBox(Xfc *lpCatalog, std::string lCompletePath, XfcEntity *lpXmlItem,
                      XmlEntityItem *pListItem, const vector<QString> &rAllLabels)
    : KPageDialog(), mrAllLabels(rAllLabels)

{
    setFaceType(KPageDialog::Tabbed);
    setCaption(QString("Details"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setModal(true);
    mCompletePath = lCompletePath;
    mpXfcItem = lpXmlItem;
    mpCatalog = lpCatalog;
    mpViewItem = pListItem;
    mCatalogWasModified = false;
    mLabelsWereModified = false;
    layout();
    connectButtons();
}

DetailsBox::~DetailsBox() {}

void DetailsBox::editLabels()
{
#if defined(XFC_DEBUG)
    cout << ":debug:" << __FUNCTION__ << endl;
#endif
    vector<QString> allCurrentLabels;  // all catalog labels + current labels
    // we need this to avoid bugs when editing an element multiple times
    // (without hitting 'OK').
    allCurrentLabels = mrAllLabels;
    for (uint i = 0; i < mCurrentLabels.size(); i++) {
        if (!contains(allCurrentLabels, mCurrentLabels[i])) {
            allCurrentLabels.push_back(mCurrentLabels[i]);
        }
    }
    LabelsBox *pLabelsBox = new LabelsBox(allCurrentLabels, mCurrentLabels);
    if (QDialog::Accepted == pLabelsBox->exec()) {
        mCurrentLabels = pLabelsBox->getSelectedLabels();
        mpLabels->clear();
        for (uint i = 0; i < mCurrentLabels.size(); i++) {
            QStringList list;
            list.push_back("");
            list.push_back(mCurrentLabels[i]);
            QTreeWidgetItem *pLabel = new QTreeWidgetItem(list);
            pLabel->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
            mpLabels->addTopLevelItem(pLabel);
        }
    }
}

void DetailsBox::connectButtons()
{
    connect(mpEditLabelsButton, SIGNAL(clicked()), this, SLOT(editLabels()));
}

void DetailsBox::layout()
{
    std::string str;
    QFont lFont;
    map<string, string> details;
    QSize lSize;
    QFontMetrics *lpFontMetrics;

    resize(500, 400);

    KVBox *pBox1 = new KVBox();
    KPageWidgetItem *pPage1 = addPage(pBox1, QString("General"));
    pPage1->setHeader(QString("General"));

    mpName = new QLabel(str2qstr(mpXfcItem->getName()), pBox1);
    mpName->setAlignment(Qt::AlignHCenter);
    mpName->setTextInteractionFlags(Qt::TextSelectableByMouse);
    lFont = mpName->font();
    lFont.setBold(true);
    mpName->setFont(lFont);

    KHBox *pDescriptionBox = new KHBox(pBox1);
    mpTmpLabel1 = new QLabel("Description: ", pDescriptionBox);
    mpDescriptionEdit = new KLineEdit(pDescriptionBox);
    details = mpXfcItem->getDetails();
    mpDescriptionEdit->setText(str2qstr(details["description"]));

    KHBox *pCommentBox = new KHBox(pBox1);
    mpTmpLabel1 = new QLabel("Comment: ", pCommentBox);
    mpCommentEdit = new KLineEdit(pCommentBox);
    string comment = mpXfcItem->getComment();
    mpCommentEdit->setText(str2qstr(comment));

    // labels group box
    mpLabelGroup = new Q3VGroupBox("Labels", pBox1);

    KHBox *pLabelsBox = new KHBox(mpLabelGroup);
    mpLabels = new QTreeWidget(pLabelsBox);
    mpLabels->setColumnCount(2);
    mpLabels->header()->hide();
    mpLabels->setSelectionMode(QAbstractItemView::NoSelection);
    mpLabels->sortByColumn(1, Qt::AscendingOrder);
    mpLabels->setSortingEnabled(true);
    mpLabels->setColumnHidden(0, true);
    mpLabels->setAlternatingRowColors(true);
    lSize = mpLabels->size();
    lFont = mpLabels->font();
    lpFontMetrics = new QFontMetrics(lFont);
    mpLabels->resize(lSize.width(), 2 * lpFontMetrics->height());
    delete lpFontMetrics;

    mCurrentLabels = vectWstringToVectWQString(mpXfcItem->getLabels());
    for (uint i = 0; i < mCurrentLabels.size(); i++) {
        QStringList list;
        list.push_back("");
        list.push_back(mCurrentLabels[i]);
        QTreeWidgetItem *pLabel = new QTreeWidgetItem(list);
        pLabel->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
        mpLabels->addTopLevelItem(pLabel);
    }

    KHBox *pLabelButtons = new KHBox(mpLabelGroup);
    mpEditLabelsButton = new QPushButton("Edit labels", pLabelButtons);

    // end labels group box

    // page2
    KPageWidgetItem *pPage2 = NULL;
    KVBox *pBox2 = NULL;
    if (mpXfcItem->isFileOrDir()) {
        int fileType;
        fileType = mpXfcItem->getTypeOfFile();
        if (0 == fileType)  // not good
            ;
        else if (1 == fileType) {
            pBox2 = new KVBox();
            pPage2 = addPage(pBox2, QString("File details"));
            pPage2->setHeader(QString("File details"));
            KVBox *pLayoutBox2 = new KVBox(pBox2);
            pLayoutBox2->setSpacing(20);

            // size
            str = sizeToHumanReadableSize(details["size"]);
            addDetailLabels(pLayoutBox2, "Size   :", str.c_str());

            // sha1
            if (details[SHA1LABEL].empty())
                str = "-";
            else {
                str = details[SHA1LABEL].substr(0, details[SHA1LABEL].size() / 2);
                str += "\n";
                str += details[SHA1LABEL].substr(details[SHA1LABEL].size() / 2);
            }
            addDetailLabels(pLayoutBox2, "SHA1   :", str.c_str());

            // sha256
            if (details[SHA256LABEL].empty())
                str = "-";
            else {
                str = details[SHA256LABEL].substr(0, details[SHA256LABEL].size() / 2);
                str += "\n";
                str += details[SHA256LABEL].substr(details[SHA256LABEL].size() / 2);
            }
            addDetailLabels(pLayoutBox2, "SHA256 :", str.c_str());

            pLayoutBox2->layout()->addItem(
                new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
        } else if (2 == fileType) {
            pBox2 = new KVBox();
            pPage2 = addPage(pBox2, QString("Directory details"));
            pPage2->setHeader(QString("Directory details"));
        }
    } else if (mpXfcItem->isDisk()) {  // disk
        pBox2 = new KVBox();
        pPage2 = addPage(pBox2, QString("Disk details"));
        pPage2->setHeader(QString("Disk details"));
        mpCdateBox = new KHBox(pBox2);
        new QLabel("Creation date: ", mpCdateBox);
        mpCdateEdit = new KLineEdit(mpCdateBox);
        mpCdateEdit->setText(details["cdate"].c_str());

        KHBox *pStorageDevBox = new KHBox(pBox2);
        mpTmpLabel1 = new QLabel("Storage devices: ", pStorageDevBox);
        mpStorageDevEdit = new KLineEdit(pStorageDevBox);
        string storage_dev = mpXfcItem->getStorageDev();
        mpStorageDevEdit->setText(str2qstr(storage_dev));

        pBox2->layout()->addItem(
            new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    }
}

void DetailsBox::accept()
{
    QString qstr;
    // std::vector<std::string> lDetails;
    map<string, string> details;
    bool labels_modified = false;
    bool item_modified = false;

    details = mpXfcItem->getDetails();

    // update description
    qstr = mpDescriptionEdit->text();
    if (qstr != str2qstr(details["description"])) {
        details["description"] = qstr.toStdString();
        mpCatalog->setDescriptionOf(mCompletePath, qstr2cchar(qstr));
        item_modified = true;
    }

    // check labels and update if modified
    vector<QString> labels = vectWstringToVectWQString(mpXfcItem->getLabels());
    if (mCurrentLabels.size() != labels.size())
        labels_modified = true;
    else {
        // :fixme: optimize - use set or something else
        for (uint i = 0; i < mCurrentLabels.size() && !labels_modified; i++) {
            if (!contains(labels, mCurrentLabels[i])) {
                labels_modified = true;
            }
        }
        for (uint i = 0; i < labels.size() && !labels_modified; i++) {
            if (!contains(mCurrentLabels, labels[i])) {
                labels_modified = true;
            }
        }
    }
    if (labels_modified) {
        // remove all labels
        for (unsigned int i = 0; i < labels.size(); i++)
            mpCatalog->removeLabelFrom(mCompletePath, qstr2cchar(labels[i]));
        // add new labels
        for (unsigned int i = 0; i < mCurrentLabels.size(); i++) {
            gkLog << xfcDebug << "adding new label to element: ";
            gkLog << qstr2cchar(mCurrentLabels[i]) << eol;
            mpCatalog->addLabelTo(mCompletePath, qstr2cchar(mCurrentLabels[i]));
        }
        item_modified = true;
        mLabelsWereModified = true;
        string labelsString = mpXfcItem->getLabelsAsString();
        details["labels"] = labelsString;
    }

    // update comment
    qstr = mpCommentEdit->text();
    string comment = mpXfcItem->getComment();
    if (qstr != str2qstr(comment)) {
        mpXfcItem->setComment(qstr2cchar(qstr));
        item_modified = true;
    }

    if (mpXfcItem->isDisk()) {
        try {
            // update cdate
            qstr = mpCdateEdit->text();
            if (qstr != details["cdate"].c_str()) {
                mpCatalog->setCDate(mCompletePath, qstr2cchar(qstr));
                item_modified = true;
            }

            // update storage_dev
            qstr = mpStorageDevEdit->text();
            string storage_dev = mpXfcItem->getStorageDev();
            if (qstr != str2qstr(storage_dev)) {
                mpXfcItem->setStorageDev(qstr2cchar(qstr));
                item_modified = true;
            }
        } catch (...) {
            KMessageBox::error(this, "Error updating disk");
        }
    }

    if (item_modified) {
        mCatalogWasModified = true;
        mpViewItem->updateVisualTexts(*mpXfcItem, details);
    }
    KPageDialog::accept();
}

void DetailsBox::reject() { KPageDialog::reject(); }

bool DetailsBox::catalogWasModified() const { return mCatalogWasModified; }

bool DetailsBox::labelsWereModified() const { return mLabelsWereModified; }

void DetailsBox::addDetailLabels(QFrame *pFrame, const char *pText1, const char *pText2)
{
    KHBox *pBox = new KHBox(pFrame);
    pBox->setSpacing(10);
    pBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QLabel *pLabel1 = new QLabel(pText1, pBox);
    pLabel1->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    pLabel1->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QLabel *pLabel2 = new QLabel(pText2, pBox);
    pLabel2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    pLabel2->setTextInteractionFlags(Qt::TextSelectableByMouse);
}
