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

#ifndef ENTITYVIEW_HXX
#define ENTITYVIEW_HXX

#include <QtGui/QWidget>
#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"
#include "OgitorsProperty.h"

class QListWidget;
class QLineEdit;
class QToolButton;

//! Drag'n'drop structure
/*!  
    A structure that is used to track object state after initiating dragging
*/
struct DRAGDATA
{
    Ogitors::CBaseEditor            *Object;                /** Pointer to Dragged Object */
    Ogre::String                     ObjectType;            /** Type of an object being dragged */
    Ogitors::OgitorsPropertyValueMap Parameters; /** Information about the object */
};

typedef std::map<QString, unsigned char *> ImageMap;

class EntityViewWidget : public QWidget, public Ogitors::DragDropHandler
{
    Q_OBJECT;
public:
    explicit EntityViewWidget(QWidget *parent = 0);
    virtual ~EntityViewWidget();
    void prepareView();
    void clearView();
    QListWidget *getListWidget() {return listWidget;};

public Q_SLOTS:
        void filterBoxTextChanged(QString text);
        
protected:
    QListWidget *listWidget;
    QLineEdit   *filterBox;
    QToolButton *clearFilterButton;
    ImageMap     mIcons;
    DRAGDATA     mDragData; 
    
    
    void resizeEvent(QResizeEvent* evt);
    void _createImages(ImageMap& retlist);

    /** DragDropHandler functions**/
    virtual bool OnDragEnter();
    virtual void OnDragLeave();
    virtual bool OnDragMove(Ogre::Viewport *vp, unsigned int modifier, Ogre::Vector2& position);
    virtual void OnDragDropped(Ogre::Viewport *vp, Ogre::Vector2& position);
    virtual void OnDragWheel(Ogre::Viewport *vp, float delta);
};

#endif // ENTITYVIEW_HXX
