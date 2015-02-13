/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2015 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
//
//This program is free software; you can redistribute it and/or modify it under
//the terms of the GNU Lesser General Public License as published by the Free Software
//Foundation; either version 2 of the License, or (at your option) any later
//version.
//
//This program is distributed in the hope that it will be useful, but WITHOUT
//ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
//FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
//
//You should have received a copy of the GNU Lesser General Public License along with
//this program; if not, write to the Free Software Foundation, Inc., 59 Temple
//Place - Suite 330, Boston, MA 02111-1307, USA, or go to
//http://www.gnu.org/copyleft/lesser.txt.
////////////////////////////////////////////////////////////////////////////////*/

#ifndef LAYERVIEW_HXX
#define LAYERVIEW_HXX

#include <QtWidgets/QWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>

class LayerTreeWidget : public QTreeWidget
{
    Q_OBJECT;
public:
    LayerTreeWidget(QWidget *parent = 0);
    virtual ~LayerTreeWidget() {};
public Q_SLOTS:
    void contextMenu(int id);
    void itemEditDone();
    void selectAll();
    void lockAll();
    void unlockAll();
    void toggleLayer();
protected:
    QTreeWidgetItem *mEditedItem;
    QTreeWidgetItem *mContextLayer;

    void mousePressEvent(QMouseEvent *evt);
    void dragEnterEvent(QDragEnterEvent *evt);
    void dragMoveEvent(QDragMoveEvent *evt);
    void dropEvent(QDropEvent *evt);
    void contextMenuEvent(QContextMenuEvent *evt);
    void keyPressEvent(QKeyEvent *evt);
    void mouseDoubleClickEvent( QMouseEvent *evt );
};

class LayerViewWidget : public QWidget {
    Q_OBJECT;
public:
    explicit LayerViewWidget(QWidget *parent = 0);
    virtual ~LayerViewWidget();

    inline QTreeWidget *getTreeWidget() {return treeWidget;}
    QTreeWidgetItem *getRootItem(int index);
    void setSelectionUpdate(bool value) { selectionUpdateInProgress = value; }
    void updateLayerNames();
    void resetLayerNames();
    void enableLayer(int layerid, bool enable);
public Q_SLOTS:
    void selectionChanged();
    void removeLayer();
    void addLayer();
    void insertLayerBefore();
    void insertLayerAfter();
protected:
    LayerTreeWidget *treeWidget;
    QToolBar        *toolBar;
    QAction         *actAddLayer;
    QAction         *actRemoveLayer;
    bool             selectionUpdateInProgress;
    QTreeWidgetItem *mLayerRootItems[31];
    QTreeWidgetItem *mCurrentItemWidget;

    bool removeLayer(int index);
    bool addLayer(int index);
};

#endif // LAYERVIEW_HXX
