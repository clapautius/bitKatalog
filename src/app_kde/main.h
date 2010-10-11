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
#if !defined(MAIN_H)
#define MAIN_H

#include <qpixmap.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kglobal.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

#include <string>
#include <vector> // :tmp:

#include "bitkatalog.h"
#include "bitkatalogview.h"
#include "xfclib.h"
#include "xfcapp.h"


extern KSharedConfigPtr gpConfig;

extern QPixmap *gpDiskPixmap;
extern QPixmap *gpDirPixmap;
extern QPixmap *gpFilePixmap;

extern KApplication *gpApplication;
extern bitKatalogView *gpView;
extern bitKatalog *gpMainWindow;

extern int gCatalogState;
// 0 - not loaded
// 1 - modified
// 2 - not modified

extern XfcLogger gkLog;

extern std::string gHomeDir;

void msgWarn(std::string, std::string="", std::string="");
void msgInfo(std::string, std::string="", std::string="");
void msgDebug(std::string, std::string="", std::string="", std::string="");

//extern std::string gLastDir;

//void runningForTheFirstTime();

// :tmp:
/// temporary functions used during the transition from std::string to QString
std::vector<std::string> vectWQStringToVectWstring(std::vector<QString>);
std::vector<QString> vectWstringToVectWQString(std::vector<std::string>);

bool contains(std::vector<QString> vect, QString elt);

const char* qstr2cchar(const QString &str);
std::string qstr2str(const QString &str);
QString str2qstr(const std::string &s);

QString cfgGetSearchStartPath();
QString cfgGetDefaultActionForSearch();

#endif
