/***************************************************************************
 *   Copyright (C) 2009-2020 by Tudor Marian Pristavu                      *
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
#include "pref.h"

#include <Qt/qdrag.h>
#include <Qt/qpainter.h>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrinter>
//#include <qpaintdevicemetrics.h>

#ifdef USE_QT_FILEDLG
#  include <Qt/qfiledialog.h>
#else
#  include <kfiledialog.h>
#endif

#include <kapplication.h>
#include <kdeversion.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstatusbar.h>
//#include <kaccel.h>
#include <kconfig.h>
#include <kio/netaccess.h>
#include <kurl.h>
#include <kurlrequesterdlg.h>

#include <kaction.h>
#include <kmenu.h>
#include <kmenubar.h>
#include <kmessagebox.h>
#include <kshortcutsdialog.h>
#include <kstandardaction.h>
#include <kstdaccel.h>

#include "adddiskbox.h"
#include "fs.h"
#include "main.h"
#include "misc.h"
#include "newcatalogbox.h"
#include "searchbox.h"
#include "xfcEntity.h"

using std::string;

bitKatalog::bitKatalog() : KXmlGuiWindow(), m_view(new bitKatalogView(this))
//,m_printer(0)
{
    // accept dnd
    setAcceptDrops(true);

    // tell the KMainWindow that this is indeed the main widget
    setCentralWidget(m_view);

    // disable help menu in order not to break the order of the standard menus
    // I'm not sure how to do this write
    // I'll create the help menu after the creation of File/Edit/etc.
    setHelpMenuEnabled(false);

    // createGUI();
    setupGUI();

    // for some strange reason, in KDE 4.4 (and probably 4.3 too), the default
    // menu bar has lots of menu entries (Games, Tools, and many more). I don't
    // know why and I don't know how to delete them, so I delete the whole
    // KMenuBar and create a new one, empty.
    KMenuBar *pOldMenuBar = menuBar();
    setMenuBar(new KMenuBar());
    delete pOldMenuBar;

    mpFileMenu = new KMenu("&File");
    menuBar()->addMenu(mpFileMenu);
    mpEditMenu = new KMenu("&Edit");
    menuBar()->addMenu(mpEditMenu);

    // create the Help menu (the last one)
    menuBar()->addMenu(helpMenu());

    // mpEditMenu->addAction("&Search");
    mpEditMenu->addAction("&Add disk to catalog", this, SLOT(addDisk()));

    // then, setup our actions
    setupActions();

    // and a status bar
    statusBar()->show();

    toolBar()->show();

    // apply the saved mainwindow settings, if any, and ask the mainwindow
    // to automatically save settings if changed: window size, toolbar
    // position, icon size, etc.
    setAutoSaveSettings();

    gpView = m_view;

    // allow the view to change the statusbar and caption
    connect(m_view, SIGNAL(signalChangeStatusbar(const QString &)), this,
            SLOT(changeStatusbar(const QString &)));
    connect(m_view, SIGNAL(signalChangeCaption(const QString &)), this,
            SLOT(changeCaption(const QString &)));
}

bitKatalog::~bitKatalog() {}

void bitKatalog::load(const KUrl &url)
{
    QString target;
// the below code is what you should normally do.  in this
// example case, we want the url to our own.  you probably
// want to use this code instead for your app

#if 0
    // download the contents
    if (KIO::NetAccess::download(url, target)) {
        // set our caption
        setCaption(url);

        // load in the file (target is always local)
        loadFile(target);

        // and remove the temp file
        KIO::NetAccess::removeTempFile(target);
    }
#endif

    setCaption(url.prettyUrl());
    m_view->openUrl(url);
}

void bitKatalog::setupActions()
{
    mpFileMenu->addAction(KStandardAction::openNew(this, SLOT(fileNew()), this));
    mpFileMenu->addAction(KStandardAction::open(this, SLOT(fileOpen()), this));
    mpFileMenu->addAction(KStandardAction::save(this, SLOT(fileSave()), this));
    mpFileMenu->addAction(KStandardAction::saveAs(this, SLOT(fileSaveAs()), this));
    mpFileMenu->addSeparator();
    mpFileMenu->addAction(KStandardAction::quit(kapp, SLOT(quit()), this));

    mpEditMenu->addAction(KStandardAction::find(this, SLOT(search()), this));
    mpEditMenu->addSeparator();
    mpEditMenu->addAction(
        KStandardAction::preferences(this, SLOT(optionsPreferences()), this));

    mpEditMenu->addAction(m_statusbarAction = KStandardAction::showStatusbar(
                              this, SLOT(optionsShowStatusbar()), this));

    /*
    mpEditMenu->addAction(m_toolbarAction=new KToggleAction("Show Toolbar", this));
    connect(m_toolbarAction, SIGNAL(triggered()), this,
    SLOT(optionsShowToolbar()));
    */

    mpEditMenu->addAction(
        KStandardAction::keyBindings(this, SLOT(optionsConfigureKeys()), this));
    // mpEditMenu->addAction(KStandardAction::configureToolbars(this,
    // SLOT(optionsConfigureToolbars()), this));
    // mpEditMenu->addAction(KStandardAction::preferences(this,
    // SLOT(optionsPreferences()), this));

    toolBar()->addAction(KStandardAction::open(this, SLOT(fileOpen()), this));
    toolBar()->addAction(KStandardAction::save(this, SLOT(fileSave()), this));
    toolBar()->addAction(KStandardAction::find(this, SLOT(search()), this));
    KIcon addIcon("list-add");
    KAction *pAddAct = new KAction(addIcon, "Add disk ...", this);
    connect(pAddAct, SIGNAL(triggered()), this, SLOT(addDisk()));
    toolBar()->addAction(pAddAct);
}

void bitKatalog::saveProperties(KConfigGroup &config)
{
    // the 'config' object points to the session managed
    // config file.  anything you write here will be available
    // later when this app is restored

    if (!m_view->currentUrl().isEmpty()) {
        config.writeEntry("lastURL", m_view->currentUrl());
    }
}

void bitKatalog::readProperties(const KConfigGroup &config)
{
    // the 'config' object points to the session managed
    // config file.  this function is automatically called whenever
    // the app is being restored.  read in here whatever you wrote
    // in 'saveProperties'

    QString url = config.readEntry("lastURL");

    if (!url.isEmpty())
        m_view->openUrl(KUrl(url));
}

void bitKatalog::dragEnterEvent(QDragEnterEvent *event)
{
    // accept uri drops only
    if (KUrl::List::canDecode(event->mimeData()))
        event->accept();
}

void bitKatalog::dropEvent(QDropEvent *)
{
// this is a very simplistic implementation of a drop event.  we
// will only accept a dropped URL.  the Qt dnd code can do *much*
// much more, so please read the docs there
// :fixme: - kde4
#if 0
    KUrl::List urls;

    // see if we can decode a URI.. if not, just ignore it
    if (KUrlDrag::decode(event, urls) && !urls.isEmpty())
    {
        // okay, we have a URI.. process it
        const KUrl &url = urls.first();

        // load in the file
        load(url);
    }
#endif
}

void bitKatalog::fileNew()
{
    // this slot is called whenever the File->New menu is selected,
    // the New shortcut is pressed (usually CTRL+N) or the New toolbar
    // button is clicked
    if (gCatalogState == 1) {
        if (KMessageBox::warningContinueCancel(
                this, "Catalog modified. Changes will be lost. Continue?") ==
            KMessageBox::Cancel)
            return;
    }
    NewCatalogBox *lpNewCatalogBox = new NewCatalogBox;
    lpNewCatalogBox->exec();
}

void bitKatalog::fileOpen()
{
    // this slot is called whenever the File->Open menu is selected,
    // the Open shortcut is pressed (usually CTRL+O) or the Open toolbar
    // button is clicked
    // standard filedialog
    QString lLastDir;
    lLastDir = gpConfig->group("").readEntry("LastDir", ".");
    KUrl url;
#ifdef USE_QT_FILEDLG
    url = QFileDialog::getOpenFileName(this, tr("Open file"), lLastDir, QString(),
                                       nullptr, QFileDialog::DontUseNativeDialog);
#else
    url = KFileDialog::getOpenUrl(lLastDir, QString::null, this, i18n("Open Location"));
#endif
    if (!url.isEmpty()) {
        m_view->openUrl(url);
        lLastDir = url.directory();
        gpConfig->group("").writeEntry("LastDir", lLastDir);
        gpConfig->sync();
    }
}

void bitKatalog::fileSave()
{
    // this slot is called whenever the File->Save menu is selected,
    // the Save shortcut is pressed (usually CTRL+S) or the Save toolbar
    // button is clicked

    // save the current file
    if (gCatalogState == 1) {
        Xfc *lpCatalog = m_view->getCatalog();
        if (lpCatalog == NULL) {
            KMessageBox::error(this,
                               "No catalog! You should load a catalog from file \
or create a new one");
        } else {
            if (mCatalogPath == "") {
                fileSaveAs();
            } else {
                string err;
                // :fixme: - check global config
                if (saveWithBackup(lpCatalog, mCatalogPath, err) == 0) {
                    // m_view->resetModifiedFlag();
                    gCatalogState = 2;
                    gpMainWindow->updateTitle(false);
                    // KMessageBox::information(this, "File succesfully saved");
                } else {
                    KMessageBox::error(
                        this,
                        QString("Error saving xml file. Error was: ") + err.c_str());
                }
            }
        }
    }
}

void bitKatalog::fileSaveAs()
{
    // this slot is called whenever the File->Save As menu is selected,
    QString lLastDir;
    lLastDir = gpConfig->group("").readEntry("LastDir", ".");
    KUrl file_url;
#ifdef USE_QT_FILEDLG
    file_url = QFileDialog::getSaveFileName(nullptr, "Save as ...", lLastDir, QString(), nullptr, QFileDialog::DontUseNativeDialog);
#else
    file_url = KFileDialog::getSaveUrl(lLastDir);
#endif
    if (!file_url.isEmpty() && file_url.isValid()) {
        lLastDir = file_url.directory();
        gpConfig->group("").writeEntry("LastDir", lLastDir);
        gpConfig->sync();

        Xfc *lpCatalog = m_view->getCatalog();
        if (lpCatalog == NULL) {
            KMessageBox::error(
                this,
                "No catalog! You should load a catalog from file or create a new one");
        } else {
            string err;
            // :fixme: - check global config
            if (saveWithBackup(lpCatalog, file_url.path().toStdString(), err) == 0) {
                // m_view->resetModifiedFlag();
                gCatalogState = 2;
                mCatalogPath = file_url.path().toStdString();
                gpMainWindow->updateTitle(false);
                // KMessageBox::information(this, "File succesfully saved");
            } else {
                KMessageBox::error(
                    this,
                    QString("Error saving xml file. Error was: ") + QString(err.c_str()));
            }
        }
    }
}

void bitKatalog::filePrint()
{
// :fixme: - kde4
#if 0
    // this slot is called whenever the File->Print menu is selected,
    // the Print shortcut is pressed (usually CTRL+P) or the Print toolbar
    // button is clicked
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    //printDialog.setWindowTitle(i18n("Print Dialog Title"));
    if (printDialog.exec()) {
        // setup the printer.  with Qt, you always "print" to a
        // QPainter.. whether the output medium is a pixmap, a screen,
        // or paper
        QPainter p;
        p.begin(printer);

        // we let our view do the actual printing
        QPaintDeviceMetrics metrics(m_printer);
        m_view->print(&p, metrics.height(), metrics.width());

        // and send the result to the printer
        p.end();
    }
#endif
}

/*
void
bitKatalog::optionsShowToolbar()
{
    // this is all very cut and paste code for showing/hiding the
    // toolbar
    if (m_toolbarAction->isChecked())
        toolBar()->show();
    else
        toolBar()->hide();
}
*/

void bitKatalog::optionsShowStatusbar()
{
    // this is all very cut and paste code for showing/hiding the
    // statusbar
    if (m_statusbarAction->isChecked())
        statusBar()->show();
    else
        statusBar()->hide();
}

void bitKatalog::optionsConfigureKeys()
{
    KShortcutsDialog::configure(actionCollection());
}

void bitKatalog::optionsConfigureToolbars()
{
    // use the standard toolbar editor
    saveMainWindowSettings(KGlobal::config()->group(""));
}

void bitKatalog::newToolbarConfig()
{
    // this slot is called when user clicks "Ok" or "Apply" in the toolbar editor.
    // recreate our GUI, and re-apply the settings (e.g. "text under icons", etc.)
    createGUI();
    applyMainWindowSettings(KGlobal::config()->group(""));
}

void bitKatalog::optionsPreferences()
{
    // popup some sort of preference dialog, here
    bitKatalogPreferences dlg;
    if (dlg.exec()) {
        // redo your settings
    }
}

void bitKatalog::changeStatusbar(const QString &text)
{
    // display the text on the statusbar
    statusBar()->changeItem(text, 0);
}

void bitKatalog::changeCaption(const QString &text)
{
    // display the text on the caption
    setCaption(text);
}

void bitKatalog::search()
{
    if (gCatalogState == 1 || gCatalogState == 2) {
        msgDebug("Trying to search ...");
        SearchBox *lpSearchBox =
            new SearchBox(m_view->getCatalog(), m_view->getCatalogLabels());
        lpSearchBox->exec();
    } else
        KMessageBox::error(this, "No catalog");
}

void bitKatalog::addDisk()
{
    if (gCatalogState == 1 || gCatalogState == 2) {
        AddDiskBox *pAddDiskBox = new AddDiskBox(m_view->getCatalog());
        pAddDiskBox->exec();
        if (pAddDiskBox->catalogWasModified()) {
            gCatalogState = 1;  // modified
            gpMainWindow->updateTitle(true);
            if (pAddDiskBox->resultOk()) {
                string newDiskPath = pAddDiskBox->addedDiskPath();
                XfcEntity ent =
                    m_view->getCatalog()->getEntityFromPath(newDiskPath.c_str());
                m_view->addFirstLevelElement(ent);
            } else {
                KMessageBox::error(this,
                                   "The disk could not be added.\n\
The catalog was left in an inconsistent state.\n\
Do not save this catalog!");
            }
        }
    } else
        KMessageBox::error(this, "No catalog");
}

void bitKatalog::createNumberedBackup(std::string lFile) throw(std::string)
{
    char lAux[5];
    int i;
    std::string lBackupName;
    if (fileExists(lFile)) {
        for (i = 0; i < 999; i++) {
            sprintf(lAux, ".%03d", i);
            lBackupName = lFile + lAux;
            if (!fileExists(lBackupName))
                break;
        }
        if (i == 1000)  // wow, 1000 backups
            throw std::string("way too many backups");
        lBackupName = lFile + lAux;
        rename(lFile, lBackupName);
    }
}

bitKatalogView *bitKatalog::getView() const { return m_view; }

void bitKatalog::setCatalogPath(std::string lCatalogPath) { mCatalogPath = lCatalogPath; }

void bitKatalog::setCatalogPath(QString catalogPath)
{
    mCatalogPath = catalogPath.toStdString();
}

std::string bitKatalog::getCatalogPath() const { return mCatalogPath; }

bool bitKatalog::queryClose()
{
    if (1 == gCatalogState) {
        switch (KMessageBox::warningYesNoCancel(
            this, i18n("The catalog is not saved. Are you sure you want to exit?"))) {
            case KMessageBox::Yes:
                return true;
            case KMessageBox::No:
                return false;
            default:  // cancel
                return false;
        }
    } else
        return true;
}

void bitKatalog::updateTitle(bool modified)
{
    setCaption(KDialog::makeStandardCaption(
        mCatalogPath.c_str(), this,
        modified ? KDialog::ModifiedCaption : KDialog::NoCaptionFlags));
}
