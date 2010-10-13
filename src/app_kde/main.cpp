/***************************************************************************
 *   Copyright (C) 2009 by Tudor Marian Pristavu                           *
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
#include <iostream>

#include "bitkatalog.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <sstream>
#include <fstream>

#include <vector>
#include <string>
using std::vector;
using std::string;

#include "main.h"
#include "fs.h"
#include "xfc.h"

KSharedConfigPtr gpConfig;

KIcon *gpDiskIcon=NULL;
KIcon *gpDirIcon=NULL;
KIcon *gpFileIcon=NULL;

KApplication *gpApplication;
bitKatalogView *gpView=NULL;
bitKatalog *gpMainWindow=NULL;

int gCatalogState=0;

static const char version[] = XFCAPP_VERSION;

XfcLogger gkLog;

string gHomeDir;


void
msgWarn(std::string s, std::string s2, std::string s3)
{
    gkLog<<xfcWarn<<s;
    if (!s2.empty())
        gkLog<<s2;
    if (!s3.empty())
        gkLog<<s3;
    gkLog<<eol;
}


void
msgInfo(std::string s, std::string s2, std::string s3)
{
    gkLog<<xfcWarn<<s;
    if (!s2.empty())
        gkLog<<s2;
    if (!s3.empty())
        gkLog<<s3;
    gkLog<<eol;
}


void msgDebug(std::string s, std::string s2, std::string s3, std::string s4)
{
    gkLog<<xfcWarn<<s;
    if (!s2.empty())
        gkLog<<s2;
    if (!s3.empty())
        gkLog<<s3;
    if (!s4.empty())
        gkLog<<s4;
    gkLog<<eol;
}


static void
startUp(KCmdLineArgs *);


int main(int argc, char **argv)
{
    std::string otherText;
    KAboutData about(
        QByteArray("bitKatalog"), QByteArray("bitKatalog"), ki18n("bitKatalog"),
        QByteArray(version),
        ki18n("A KDE file & disk catalog that uses plain-text XML to store catalog information."),
        KAboutData::License_GPL_V2, ki18n("(C) 2010 Tudor Marian Pristavu"));

    about.addAuthor(ki18n("Tudor Marian Pristavu"), ki18n(""), QByteArray("clapautiusAtGmailDotCom") );
    about.setProgramIconName("media-optical");
    
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineOptions options;
    options.add("verbose-level <argument>", ki18n("Verbose level (0-3)"), "0");
    options.add("xfclib-verbose-level <argument>", ki18n("Verbose level for xfclib (0-3)"), "0");
    options.add("+[URL]", ki18n("Document to open"));
    KCmdLineArgs::addCmdLineOptions(options);
    KCmdLineArgs *pArgs=KCmdLineArgs::parsedArgs();
    KApplication app;

    startUp(pArgs);

    // try to find a suitable window icon
    QIcon windowIcon=KIconLoader::global()->loadIconSet(
        "bitkatalog", KIconLoader::Small, 0, true);
    if (windowIcon.isNull()) {
        gkLog<<xfcInfo<<"Couldn't find bitKatalog icon"<<eol;
        windowIcon=KIconLoader::global()->loadIconSet(
            "media-optical", KIconLoader::Small, 0, false);
        // cannot return null
    }
    app.setWindowIcon(windowIcon);

    // see if we are starting with session management
    if (app.isSessionRestored()) {
        RESTORE(bitKatalog);
    }
    else {
        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
        if (args->count() == 0) {
            bitKatalog *widget = new bitKatalog;
            widget->setWindowIcon(windowIcon);
            widget->show();
            gpMainWindow=widget;
        }
        else if (args->count() == 1) {
            bitKatalog *widget = new bitKatalog;
            widget->show();
            gpMainWindow=widget;
            widget->load(args->url(0));
        }
        args->clear();
    }
    return app.exec();
}


void startUp(KCmdLineArgs *pArgs)
{
    gpDiskIcon=new KIcon("media-optical");
    gpDirIcon=new KIcon("folder-green");
    gpFileIcon=new KIcon("text-plain");
    
    xmlIndentTreeOutput=1;
    xmlKeepBlanksDefault(0);

    gkLog.setVerboseLevel(pArgs->getOption("verbose-level").toInt());
    gLog.setVerboseLevel(pArgs->getOption("xfclib-verbose-level").toInt());

    gkLog<<xfcInfo<<"kde app verbose level: "<<gkLog.getVerboseLevel()<<eol;
    gkLog<<xfcInfo<<"xfc lib verbose level: "<<gLog.getVerboseLevel()<<eol;
    
    //gpConfig=gpApplication->sessionConfig();
    gpConfig=KGlobal::config();
    //gpConfig->setReadOnly(false);
    //runningForTheFirstTime();

    gHomeDir=getenv("HOME");
}


std::vector<std::string>
vectWQStringToVectWstring(std::vector<QString> v)
{
    vector<string> ret;
    for (uint i=0; i<v.size(); i++)
        ret.push_back(qstr2cchar(v[i]));
    return ret;
}

std::vector<QString> vectWstringToVectWQString(std::vector<std::string> v)
{
    vector<QString> ret;
    for (uint i=0; i<v.size(); i++)
        ret.push_back(str2qstr(v[i]));
    return ret;
}


bool
contains(vector<QString> vect, QString elt)
{
    for (uint i=0; i<vect.size(); i++)
        if (vect[i]==elt)
            return true;
    return false;
}


/**
 * Converts from QString to const char*, keeps utf-8 encoding.
 **/
const char*
qstr2cchar(const QString &str)
{
    return str.toUtf8().constData();
}


/**
 * Converts from QString to string, keeps utf-8 encoding.
 **/
string
qstr2str(const QString &str)
{
    string s(str.toUtf8().constData());
    return s;
}


/**
 * Converts from std::string to QString, keeps utf-8 encoding.
 **/
QString
str2qstr(const std::string &s)
{
    return QString::fromUtf8(s.c_str());
}


QString
cfgGetSearchStartPath()
{
    return gpConfig->group("").readEntry("SearchPath", gHomeDir.c_str());
}


QString
cfgGetDefaultActionForSearch()
{
    return gpConfig->group("").readEntry("SearchDefaultAction", "launch");
}
