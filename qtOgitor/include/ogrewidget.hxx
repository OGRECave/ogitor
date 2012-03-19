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
#ifndef __OGREWIDGET_HXX__
#define __OGREWIDGET_HXX__

#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include "OgitorsPrerequisites.h"
#include "BaseEditor.h"
#include "OgitorsSystem.h"
#include "OgitorsRoot.h"
#include "CameraEditor.h"
#include "ViewportEditor.h"
#include "TerrainEditor.h"

#if defined(Q_WS_X11)
#include <QtGui/QX11Info>
#endif

//-----------------------------------------------------------------------------------------

class OverlayWidget : public QWidget
{
    Q_OBJECT;

public:
    OverlayWidget(QWidget *parent=0) : QWidget(parent) 
    {
        msgstr = QApplication::translate("OgreWidget","Initializing OGRE...");
        setContextMenuPolicy( Qt::PreventContextMenu );
    };

    virtual ~OverlayWidget() {};

    void setMessageString(QString msg)
    {
        if(msgstr != msg)
        {
            msgstr = msg;
            this->repaint();
        }
    };

protected:
    QString msgstr;

    void paintEvent(QPaintEvent* evt);
};

//-----------------------------------------------------------------------------------------

class OgreWidget : public QWidget, public Ogre::FrameListener, public Ogre::RenderSystem::Listener, public Ogre::MeshSerializerListener
{
    Q_OBJECT;

public:
    bool  mOgreInitialised;
    bool  mRenderStop;
    float mFrameRate;
    Ogre::RenderWindow *mRenderWindow;

    OgreWidget(QWidget *parent=0, bool doLoadFile = false, Qt::WindowFlags f=0);

    virtual ~OgreWidget();

    // Override QWidget::paintEngine to return NULL
    QPaintEngine* paintEngine() const; // Turn off QTs paint engine for the Ogre widget.
    
    void showCursorEx(bool bShow)
    {
        if(mCursorHidden == bShow)
        {
            if(!bShow)
                setCursor(Qt::BlankCursor);
            else
                setCursor(Qt::ArrowCursor);

            mCursorHidden = !bShow;
        }
    };
    void initializeOGRE();
    void ProcessKeyActions();
    void setTimerInterval(int value);
    void setOverlayMessage(QString msg)
    {
        mOverlayWidget->setMessageString(msg);
        update();
    };

    void setDoLoadFile(bool doLoad = true);
    bool isSizing()
    {
        return mScreenResize;
    }

    bool isSwitchingScene() {return mSwitchingScene;};
    void setSwitchingScene(bool switching) {mSwitchingScene = switching;};
    void processMaterialName(Ogre::Mesh *mesh, Ogre::String *name);
    void processSkeletonName(Ogre::Mesh *mesh, Ogre::String *name){};
    void stopRendering(bool stop) {mRenderStop = stop;};

    OverlayWidget* getOverlayWidget() {return mOverlayWidget;};

public Q_SLOTS:
    void timerLoop();
    void objectMenu(int id);

protected:
    Ogre::Root         *mOgreRoot;
    bool                mSwitchingScene;

    unsigned int  mLastKeyEventTime;
    volatile bool mScreenResize;
    int           mFrameCounter;
    double        mTotalFrameTime;
    bool          mCursorHidden;
    bool          mDoLoadFile;

    OverlayWidget *mOverlayWidget;

    void resizeEvent(QResizeEvent* evt);
    void paintEvent(QPaintEvent* evt);
    void focusOutEvent(QFocusEvent *evt);
    void focusInEvent(QFocusEvent *evt);
    void keyPressEvent(QKeyEvent *evt);
    void keyReleaseEvent(QKeyEvent *evt);
    void mouseMoveEvent(QMouseEvent *evt);
    void mousePressEvent(QMouseEvent *evt);
    void mouseReleaseEvent(QMouseEvent *evt);
    void leaveEvent(QEvent *evt);
    void wheelEvent(QWheelEvent *evt);
    void dragEnterEvent(QDragEnterEvent *evt);
    void dragLeaveEvent(QDragLeaveEvent *evt);
    void dragMoveEvent(QDragMoveEvent *evt);
    void dropEvent(QDropEvent *evt);

    void eventOccurred (const Ogre::String &eventName, const Ogre::NameValuePairList *parameters=0)
    {
         if(mScreenResize)
             return;
    };

    void onSceneLoadStateChange(Ogitors::IEvent* evt);
    bool frameStarted(const Ogre::FrameEvent& evt);

    unsigned int getMouseButtons(Qt::MouseButtons buttons, Qt::MouseButton button);
    void displayFPS(float time);
    void showObjectMenu();
};

//-----------------------------------------------------------------------------------------

#endif // __OGREWIDGET_HXX__

//-----------------------------------------------------------------------------------------
