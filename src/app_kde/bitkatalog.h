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


#ifndef _BITKATALOG_H_
#define _BITKATALOG_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QtGui/QDragEnterEvent>

#include <kapplication.h>
#include <kxmlguiwindow.h>
#include <kstandardaction.h>
#include <ktoolbar.h>
#include <ktoggleaction.h>

#include "bitkatalogview.h"

//class KPrinter;
class Kurl;

/**
 * This class serves as the main window for bitKatalog.  It handles the
 * menus, toolbars, and status bars.
 *
 */
class bitKatalog : public KXmlGuiWindow
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    bitKatalog();

    /**
     * Default Destructor
     */
    virtual ~bitKatalog();

    /**
     * Use this method to load whatever file/URL you have
     */
    void load(const KUrl& url);
    
    bitKatalogView* getView() const;
    
    void setCatalogPath(std::string);
    void setCatalogPath(QString);

    std::string getCatalogPath() const;

    void updateTitle(bool modified=false);
    
protected:
    /**
     * Overridden virtuals for Qt drag 'n drop (XDND)
     */
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);

    virtual bool queryClose();

protected:
    /**
     * This function is called when it is time for the app to save its
     * properties for session management purposes.
     */
    virtual void saveProperties(KConfigGroup&);

    /**
     * This function is called when this app is restored.  The KConfig
     * object points to the session management config file that was saved
     * with @ref saveProperties
     */
    virtual void readProperties(const KConfigGroup&);


private slots:
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void filePrint();
    /*void optionsShowToolbar();*/
    void optionsShowStatusbar();
    void optionsConfigureKeys();
    void optionsConfigureToolbars();
    void optionsPreferences();
    void newToolbarConfig();

    void search();
    
    void addDisk();
    
    void changeStatusbar(const QString& text);
    void changeCaption(const QString& text);

private:

    void setupAccel();
    void setupActions();
    
    void createNumberedBackup(std::string lFile)
            throw (std::string);
    
private:

    bitKatalogView *m_view;

    KMenu *mpFileMenu, *mpEditMenu;
    //KPrinter   *m_printer;

    /*KToggleAction *m_toolbarAction;*/
    KToggleAction *m_statusbarAction;
    
    std::string mCatalogPath;
};

#endif // _BITKATALOG_H_

