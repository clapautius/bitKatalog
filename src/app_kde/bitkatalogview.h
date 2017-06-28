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


#ifndef _BITKATALOGVIEW_H_
#define _BITKATALOGVIEW_H_

#include <Qt/qwidget.h>
#include <kparts/part.h>
#include <QTreeWidget>

#include "xfc.h"
#include "xmlentityitem.h"

class QPainter;
class KUrl;

/**
 * Helper function (callback for recursive collecting labels).
 **/
int collectLabels(uint depth, std::string path, Xfc& rCatalog, xmlNodePtr pNode,
                  void *pParam);


/**
 * This is the main view class for bitKatalog.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 *
 * This bitkatalog uses an HTML component as an example.
 *
 */
class bitKatalogView : public QWidget
{
    friend int collectLabels(uint depth, const std::string &path, Xfc& rCatalog,
                             xmlNodePtr pNode, void *pParam);

    Q_OBJECT
public:
	/**
	 * Default constructor
	 */
    bitKatalogView(QWidget *parent);

	/**
	 * Destructor
	 */
    virtual ~bitKatalogView();

    Xfc* getCatalog();
    void setCatalog(Xfc*);
    // doesn't modify gCatalogState
    // does refresh
    // does delete old xfc

    const std::vector<std::string>& getCatalogLabels() const;
    
    void populateTree(Xfc*);

    void addFirstLevelElement(XfcEntity &rEnt);
        
    /**
     * Random 'get' function
     */
    QString currentUrl();

    /**
     * Random 'set' function accessed by DCOP
     */
    virtual void openUrl(QString url);

    /**
     * Random 'set' function
     */
    virtual void openUrl(const KUrl& url);

    /**
     * Print this view to any medium -- paper or not
     */
    void print(QPainter *, int height, int width);

    int getNameColumnIndex() const
    {
        return k_name_column_idx;
    }

    int getDescriptionColumnIndex() const
    {
        return k_description_column_idx;
    }

    int getLabelsColumnIndex() const
    {
        return k_labels_column_idx;
    }

    int getStorageDevColumnIndex() const
    {
        return k_storage_column_idx;
    }

    int getNoColumns() const
    {
        return k_max_columns;
    }

    void fillColumnValues(QStringList &columns, const QString &name,
                          const QString &description, const QString &labels,
                          const XfcEntity &ent);

signals:
    /**
     * Use this signal to change the content of the statusbar
     */
    void signalChangeStatusbar(const QString& text);

    /**
     * Use this signal to change the content of the caption
     */
    void signalChangeCaption(const QString& text);

private slots:

    virtual void expandItem(QTreeWidgetItem*);
    virtual void collapseItem(QTreeWidgetItem*);

    virtual void contextMenu(const QPoint &point);
    
    void slotOnUrl(const QString& url);

    void slotSetTitle(const QString& title);
    

    void details() throw();

    void addLabelRec() throw();
    
    void verifyDisk() throw();

    void renameDisk() throw();

    void deleteDisk() throw();
    
private:
    
    void setupListView();

    void populateCatalogLabels();

    void addCatalogLabels(std::vector<std::string>);

    void addCatalogLabel(std::string);

    /**
     * Sort first columnt, etc.
     */
    void treeRedrawn();

    KParts::ReadOnlyPart *m_html;
    
    QTreeWidget *mListView; // :tmp: - rename this
    
    XmlEntityItem *mRootItem;
    
    Xfc *mCatalog;
    
    bool mModifiedCatalog;

    /**
     * A list of the labels in the catalog.
     **/
    std::vector<std::string> mCatalogLabels;
    
    XmlEntityItem *mpCurrentItem;

    std::string mCurrentItemPath;

protected:

    void setupColumns();

    const int k_name_column_idx = 0;
    const int k_description_column_idx = 1;
    const int k_storage_column_idx = 2;
    const int k_labels_column_idx = 3;
    const int k_max_columns = 4;

};

#endif // _BITKATALOGVIEW_H_
