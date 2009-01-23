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

//std::string gLastDir;

// :ver:
static const char version[] = "0.4.0-alpha";



void startUp();


int main(int argc, char **argv)
{
    std::string otherText;
    KAboutData about(QByteArray("axfck"), QByteArray(""), ki18n("axfck"), QByteArray(version), ki18n("A beautiful KDE application"));

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
    gpApplication=&app;

    startUp();

    // see if we are starting with session management
    if (app.isSessionRestored())
    {
        RESTORE(bitKatalog);
    }
    else
    {
        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
        if (args->count() == 0)
        {
            bitKatalog *widget = new bitKatalog;
            widget->show();
            gpMainWindow=widget;
        }
        else if(args->count() == 1)
        {
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
    gpDiskPixmap=new QPixmap(ICON_DISK);
    gpDirPixmap=new QPixmap(ICON_DIR);
    gpFilePixmap=new QPixmap(ICON_DIR);
    
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
