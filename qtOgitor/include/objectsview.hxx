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

#ifndef OBJECTSVIEW_HXX
#define OBJECTSVIEW_HXX

#include <QtWidgets/QWidget>
#include <QtCore/QTimer>

#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"
#include "OgitorsProperty.h"

class QListWidget;

//! Drag'n'drop structure
/*!  
    A structure that is used to track object state after initiating dragging
*/
struct OBJECTSDRAGDATA
{
    Ogitors::CBaseEditor            *Object;                /** Pointer to Dragged Object */
    Ogre::String                     ObjectType;            /** Type of an object being dragged */
    Ogitors::OgitorsPropertyValueMap Parameters; /** Information about the object */
};

class ObjectsViewWidget : public QWidget, public Ogitors::DragDropHandler 
{
    Q_OBJECT;
public:
    explicit ObjectsViewWidget(QWidget *parent = 0);
    virtual ~ObjectsViewWidget();
    void prepareView();
    void clearView();
    QListWidget *getListWidget() {return listWidget;};
public Q_SLOTS:
    void updateView();
protected:
    QListWidget *listWidget;
    QTimer      *mTimer;
    OBJECTSDRAGDATA mDragData;

    void resizeEvent(QResizeEvent* evt);

    /** DragDropHandler functions**/
    virtual bool OnDragEnter();
    virtual void OnDragLeave();
    virtual bool OnDragMove(Ogre::Viewport *vp, unsigned int modifier, Ogre::Vector2& position);
    virtual void OnDragDropped(Ogre::Viewport *vp, Ogre::Vector2& position);
    virtual void OnDragWheel(Ogre::Viewport *vp, float delta);
};

#endif // OBJECTSVIEW_HXX
