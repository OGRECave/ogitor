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

#ifndef MATERIALVIEW_HXX
#define MATERIALVIEW_HXX

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>

#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"

//----------------------------------------------------------------------------------------

class MaterialTreeWidget : public QTreeWidget, public Ogitors::DragDropHandler
{
    Q_OBJECT;
    friend class MaterialViewWidget;
public:
    MaterialTreeWidget(QWidget *parent = 0);
    virtual ~MaterialTreeWidget();
public Q_SLOTS:
    void contextMenu(int id);
    
protected:
    Ogitors::CMaterialEditor *mMaterialEditor;

    void contextMenuEvent(QContextMenuEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    /** DragDropHandler functions**/
    virtual bool OnDragEnter();
    virtual void OnDragLeave();
    virtual bool OnDragMove(Ogre::Viewport *vp, unsigned int modifier, Ogre::Vector2& position);
    virtual void OnDragDropped(Ogre::Viewport *vp, Ogre::Vector2& position);
    virtual void OnDragWheel(Ogre::Viewport *vp, float delta) {};
};

//----------------------------------------------------------------------------------------

class MaterialViewWidget : public QWidget 
{
    Q_OBJECT;
public:
    explicit MaterialViewWidget(QWidget *parent = 0);
    virtual ~MaterialViewWidget();

    QTreeWidget *getTreeWidget() {return treeWidget;}
    virtual Ogitors::CMaterialEditor *getMaterialEditor() {return mMaterialEditor;}

    void prepareView();
    void destroyView();

    void onSceneLoadStateChange(Ogitors::IEvent* evt);
    void onGlobalPrepareView(Ogitors::IEvent* evt);

public Q_SLOTS:
    void selectionChanged();

protected:
    MaterialTreeWidget              *treeWidget;
    Ogitors::CMaterialEditor        *mMaterialEditor;
    Ogitors::CTechniqueEditor       *mTechniqueEditor;
};

//----------------------------------------------------------------------------------------

extern MaterialViewWidget *mMaterialViewWidget;

//----------------------------------------------------------------------------------------

#endif // MATERIALVIEW_HXX

//----------------------------------------------------------------------------------------
