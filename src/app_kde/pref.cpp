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

#include <klocale.h>

#include <Qt/qlayout.h>
#include <Qt/qlabel.h>

bitKatalogPreferences::bitKatalogPreferences()
    : KDialog()
/*    : KDialog(TreeList, i18n("bitKatalog Preferences"),
      Help|Default|Ok|Apply|Cancel, Ok) */

{
    setButtons(KDialog::Help | KDialog::Default | KDialog::Ok | KDialog::Apply | KDialog::Cancel);
    setCaption(i18n("bitKatalog Preferences"));
    
    // this is the base class for your preferences dialog.  it is now
    // a Treelist dialog.. but there are a number of other
    // possibilities (including Tab, Swallow, and just Plain)
    // :todo: kde4
    /*
    KHbox *page=new KHBox();
    KPageWidgetItem *item = addPage(page, i18n("First Page"), i18n("Page One Options"));
    m_pageOne = new bitKatalogPrefPageOne(frame);

    frame = addPage(i18n("Second Page"), i18n("Page Two Options"));
    m_pageTwo = new bitKatalogPrefPageTwo(frame);
    */
}


bitKatalogPrefPageOne::bitKatalogPrefPageOne(QWidget *parent)
    : QFrame(parent)
{
    new QLabel(i18n("Add something here"), this);
}


bitKatalogPrefPageTwo::bitKatalogPrefPageTwo(QWidget *parent)
    : QFrame(parent)
{
    new QLabel(i18n("Add something here"), this);
}
