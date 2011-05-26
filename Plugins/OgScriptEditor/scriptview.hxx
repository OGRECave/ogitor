/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2011 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
//
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
#ifndef SCRIPTVIEW_HXX
#define SCRIPTVIEW_HXX

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>
#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"

class ScriptTreeWidget : public QTreeWidget
{
    Q_OBJECT;
    friend class ScriptViewWidget;
public:
    ScriptTreeWidget(QWidget *parent = 0);
    virtual ~ScriptTreeWidget();
public Q_SLOTS:
    void contextMenu(int id);
protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
};

class ScriptViewWidget : public QWidget 
{
    Q_OBJECT;
public:
    explicit ScriptViewWidget(QWidget *parent = 0);
    virtual ~ScriptViewWidget();

    QTreeWidget *getTreeWidget() {return treeWidget;}

    void prepareView();
    void destroyScene();

    void onSceneLoadStateChange(Ogitors::IEvent* evt);

protected:
    ScriptTreeWidget *treeWidget;
    QTreeWidgetItem  *generalCategory;
    QTreeWidgetItem  *projectCategory;

};

extern ScriptViewWidget *mScriptViewWidget;


#endif // SCRIPTVIEW_HXX
