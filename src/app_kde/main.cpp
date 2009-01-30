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
#include <iostream>

#include "bitkatalog.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <ksharedconfig.h>

#include <sstream>
#include <fstream>

#include "main.h"
#include "fs.h"
#include "xfc.h"

KSharedConfigPtr gpConfig;

QPixmap *gpDiskPixmap=NULL;
QPixmap *gpDirPixmap=NULL;
QPixmap *gpFilePixmap=NULL;

KApplication *gpApplication;
bitKatalogView *gpView=NULL;
bitKatalog *gpMainWindow=NULL;

int gCatalogState=0;

static const char version[] = XFCAPP_VERSION;

// :release:
static int gVerboseLevel=0;


void
msgWarn(std::string s, std::string s2, std::string s3)
{
    if (gVerboseLevel>=1) {
        std::cout<<s;
        if (!s2.empty())
            std::cout<<s2;
        if (!s3.empty())
            std::cout<<s3;
        std::cout<<std::endl;
    }
}


void
msgInfo(std::string s, std::string s2, std::string s3)
{
    if (gVerboseLevel>=2) {
        std::cout<<s;
        if (!s2.empty())
            std::cout<<s2;
        if (!s3.empty())
            std::cout<<s3;
        std::cout<<std::endl;
    }
}


void msgDebug(std::string s, std::string s2, std::string s3)
{
    if (gVerboseLevel>=3) {
        std::cout<<s;
        if (!s2.empty())
            std::cout<<s2;
        if (!s3.empty())
            std::cout<<s3;
        std::cout<<std::endl;
    }
}


void startUp();


int main(int argc, char **argv)
{
    std::string otherText;
    KAboutData about(QByteArray("bitKatalog"), QByteArray("bitKatalog"), ki18n("bitKatalog"), QByteArray(version), ki18n("A KDE file & disk catalog that uses plain-text XML to store catalog information."),  KAboutData::License_GPL);

    // :fixme: - kde4
    //KAboutData::License_GPL, "(C) 2009 Tudor Pristavu", 0, 0, "clapautiusAtGmailDotCom");

    // :fixme: - kde4
    //about.addAuthor( "Tudor Pristavu", 0, "tudor.pristavu@gmail.com" );
    //otherText="Using libaxfc version: ";
    //otherText+=Xfc::getVersionString();
    //about.setOtherText(otherText.c_str());

    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineOptions options;
    options.add("+[URL]", ki18n("Document to open"));
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

    // try to find a suitable window icon
    KIconLoader *pIconLoader=KIconLoader::global();
    QIcon windowIcon=pIconLoader->loadIconSet("bitKatalog", KIconLoader::NoGroup, 0, true);
    if (windowIcon.isNull()) {
        msgInfo("Couldn't find bitKatalog icon");
        //delete pWindowIcon;
        windowIcon=pIconLoader->loadIconSet("media-optical", KIconLoader::NoGroup, 0, false);
        if (windowIcon.isNull())
            msgWarn("Couldn't find media-optical icon");
    }
    app.setWindowIcon(windowIcon);

    startUp();

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


void startUp()
{
    KIconLoader *pIconLoader=KIconLoader::global();
    gpDiskPixmap=new QPixmap(pIconLoader->loadIcon("media-optical", KIconLoader::Small));
    gpDirPixmap=new QPixmap(pIconLoader->loadIcon("folder-green", KIconLoader::Small));
    gpFilePixmap=new QPixmap(pIconLoader->loadIcon("text-plain", KIconLoader::Small));

    xmlIndentTreeOutput=1;
    xmlKeepBlanksDefault(0);

    //gpConfig=gpApplication->sessionConfig();
    gpConfig=KGlobal::config();
    //gpConfig->setReadOnly(false);
    //runningForTheFirstTime();
}


/*
void runningForTheFirstTime()
{
    std::ostringstream lOut;
    std::ofstream lFout;
    if(!fileExists(CONFIG_FILE))
    {
        lFout.open(CONFIG_FILE);
        if(!lFout.good())
        {
            KMessageBox::error(NULL, "Cannot create config file");
        }
        else
        {
            lFout.close();
            KMessageBox::information(NULL, "Default config file created");
        }
    }
        
    
    gLastDir=gpConfig->readEntry("lastDir", ".").ascii();
}
*/
