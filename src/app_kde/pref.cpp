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

#include "pref.h"

#include <Qt/qlabel.h>
#include <Qt/qlayout.h>
#include <kfiledialog.h>
#include <khbox.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kvbox.h>
#include <QGroupBox>

#include "main.h"

bitKatalogPreferences::bitKatalogPreferences() : KPageDialog()
/*    : KDialog(TreeList, i18n("bitKatalog Preferences"),
      Help|Default|Ok|Apply|Cancel, Ok) */

{
    setButtons(KDialog::Ok | KDialog::Cancel);
    setCaption(i18n("bitKatalog Preferences"));

    setModal(true);
    layout();
}

void bitKatalogPreferences::layout()
{
    KVBox *pBox1 = new KVBox();
    QString searchPath = cfgGetSearchStartPath();

    KPageWidgetItem *pPage1 = addPage(pBox1, QString("Search"));
    pPage1->setHeader(QString("General"));

    KHBox *pSearchPathBox = new KHBox(pBox1);
    new QLabel("Search start path: ", pSearchPathBox);
    mpSearchPathEdit = new KLineEdit(pSearchPathBox);
    mpSearchPathEdit->setText(searchPath);
    mpSearchPathEdit->setReadOnly(true);
    KPushButton *pButton1 = new KPushButton("Set path", pSearchPathBox);
    connect(pButton1, SIGNAL(clicked()), this, SLOT(setSearchPath()));

    QGroupBox *pBoxWithFrame = new QGroupBox("Default action for local items", pBox1);
    mpButton1 = new QRadioButton("Launch default application", pBoxWithFrame);
    mpButton2 = new QRadioButton("Open parent folder", pBoxWithFrame);
    QVBoxLayout *pVBox = new QVBoxLayout;
    pVBox->addWidget(mpButton1);
    pVBox->addWidget(mpButton2);
    pBoxWithFrame->setLayout(pVBox);
    if (cfgGetDefaultActionForSearch() == "launch") {
        mpButton1->setChecked(true);
    } else {
        mpButton2->setChecked(true);
    }
}

void bitKatalogPreferences::accept()
{
    gpConfig->group("").writeEntry("SearchPath", mpSearchPathEdit->text());
    if (mpButton1->isChecked()) {
        gpConfig->group("").writeEntry("SearchDefaultAction", "launch");
    } else {
        gpConfig->group("").writeEntry("SearchDefaultAction", "openFolder");
    }
    gpConfig->sync();

    KPageDialog::accept();
}

void bitKatalogPreferences::reject() { KPageDialog::reject(); }

void bitKatalogPreferences::setSearchPath()
{
    QString dir = KFileDialog::getExistingDirectory(KUrl(), this, i18n("Search path"));
    if (dir != "") {
        mpSearchPathEdit->setText(dir);
    }
}
