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


#ifndef _BITKATALOG_H_
#define _BITKATALOG_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kapplication.h>
#include <kmainwindow.h>

#include "bitkatalogview.h"

class KPrinter;
class KURL;

/**
 * This class serves as the main window for bitKatalog.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author Tudor Pristavu <clapautiuAtGmaliDotCom>
 * @version 0.1
 */
class bitKatalog : public KMainWindow
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
    void load(const KURL& url);

protected:
    /**
     * Overridden virtuals for Qt drag 'n drop (XDND)
     */
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);

protected:
    /**
     * This function is called when it is time for the app to save its
     * properties for session management purposes.
     */
    void saveProperties(KConfig *);

    /**
     * This function is called when this app is restored.  The KConfig
     * object points to the session management config file that was saved
     * with @ref saveProperties
     */
    void readProperties(KConfig *);


private slots:
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void filePrint();
    void optionsPreferences();

    void changeStatusbar(const QString& text);
    void changeCaption(const QString& text);

private:
    void setupAccel();
    void setupActions();

private:
    bitKatalogView *m_view;

    KPrinter   *m_printer;
};

#endif // _BITKATALOG_H_

