/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2013 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
///
/// The MIT License
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////*/

#ifndef SCENEVIEW_HXX
#define SCENEVIEW_HXX

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>

#include "extendedTreeView.hxx"

//-----------------------------------------------------------------------------------------

class SceneTreeWidget : public ExtendedTreeWidget
{
    Q_OBJECT;

public:
    SceneTreeWidget(QWidget *parent = 0);
    virtual ~SceneTreeWidget() {};

public Q_SLOTS:
    void contextMenu(int id);
    void pasteFromClipboardBuffer(int id);
    void itemDoubleClicked(QTreeWidgetItem* item, int column);

protected:
    void contextMenuEvent(QContextMenuEvent *evt);
    void dragEnterEvent(QDragEnterEvent *evt);
    void dragMoveEvent(QDragMoveEvent *evt);
    void dropEvent(QDropEvent *evt);
    void keyPressEvent(QKeyEvent *evt);
};

//-----------------------------------------------------------------------------------------

class SceneViewWidget : public QWidget 
{
    Q_OBJECT;

public:
    explicit SceneViewWidget(QWidget *parent = 0);
    virtual ~SceneViewWidget();

    inline QTreeWidget *getTreeWidget() {return treeWidget;}
    void setSelectionUpdate(bool value) {selectionUpdateInProgress = value;}
	void setEditRenameState();

public Q_SLOTS:
    void selectionChanged();

protected:
    SceneTreeWidget *treeWidget;
    bool             selectionUpdateInProgress;
};

//-----------------------------------------------------------------------------------------

#endif // SCENEVIEW_HXX

//-----------------------------------------------------------------------------------------
