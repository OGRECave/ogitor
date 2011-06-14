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
#include "mainwindow.hxx"
#include "ogrewidget.hxx"
#include "entityview.hxx"
#include "layerview.hxx"
#include "objectsview.hxx"
#include "templateview.hxx"
#include "propertiesviewgeneral.hxx"
#include "projectfilesview.hxx"

#include "MultiSelEditor.h"
#include "NodeEditor.h"
#include "EntityEditor.h"
#include "DefaultEvents.h"
#include "EventManager.h"

using namespace Ogitors;

extern QString ConvertToQString(Ogre::UTFString& value);

bool ViewKeyboard[1024];

//----------------------------------------------------------------------------------------
void OverlayWidget::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);
    painter.setClipRect(0,0,width(),height());
    painter.setBrush(QBrush(QColor(0,0,0)));
    painter.fillRect(QRectF(0,0,width(),height()), QColor(0,0,0));
    painter.setPen(QColor(210,210,210));
    painter.drawText(QRectF(0,0,width(),height()),msgstr,QTextOption(Qt::AlignVCenter | Qt::AlignHCenter));
}
//----------------------------------------------------------------------------------------
OgreWidget::OgreWidget(QWidget *parent, bool doLoadFile, Qt::WindowFlags f): QWidget( parent,  f /*| Qt::MSWindowsOwnDC*/ ),
mOgreRoot(0), mRenderWindow(0), mOgreInitialised(false), mLastKeyEventTime(0),
mRenderStop(false), mScreenResize(false), mCursorHidden(false), mDoLoadFile(doLoadFile)
{
    mFrameCounter = 0;
    mTotalFrameTime = 0;
    mSwitchingScene = false;

    for(int i = 0;i < 1024;i++)
        ViewKeyboard[i] = false;

    setAcceptDrops(true);
    setContextMenuPolicy( Qt::PreventContextMenu );

    setFocusPolicy(Qt::WheelFocus);
    setMouseTracking(true);
    setAttribute(Qt::WA_NoBackground);
    setAttribute(Qt::WA_PaintOnScreen);

    mOverlayWidget = new OverlayWidget(this);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->addWidget(mOverlayWidget);
    setLayout(layout);
    
    EventManager::getSingletonPtr()->connectEvent(EventManager::LOAD_STATE_CHANGE,      this, true, 0, true, 0, EVENT_CALLBACK(OgreWidget, onSceneLoadStateChange));
}
//----------------------------------------------------------------------------------------
OgreWidget::~OgreWidget()
{
    if(mOgreInitialised)
    {
        Ogre::Root::getSingletonPtr()->removeFrameListener(this);
        mOgreRoot->getRenderSystem()->removeListener(this);
    }

    EventManager::getSingletonPtr()->disconnectEvent(EventManager::LOAD_STATE_CHANGE,           this);
    
    destroy();
}
//----------------------------------------------------------------------------------------
static Ogre::Vector3 oldCamPos = Ogre::Vector3::ZERO;
int oldTris = 0;

bool OgreWidget::frameStarted(const Ogre::FrameEvent& evt)
{
    displayFPS(evt.timeSinceLastFrame);
    OgitorsRoot::getSingletonPtr()->GetViewport()->UpdateAutoCameraPosition(evt.timeSinceLastFrame);

    OgitorsRoot::getSingletonPtr()->Update(evt.timeSinceLastFrame);

    Ogre::Vector3 campos = OgitorsRoot::getSingletonPtr()->GetViewport()->getCameraEditor()->getCamera()->getDerivedPosition();
    if(oldCamPos != campos)
    {
        char temp[128];
        sprintf(temp," X: % .2f, Y: % .2f, Z: % .2f", campos.x, campos.y, campos.z);
        QString camtext = QApplication::translate("MainWindow","Camera Position:") + QString(temp);
        mOgitorMainWindow->mCamPosLabel->setText(camtext);
        oldCamPos = campos;
    }

    int tris = mRenderWindow->getTriangleCount();
    if(oldTris != tris)
    {
        QString tritext = QApplication::translate("MainWindow","Triangles : %1").arg(tris);
        mOgitorMainWindow->mTriangleCountLabel->setText(tritext);
        oldTris = tris;
    }

    return true;
}
//----------------------------------------------------------------------------------------
void OgreWidget::initializeOGRE()
{
    //== Creating and Acquiring Ogre Window ==//

    // Get the parameters of the window QT created

    mOgreRoot = Ogre::Root::getSingletonPtr();

    Ogre::NameValuePairList params;

#if defined(Q_WS_MAC) || defined(Q_WS_WIN)
    params["externalWindowHandle"] = Ogre::StringConverter::toString((size_t) (this->winId()));
#else
    const QX11Info info = this->x11Info();
    //QX11Info info = x11Info();
    Ogre::String winHandle;
    winHandle  = Ogre::StringConverter::toString((unsigned long)(info.display()));
    winHandle += ":";
    winHandle += Ogre::StringConverter::toString((unsigned int)(info.screen()));
    winHandle += ":";
    winHandle += Ogre::StringConverter::toString((unsigned long)(this->winId()));
    winHandle += ":";
    winHandle += Ogre::StringConverter::toString((unsigned long)(info.visual()));

    params["externalWindowHandle"] = winHandle;
#endif

#if defined(Q_WS_MAC)
    params["macAPI"] = "cocoa";   
    params["macAPICocoaUseNSView"] = "true";
#endif

    mRenderWindow = mOgreRoot->createRenderWindow( "QtOgitorRenderWindow",
        this->width(),
        this->height(),
        false,
        &params );

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    mRenderWindow->windowMovedOrResized();
    //resizeEvent(0);
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    mRenderWindow->resize(width(), height());
#endif

    mOgreRoot->getRenderSystem()->addListener(this);
    OgitorsRoot::getSingletonPtr()->SetRenderWindow(mRenderWindow);
    Ogre::Root::getSingletonPtr()->addFrameListener(this);
    Ogre::MeshManager::getSingletonPtr()->setListener(this);

    mOgreInitialised = true;
}
//------------------------------------------------------------------------------------
void OgreWidget::timerLoop()
{
    if(mOgitorMainWindow->isMinimized())
        return;

    if(mRenderStop)
    {
        if(QMessageBox::information(this,"qtOgitor", tr("Render Device is Lost! Please click ok to continue.."), QMessageBox::Ok) == QMessageBox::Ok)
            mRenderStop = false;
    }
    update();
}
//------------------------------------------------------------------------------------
void OgreWidget::setDoLoadFile(bool doLoad)
{
    mDoLoadFile = doLoad;
}
//------------------------------------------------------------------------------------
bool adjustFrameTime = false;

void OgreWidget::paintEvent(QPaintEvent* evt)
{
    if(!isVisible())
        return;

    if(!mRenderWindow)
        initializeOGRE();

    if(mOgreInitialised && OgitorsRoot::getSingletonPtr()->IsSceneLoaded() && !mRenderStop)
    {
        if(this->width() > 0 && this->height() > 0)
        {
            if(OgitorsRoot::getSingletonPtr()->IsClearScreenNeeded())
            {
                Ogre::Camera *clearcam = OgitorsRoot::getSingletonPtr()->GetSceneManager()->createCamera("sbtClearCam");
                clearcam->setNearClipDistance(0.0001f);
                clearcam->setFarClipDistance(0.0002f);
                clearcam->lookAt(0,-1,0);
                OgitorsRoot::getSingletonPtr()->GetRenderWindow()->addViewport(clearcam,0);
            }

            if(adjustFrameTime)
            {
                mOgreRoot->renderOneFrame(0.01f);
                adjustFrameTime = false;
            }
            else
                mOgreRoot->renderOneFrame();


#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
            mRenderWindow->update();
#endif
            if(OgitorsRoot::getSingletonPtr()->IsClearScreenNeeded())
            {
                OgitorsRoot::getSingletonPtr()->GetRenderWindow()->removeViewport(0);
                OgitorsRoot::getSingletonPtr()->GetSceneManager()->destroyCamera("sbtClearCam");
                OgitorsRoot::getSingletonPtr()->ClearScreenBackground(false);
            }
        }
    }
    else
    {
        QString msgstr = tr("Initializing OGRE...");

        if(mOgreInitialised && !OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
        {
            if(mDoLoadFile)
                msgstr = tr("Loading Scene...");
            else
                msgstr = tr("Please load a Scene File...");
        }

        if(mRenderStop)
            msgstr = tr("Device Lost...");

        mOverlayWidget->setMessageString(msgstr);

        adjustFrameTime = true;
    }
}
//------------------------------------------------------------------------------------
void OgreWidget::resizeEvent(QResizeEvent* evt)
{
    if(!mRenderWindow)
        return;

    mScreenResize = true;

#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    mRenderWindow->resize(width(), height());
#endif

    mRenderWindow->windowMovedOrResized();

    if(OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
        OgitorsRoot::getSingletonPtr()->RenderWindowResized();

    mScreenResize = false;
}
//------------------------------------------------------------------------------------
void OgreWidget::focusInEvent(QFocusEvent *evt)
{
    if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
        return;

    for(int i = 0;i < 1024;i++)
        ViewKeyboard[i] = false;

    evt->setAccepted(true);
}
//------------------------------------------------------------------------------------
void OgreWidget::focusOutEvent(QFocusEvent *evt)
{
    if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
        return;

    for(int i = 0;i < 1024;i++)
        ViewKeyboard[i] = false;

    if(OgitorsRoot::getSingletonPtr()->GetTerrainEditor())
        OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->stopEdit();

    evt->setAccepted(true);
}
//------------------------------------------------------------------------------------
void OgreWidget::keyPressEvent(QKeyEvent *evt)
{
    if(evt->isAutoRepeat())
        return;

    if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
        return;

    unsigned int key = evt->key();
    if(key > 255)
        key = (key & 0xFFF) + 0xFF;

    OgitorsRoot::getSingletonPtr()->OnKeyDown(key);
}
//------------------------------------------------------------------------------------
void OgreWidget::keyReleaseEvent(QKeyEvent *evt)
{
    if(evt->isAutoRepeat())
        return;

    if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
        return;

    unsigned int key = evt->key();
    if(key > 255)
        key = (key & 0xFFF) + 0xFF;

    OgitorsRoot::getSingletonPtr()->OnKeyUp(key);
}
//------------------------------------------------------------------------------------
unsigned int OgreWidget::getMouseButtons(Qt::MouseButtons buttons, Qt::MouseButton button)
{
    unsigned int flags = 0;
    buttons |= button;

    if(buttons.testFlag(Qt::LeftButton))
        flags |= OMB_LEFT;
    if(buttons.testFlag(Qt::RightButton))
        flags |= OMB_RIGHT;
    if(buttons.testFlag(Qt::MidButton))
        flags |= OMB_MIDDLE;

    return flags;
}
//------------------------------------------------------------------------------------
bool OgreWidgetMouseMovedSincePress = false;
void OgreWidget::mouseMoveEvent(QMouseEvent *evt)
{
    if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
        return;

    if(!hasFocus())
        setFocus();

    Ogre::Vector2 pos(evt->x(), evt->y());

    OgitorsRoot::getSingletonPtr()->OnMouseMove(pos, getMouseButtons(evt->buttons(), evt->button()));

    OgreWidgetMouseMovedSincePress = true;
}
//------------------------------------------------------------------------------------
void OgreWidget::mousePressEvent(QMouseEvent *evt)
{
    if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
        return;

    if(!hasFocus())
        setFocus();

    Ogre::Vector2 pos1(evt->x(), evt->y());

    if(evt->button() == Qt::LeftButton)
        OgitorsRoot::getSingletonPtr()->OnMouseLeftDown(pos1, getMouseButtons(evt->buttons(), evt->button()));
    else if(evt->button() == Qt::RightButton)
        OgitorsRoot::getSingletonPtr()->OnMouseRightDown(pos1, getMouseButtons(evt->buttons(), evt->button()));
    else if(evt->button() == Qt::MidButton)
        OgitorsRoot::getSingletonPtr()->OnMouseMiddleDown(pos1, getMouseButtons(evt->buttons(), evt->button()));

    OgreWidgetMouseMovedSincePress = false;
}
//------------------------------------------------------------------------------------
void OgreWidget::mouseReleaseEvent(QMouseEvent *evt)
{
    if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
        return;

    if(!hasFocus())
        setFocus();

    Ogre::Vector2 pos(evt->x(), evt->y());

    if(evt->button() == Qt::LeftButton)
        OgitorsRoot::getSingletonPtr()->OnMouseLeftUp(pos, getMouseButtons(evt->buttons(), evt->button()));
    else if(evt->button() == Qt::RightButton)
        OgitorsRoot::getSingletonPtr()->OnMouseRightUp(pos, getMouseButtons(evt->buttons(), evt->button()));
    else if(evt->button() == Qt::MidButton)
        OgitorsRoot::getSingletonPtr()->OnMouseMiddleUp(pos, getMouseButtons(evt->buttons(), evt->button()));

    if(!OgreWidgetMouseMovedSincePress && evt->button() == Qt::RightButton)
    {
        setContextMenuPolicy( Qt::PreventContextMenu );
        showObjectMenu();
    }
}
//------------------------------------------------------------------------------------
void OgreWidget::leaveEvent(QEvent *evt)
{
    if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
        return;

    Ogre::Vector2 pos(-1, -1);

    OgitorsRoot::getSingletonPtr()->OnMouseLeave(pos, 0);
    OgreWidgetMouseMovedSincePress = true;
}
//------------------------------------------------------------------------------------
void OgreWidget::wheelEvent(QWheelEvent *evt)
{
    if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
        return;

    if(!hasFocus())
        setFocus();

    Ogre::Vector2 pos(evt->x(), evt->y());

    OgitorsRoot::getSingletonPtr()->OnMouseWheel(pos, evt->delta(), getMouseButtons(evt->buttons(), Qt::NoButton));
}

//------------------------------------------------------------------------------------
QPaintEngine* OgreWidget::paintEngine() const
{
    // We don't want another paint engine to get in the way for our Ogre based paint engine.
    // So we return nothing.
    return NULL;
}

//------------------------------------------------------------------------------------
void OgreWidget::ProcessKeyActions()
{
    QTime time = QTime::currentTime();
    unsigned int curtime = time.hour() * 60 + time.minute();
    curtime = (curtime * 60) + time.second();
    curtime = (curtime * 1000) + time.msec();

    unsigned int iTimeDiff = curtime - mLastKeyEventTime;
    mLastKeyEventTime = curtime;

    if(OgitorsRoot::getSingletonPtr()->GetViewport())
        OgitorsRoot::getSingletonPtr()->GetViewport()->ProcessKeyActions(iTimeDiff);
}
//------------------------------------------------------------------------------------
void OgreWidget::displayFPS(float time)
{
    mFrameCounter++;
    mTotalFrameTime += time;
    mFrameRate = (float)mFrameCounter / (float)mTotalFrameTime ;
    if(mTotalFrameTime > 2.0f)
    {
        mTotalFrameTime = 0;
        mFrameCounter = 0;
        char temp[500];
        sprintf(temp,"Ogre FPS: %.1f  ", mFrameRate);
        mOgitorMainWindow->mFPSLabel->setText(QString(temp));
    }
}
//----------------------------------------------------------------------------------
void OgreWidget::processMaterialName(Ogre::Mesh *mesh, Ogre::String *name)
{
    Ogre::NameValuePairList *modelMaterialMap = OgitorsRoot::getSingletonPtr()->GetModelMaterialMap();
    modelMaterialMap->insert(Ogre::NameValuePairList::value_type(mesh->getName(), name->c_str()));
}
//-------------------------------------------------------------------------------------------
void OgreWidget::dragEnterEvent(QDragEnterEvent *evt)
{
    void *source = (void*)(evt->source());

    if(OgitorsRoot::getSingletonPtr()->OnDragEnter(source))
    {
        evt->setDropAction(Qt::IgnoreAction);
        evt->acceptProposedAction();

        setFocus();
        grabKeyboard();
    }
}
//-------------------------------------------------------------------------------------------
void OgreWidget::dragLeaveEvent(QDragLeaveEvent *evt)
{
    OgitorsRoot::getSingletonPtr()->OnDragLeave();
    releaseKeyboard();
}
//-------------------------------------------------------------------------------------------
void OgreWidget::dragMoveEvent(QDragMoveEvent *evt)
{
    void *source = (void*)(evt->source());
    unsigned int modifier = evt->keyboardModifiers();

    evt->setAccepted(OgitorsRoot::getSingletonPtr()->OnDragMove(source, modifier, evt->pos().x(), evt->pos().y()));
}
//-------------------------------------------------------------------------------------------
void OgreWidget::dropEvent(QDropEvent *evt)
{
    void *source = (void*)(evt->source());

    evt->setDropAction(Qt::IgnoreAction);

    OgitorsRoot::getSingletonPtr()->OnDragDropped(source, evt->pos().x(), evt->pos().y());

    releaseKeyboard();
}
//-------------------------------------------------------------------------------------------
void OgreWidget::showObjectMenu()
{
    CBaseEditor *e = 0;

    if(!OgitorsRoot::getSingletonPtr()->GetSelection()->isEmpty())
        e = OgitorsRoot::getSingletonPtr()->GetSelection()->getAsSingle();

    QMenu* ctxMenu = new QMenu(this);

    if(e != 0)
    {
        ctxMenu->setTitle(tr("Object Menu : ") + QString(e->getName().c_str()));
        QSignalMapper *signalMapper = 0;
        QSignalMapper *pasteSignalMapper = 0;

        ctxMenu->addAction(mOgitorMainWindow->actEditCopy);
        ctxMenu->addAction(mOgitorMainWindow->actEditCut);
        ctxMenu->addAction(mOgitorMainWindow->actEditDelete);
        ctxMenu->addAction(mOgitorMainWindow->actEditRename);
        ctxMenu->addSeparator();
        ctxMenu->addAction(mOgitorMainWindow->actEditCopyToTemplate);
        ctxMenu->addAction(mOgitorMainWindow->actEditCopyToTemplateWithChildren);

        UTFStringVector menuList;
        if(e->getObjectContextMenu(menuList))
        {
            UTFStringVector vList;
            int counter = 0;
            int mapslot = 0;
            signalMapper = new QSignalMapper(this);

            for(unsigned int i = 0;i < menuList.size();i++)
            {
                if(i == 0)
                    ctxMenu->addSeparator();

                OgitorsUtils::ParseUTFStringVector(menuList[i], vList);
                if(vList.size() > 0 && vList[0] != "")
                {
                    if(vList[0] == "-")
                    {
                        ctxMenu->addSeparator();
                        continue;
                    }

                    QAction *item = ctxMenu->addAction( ConvertToQString(vList[0]), signalMapper, SLOT(map()), 0);
                    if(vList.size() > 1)
                        item->setIcon(QIcon(ConvertToQString(vList[1])));
                    signalMapper->setMapping(item, mapslot);
                    counter++;
                }
                ++mapslot;
            }
            if(counter)
            {
                connect(signalMapper, SIGNAL(mapped( int )), this, SLOT(objectMenu( int )));
            }
        }
        ctxMenu->exec(QCursor::pos());
        delete signalMapper;
    }
    else
    {
        ctxMenu->addAction(mOgitorMainWindow->menuFile->menuAction());
        ctxMenu->addAction(mOgitorMainWindow->menuEdit->menuAction());
        ctxMenu->addAction(mOgitorMainWindow->menuView->menuAction());
        ctxMenu->addAction(mOgitorMainWindow->menuCamera->menuAction());
        ctxMenu->addAction(mOgitorMainWindow->menuTools->menuAction());
        ctxMenu->addAction(mOgitorMainWindow->menuTerrainTools->menuAction());
        ctxMenu->exec(QCursor::pos());
    }

    delete ctxMenu;
}
//-------------------------------------------------------------------------------------------
void OgreWidget::objectMenu(int id)
{
    if(!OgitorsRoot::getSingletonPtr()->GetSelection()->isEmpty())
        OgitorsRoot::getSingletonPtr()->GetSelection()->getAsSingle()->onObjectContextMenu(id);
}
//----------------------------------------------------------------------------------------
void OgreWidget::onSceneLoadStateChange(Ogitors::IEvent* evt)
{
    LoadStateChangeEvent *change_event = Ogitors::event_cast<LoadStateChangeEvent*>(evt);

    if(change_event)
    {
        //reload the zone selection widget when a scene is loaded
        LoadState state = change_event->getType();

        QString appTitle;
        appTitle = "qtOgitor ";
        appTitle += Ogitors::OGITOR_VERSION;

        if(state == LS_LOADED)
        {
            appTitle += QString(" - ") + QString(OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ProjectName.c_str()) + QString(".ofs");

            mOgitorMainWindow->setCameraPositions();

            CViewportEditor *ovp = OgitorsRoot::getSingletonPtr()->GetViewport();
            if(ovp)
            {
                ovp->SetEditorTool(TOOL_SELECT);
                mOgitorMainWindow->mCameraSpeedSlider->setValue(ovp->GetCameraSpeed());
                mOgitorMainWindow->mCameraSpeedSlider->setToolTip(QString("%1").arg((int)ovp->GetCameraSpeed()));
            }

            setAttribute(Qt::WA_PaintOnScreen, true);

            Ogre::Vector3 pos = OgitorsRoot::getSingletonPtr()->GetViewport()->getCameraEditor()->getCamera()->getDerivedPosition();
            char temp[128];
            sprintf(temp," X: % .2f, Y: % .2f, Z: % .2f", pos.x, pos.y, pos.z);
            QString camtext = QApplication::translate("MainWindow","Camera Position:") + QString(temp);
            mOgitorMainWindow->mCamPosLabel->setText(camtext);

            mOgitorMainWindow->getObjectsViewWidget()->prepareView();
            mOgitorMainWindow->getEntityViewWidget()->prepareView();
            mOgitorMainWindow->getTemplatesViewWidget()->prepareView();
            mOgitorMainWindow->getProjectFilesViewWidget()->prepareView();
            mOgitorMainWindow->toggleGrid();

            mOgitorMainWindow->getLayersViewWidget()->updateLayerNames();

            getOverlayWidget()->hide();

            mOgitorMainWindow->getAutoBackupTimer()->stop();

            // Check if backups are enabled, but the folder is not valid
            if(OgitorsRoot::getSingletonPtr()->GetProjectOptions()->AutoBackupEnabled == true)
            {
                QDir dir = QDir(OgitorsRoot::getSingletonPtr()->GetProjectOptions()->AutoBackupFolder.c_str());
                if(!dir.exists())
                {
                    QMessageBox::information(QApplication::activeWindow(), "Ogitor", tr("The specified auto backup directory does not exist.") + "\n" + tr("Auto backup will therefore be disabled."));
                    OgitorsRoot::getSingletonPtr()->GetProjectOptions()->AutoBackupEnabled = false;
                }
                else
                {
                    // Auto Backup Period in minutes
                    int aPeriod = 0;

                    // Adjust auto backup timer
                    if(OgitorsRoot::getSingletonPtr()->GetProjectOptions()->AutoBackupPeriodType == 0)
                        // Minutes -> value (in minutes) * 60 (to convert to seconds) * 1000 (to convert to milli-seconds)
                        aPeriod = OgitorsRoot::getSingletonPtr()->GetProjectOptions()->AutoBackupPeriod;
                    else if(OgitorsRoot::getSingletonPtr()->GetProjectOptions()->AutoBackupPeriodType == 1)
                        // Hours -> value (in hours) * 60 (to convert to minutes ) * 60 (to convert to seconds) * 1000 (to convert to milli-seconds)
                        aPeriod = OgitorsRoot::getSingletonPtr()->GetProjectOptions()->AutoBackupPeriod * 60;

                    if(aPeriod > 0)
                    {
                        mOgitorMainWindow->getAutoBackupTimer()->setInterval(aPeriod * 60 * 1000);
                        mOgitorMainWindow->getAutoBackupTimer()->start();
                    }
                    else
                    {
                        QMessageBox::information(QApplication::activeWindow(), "Ogitor", tr("Auto Backup Period is not valid.") + "\n" + tr("Auto backup will therefore be disabled."));                        
                        OgitorsRoot::getSingletonPtr()->GetProjectOptions()->AutoBackupEnabled = false;
                    }
                }
            }
        }
        else if(state == LS_UNLOADED)
        {
            // Don't hide on OSX since we always want the windows up
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE
            if(!isSwitchingScene())
                mOgitorMainWindow->hideSubWindows();

            setSwitchingScene(false);
#endif
            mOgitorMainWindow->getObjectsViewWidget()->clearView();
            mOgitorMainWindow->getEntityViewWidget()->clearView();
            mOgitorMainWindow->getTemplatesViewWidget()->destroyScene();
            mOgitorMainWindow->getProjectFilesViewWidget()->clearView();

            mOgitorMainWindow->mCamPosLabel->setText("");
            mOgitorMainWindow->mTriangleCountLabel->setText("");
            getOverlayWidget()->show();
            mOgitorMainWindow->getAutoBackupTimer()->stop();
        }
        else if(state == LS_LOADING)
        {
            mOgitorMainWindow->showSubWindows();
            repaint();
            return;
        }

        mOgitorMainWindow->updateLoadTerminateActions(state == LS_LOADED);
        mOgitorMainWindow->setWindowTitle(appTitle);
        setFocus();
        repaint();
    }
}
//------------------------------------------------------------------------------------

