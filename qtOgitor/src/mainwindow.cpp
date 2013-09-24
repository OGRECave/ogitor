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

#include "mainwindow.hxx"

#include "Ogitors.h"
#include "OgitorsScriptConsole.h"
#include "DefaultEvents.h"
#include "EventManager.h"

#include "ogrewidget.hxx"
#include "magickwidget.hxx"
#include "aboutdialog.hxx"
#include "sceneview.hxx"
#include "layerview.hxx"
#include "propertiesviewgeneral.hxx"
#include "propertiesviewcustom.hxx"
#include "entityview.hxx"
#include "objectsview.hxx"
#include "templateview.hxx"
#include "colourpicker.hxx"
#include "actiontoolbar.hxx"
#include "preferencesmanager.hxx"
#include "lineeditwithhistory.hxx"
#include "terraintoolswidget.hxx"
#include "generictexteditor.hxx"
#include "projectfilesview.hxx"
#include "genericimageeditor.hxx"

#include <QtCore/QProcess>
#include <QtCore/QTime>
#include <QtCore/QDir>
#include <QtCore/QDateTime>

#include <QtWidgets/QWidgetAction>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QStatusBar>

#if OGRE_MEMORY_TRACKER
#include "OgreMemoryTracker.h"
#endif

MainWindow *mOgitorMainWindow = 0;
int FPSLIST[12] = {1000,200,100,50,33,25,20,10,5,3,2,1};

extern QString ConvertToQString(Ogre::UTFString& value);

using namespace Ogitors;

struct LogData
{
    int     mLevel;
    QString mMessage;
};

typedef std::vector<LogData> LogDataVector;

class LogBuffer
{
public:
    LogBuffer() {mData.clear();}
    ~LogBuffer() {};
    void getBuffer(LogDataVector& buffer)
    {
        OGRE_LOCK_AUTO_MUTEX;
            buffer = mData;
        mData.clear();
    };
    void append(int lvl, const Ogre::String& msg)
    {
        OGRE_LOCK_AUTO_MUTEX;
            if(mOgitorMainWindow && mOgitorMainWindow->getOgreWidget() && mOgitorMainWindow->getOgreWidget()->isSizing())
                return;

        int level = lvl;
        if(level != 3)
        {
            if((msg.find("WARNING") != -1) || (msg.find("Compiler error") != -1))
                level = 4;
        }
        LogData data;
        data.mLevel = level;
        data.mMessage = msg.c_str();
        mData.push_back(data);
    };
private:
    OGRE_AUTO_MUTEX;
        LogDataVector mData;
};

OgitorAssistant::OgitorAssistant()
: proc(0)
{
}

OgitorAssistant::~OgitorAssistant()
{
    if (proc && proc->state() == QProcess::Running) {
        proc->terminate();
        proc->waitForFinished(3000);
    }
    delete proc;
}

void OgitorAssistant::showDocumentation(const QString &page)
{
    if (!startOgitorAssistant())
        return;

    QByteArray ba("SetSource ");
    ba.append("qthelp://Ogitor/doc/");

    proc->write(ba + page.toLocal8Bit() + '\0');
}

bool OgitorAssistant::startOgitorAssistant()
{
    if (!proc)
        proc = new QProcess();

    if (proc->state() != QProcess::Running) {

#if !defined(Q_OS_WIN)
        QString app = QLibraryInfo::location(QLibraryInfo::BinariesPath) + QDir::separator();
#else
        // we load our own assistant in Windows
        QString app = QString(Ogitors::OgitorsUtils::GetExePath().c_str()) + QDir::separator();
#endif

#if defined(Q_OS_MAC)
        app += QLatin1String("Assistant.app/Contents/MacOS/Assistant");
#else
        app += QLatin1String("assistant");
#endif

        QStringList args;
#if defined(Q_OS_LINUX)
        args << QLatin1String("-collectionFile")
            << QLatin1String(std::string(Ogitors::Globals::RESOURCE_PATH).c_str())
            + QLatin1String("/Help/collection_ogitor.qhc")
            << QLatin1String("-enableRemoteControl");
#else
        args << QLatin1String("-collectionFile")
            << QLatin1String(Ogitors::OgitorsUtils::GetExePath().c_str())
            + QLatin1String("/Help/collection_ogitor.qhc")
            << QLatin1String("-enableRemoteControl");
#endif

        proc->start(app, args);

        if (!proc->waitForStarted()) {
            QMessageBox::critical(0, QObject::tr("Ogitor Help System"),
                QObject::tr("Unable to launch Qt Assistant (%1)").arg(app));
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
MainWindow::MainWindow(QString args, QWidget *parent)
: QMainWindow(parent), mOgreWidget(0), mLastTime(0), mArgsFile(args), mHasFileArgs(false), mLastLoadedScene(""), mPrefManager(0), mSubWindowsVisible(true)
{
    mOgitorMainWindow = this;

    mPlayerToolbar = 0;
    actPlayerRunPause = 0;
    actPlayerStop = 0;
    actAddScriptToToolbar = 0;

    mTargetRenderCount = 30;

    recentMapper = 0;

    setMinimumSize(400,300);

    if(!mArgsFile.isEmpty())
    {
        mHasFileArgs = true;
    }

    QIcon icon;
    icon.addPixmap(QPixmap(":/icons/qtOgitor.png"), QIcon::Normal, QIcon::Off);
    setWindowIcon(icon);

    if(objectName().isEmpty())
        setObjectName(QString::fromUtf8("this"));
    resize(1024, 768);

    setDockNestingEnabled(true);

    addDockWidgets(this);

    setupLog();

    addActions();

    addMenus();

    setupStatusBar();

    createHomeTab();
    //createMacHomeTab();

    createScriptActionsToolbar();

    createPlayerToolbar();

    createCustomToolbars();

    mTerrainToolsWidget = new TerrainToolsWidget(parent);

    OgitorsRoot::getSingletonPtr()->GetEditorObjectFactory("Terrain Page")->setCustomToolsWindow(mTerrainToolsWidget);
    OgitorsRoot::getSingletonPtr()->GetEditorObjectFactory("Terrain Group")->setCustomToolsWindow(mTerrainToolsWidget);

    retranslateUi();

    mEditorTab = new QTabWidget(this);
    mEditorTab->setObjectName("editortab");

    createSceneRenderWindow();

    QMainWindow *mw = new QMainWindow(mEditorTab);
    mGenericTextEditor = new GenericTextEditor("GenericTextEditor", mw);
    mGenericTextEditor->setAllowDoubleDisplay(false);
    mEditorTab->addTab(mw, tr("Text Editor"));
    mw->setCentralWidget(mGenericTextEditor);

    mw = new QMainWindow(mEditorTab);
    mGenericImageEditor = new GenericImageEditor("GenericImageEditor", mw);
    mGenericImageEditor->setAllowDoubleDisplay(false);
    mEditorTab->addTab(mw, tr("Image Viewer"));
    mw->setCentralWidget(mGenericImageEditor);

    createCustomTabWindows();

    setCentralWidget(mEditorTab);

    updateLoadTerminateActions(false);

    QtOgitorSystem *system = static_cast<QtOgitorSystem*>(OgitorsSystem::getSingletonPtr());
    system->SetWindows(mOgreWidget, mSceneViewWidget, mLayerViewWidget, mGeneralPropertiesViewWidget, mCustomPropertiesViewWidget);
    system->InitTreeIcons();

    mTimer = new QTimer(this);
    mTimer->setInterval(0);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(timerLoop()));
    mTimer->start();

    mAutoBackupTimer = new QTimer(this);
    connect(mAutoBackupTimer, SIGNAL(timeout()), this, SLOT(autoSaveScene()));
    mAutoBackupTimer->stop();

    mPrefManager = new PreferencesManager(this);

    mOgitorAssistant = new OgitorAssistant;

    initHiddenRenderWindow();

    mApplicationObject = 0;

    readSettings();
    // Looks better on OSX to always be shown
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE
    hideSubWindows();
#endif

    onSwitchStackedResources(0);

    EventManager::getSingletonPtr()->connectEvent(EventManager::MODIFIED_STATE_CHANGE,      this, true, 0, true, 0, EVENT_CALLBACK(MainWindow, onSceneModifiedChange));
    EventManager::getSingletonPtr()->connectEvent(EventManager::UNDOMANAGER_NOTIFICATION,   this, true, 0, true, 0, EVENT_CALLBACK(MainWindow, onUndoManagerNotification));
    EventManager::getSingletonPtr()->connectEvent(EventManager::RUN_STATE_CHANGE,           this, true, 0, true, 0, EVENT_CALLBACK(MainWindow, onSceneRunStateChange));
    EventManager::getSingletonPtr()->connectEvent(EventManager::EDITOR_TOOL_CHANGE,         this, true, 0, true, 0, EVENT_CALLBACK(MainWindow, onSceneEditorToolChange));
    EventManager::getSingletonPtr()->connectEvent(EventManager::TERRAIN_EDITOR_CHANGE,      this, true, 0, true, 0, EVENT_CALLBACK(MainWindow, onTerrainEditorChange));
}
//------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    EventManager::getSingletonPtr()->disconnectEvent(EventManager::MODIFIED_STATE_CHANGE,       this);
    EventManager::getSingletonPtr()->disconnectEvent(EventManager::UNDOMANAGER_NOTIFICATION,    this);
    EventManager::getSingletonPtr()->disconnectEvent(EventManager::RUN_STATE_CHANGE,            this);
    EventManager::getSingletonPtr()->disconnectEvent(EventManager::EDITOR_TOOL_CHANGE,          this);
    EventManager::getSingletonPtr()->disconnectEvent(EventManager::TERRAIN_EDITOR_CHANGE,       this);

    delete mTerrainToolsWidget;
    delete mTimer;
}
//------------------------------------------------------------------------------
void MainWindow::setApplicationObject(QObject *obj)
{
    mApplicationObject = obj;
    mAppActive = true;
    obj->installEventFilter(this);
}
//------------------------------------------------------------------------------
void MainWindow::initHiddenRenderWindow()
{
    Ogre::NameValuePairList hiddenParams;

#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
    hiddenParams["externalWindowHandle"] = Ogre::StringConverter::toString((size_t) winId());
#else
#if QT_VERSION < 0x050000
    const QX11Info info = this->x11Info();
    Ogre::String winHandle;
    winHandle  = Ogre::StringConverter::toString((unsigned long)(info.display()));
    winHandle += ":";
    winHandle += Ogre::StringConverter::toString((unsigned int)(info.screen()));
    winHandle += ":";
    winHandle += Ogre::StringConverter::toString((unsigned long)(this->winId()));
    winHandle += ":";
    winHandle += Ogre::StringConverter::toString((unsigned long)(info.visual()));

    hiddenParams["externalWindowHandle"] = winHandle;

#elif QT_VERSION >= 0x050100
    const QX11Info info = this->x11Info();
    Ogre::String winHandle;
    winHandle  = Ogre::StringConverter::toString((unsigned long)(info.display()));
    winHandle += ":";
    winHandle += Ogre::StringConverter::toString((unsigned int)(info.appScreen()));
    winHandle += ":";
    winHandle += Ogre::StringConverter::toString((unsigned long)(this->winId()));

    hiddenParams["externalWindowHandle"] = winHandle;
#else // only for the time between Qt 5.0 and Qt 5.1 when QX11Info was not included
    hiddenParams["externalWindowHandle"] = Ogre::StringConverter::toString((unsigned long)(this->winId()));
#endif
#endif

#if defined(Q_OS_MAC)
    hiddenParams["macAPICocoaUseNSView"] = "true";
    hiddenParams["macAPI"] = "cocoa";
#endif

    hiddenParams["border"] = "none";
    Ogre::RenderWindow* pPrimary = Ogre::Root::getSingletonPtr()->createRenderWindow("Primary1", 1, 1, false, &hiddenParams);
    pPrimary->setVisible(false);
    pPrimary->setAutoUpdated(false);

    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Load resources
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//------------------------------------------------------------------------------------
void MainWindow::showSubWindows()
{
#ifdef OPT_SHOW_HIDE_WINDOWS

    if(mSubWindowsVisible)
        return;

    mEditorTab->setVisible(true);

    for(unsigned int i = 0;i < mSubWindowStateSave.size();i++)
    {
        mSubWindowStateSave[i]->setVisible(true);
    }
    mSubWindowStateSave.clear();

    mMenuBar->insertAction(menuHelp->menuAction(), menuEdit->menuAction());
    mMenuBar->insertAction(menuHelp->menuAction(), menuView->menuAction());
    mMenuBar->insertAction(menuHelp->menuAction(), menuLog->menuAction());
    mMenuBar->insertAction(menuHelp->menuAction(), menuCamera->menuAction());
    mMenuBar->insertAction(menuHelp->menuAction(), menuTools->menuAction());
    mMenuBar->insertAction(menuHelp->menuAction(), menuTerrainTools->menuAction());

    actFullScreen->setEnabled(true);
    actSuperFullScreen->setEnabled(true);

    mCamPosToolBar->setVisible(true);
    mStatusViewToolBar->setVisible(true);
    mStatusShowHideToolBar->setVisible(true);

    mSubWindowsVisible = true;
#endif
}
//------------------------------------------------------------------------------
void MainWindow::hideSubWindows()
{
#ifdef OPT_SHOW_HIDE_WINDOWS

    if(!mSubWindowsVisible)
        return;

    mSubWindowStateSave.clear();

    const QObjectList& childwins = children();
    for(int i = 0;i < childwins.size();i++)
    {
        if(childwins[i]->isWidgetType())
        {
            QWidget *widget = static_cast<QWidget*>(childwins[i]);
            QString objName = widget->objectName();
            // Check if the widget is a dockwidget and if its not already defined as hidden
            // Qt defines widget attribute WA_WState_Hidden if a widget will be created but not displayed on screen
            // restoreLayout also restores this attribute, so we get the correct state,
            // testing this attribute instead of isVisible() since isVisible will always
            // incorrectly return false when the window is first created but not updated yet...
            if(objName.endsWith("DockWidget", Qt::CaseInsensitive) && !widget->testAttribute(Qt::WA_WState_Hidden))
            {
                widget->setVisible(false);
                mSubWindowStateSave.push_back(widget);
            }
        }
    }

    mEditorTab->setVisible(false);

    mMenuBar->removeAction(menuTools->menuAction());
    mMenuBar->removeAction(menuEdit->menuAction());
    mMenuBar->removeAction(menuCamera->menuAction());
    mMenuBar->removeAction(menuView->menuAction());
    mMenuBar->removeAction(menuLog->menuAction());
    mMenuBar->removeAction(menuTerrainTools->menuAction());

    actFullScreen->setEnabled(false);
    actSuperFullScreen->setEnabled(false);

    mCamPosToolBar->setVisible(false);
    mStatusViewToolBar->setVisible(false);
    mStatusShowHideToolBar->setVisible(false);

    mSubWindowsVisible = false;
#endif
}
//------------------------------------------------------------------------------
void MainWindow::readSettings(QString filename)
{
    QSettings *settings;
    bool invalid_win_rect = false;

    if(filename.isEmpty())
    {
        settings = new QSettings();
    }
    else
        settings = new QSettings(filename, QSettings::IniFormat);

    settings->beginGroup("session");
    restoreState(settings->value("Layout").toByteArray());
    bool maximized = settings->value("MainWindowMaximized", false).toBool();
    QRect rect = settings->value("MainWindowGeometry").toRect();
    if(rect.isEmpty())
        invalid_win_rect = true;
    settings->endGroup();

    settings->beginGroup("messagefilters");
    actLogShowWarnings->setChecked(settings->value("FilterWarnings", 1).toBool());
    actLogShowErrors->setChecked(settings->value("FilterErrors", 1).toBool());
    actLogShowInfo->setChecked(settings->value("FilterInfo", 1).toBool());
    actLogShowDebug->setChecked(settings->value("FilterDebug", 1).toBool());
    settings->endGroup();
    toggleLogMessages();

    if(maximized)
        setWindowState(Qt::WindowMaximized);
    else
    {
        move(rect.topLeft());
        resize(rect.size());
    }

    delete settings;

    if(invalid_win_rect)
        return readSettings(":/layouts/initial.oglayout");
}
//------------------------------------------------------------------------------
void MainWindow::writeSettings(QString filename)
{
    QSettings *settings;

    if(filename.isEmpty())
        settings = new QSettings();
    else
        settings = new QSettings(filename, QSettings::IniFormat);

    settings->beginGroup("session");
    settings->setValue("MainWindowGeometry", geometry());

    bool maximized = isMaximized();
    settings->remove("Layout");
    settings->setValue("Layout", saveState());
    settings->setValue("MainWindowMaximized", maximized);
    settings->endGroup();
    settings->beginGroup("messagefilters");
    settings->setValue("FilterWarnings", actLogShowWarnings->isChecked());
    settings->setValue("FilterErrors", actLogShowErrors->isChecked());
    settings->setValue("FilterInfo", actLogShowInfo->isChecked());
    settings->setValue("FilterDebug", actLogShowDebug->isChecked());
    settings->endGroup();
    settings->setValue("preferences/lastLoadedScene", mLastLoadedScene.c_str());

    delete settings;
}
//------------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
    {
        mLastLoadedScene = OgitorsRoot::getSingletonPtr()->GetSceneName();
    }

    if(OgitorsRoot::getSingletonPtr()->TerminateScene())
    {
        delete mOgitorAssistant;

        Ogre::LogManager::getSingleton().getDefaultLog()->removeListener(this);

        if(actFullScreen->isChecked())
        {
            actFullScreen->setChecked(false);
            toggleFullScreen();
        }
        else if(actSuperFullScreen->isChecked())
        {
            actSuperFullScreen->setChecked(false);
            toggleSuperFullScreen();
        }

        showSubWindows();

        writeSettings();

        event->setAccepted(true);
    }
    else
    {
        event->setAccepted(false);
        return;
    }
}
//------------------------------------------------------------------------------
void MainWindow::retranslateUi()
{
    QString appTitle = "qtOgitor ";
    appTitle += Ogitors::Globals::OGITOR_VERSION.c_str();
    setWindowTitle(appTitle);
    mExplorerDockWidget->setWindowTitle(QApplication::translate("MainWindow", "Explorer", 0));
    mLayerDockWidget->setWindowTitle(QApplication::translate("MainWindow", "Groups", 0));
    mResourcesDockWidget->setWindowTitle(QApplication::translate("MainWindow", "Resources", 0));
    mPropertiesDockWidget->setWindowTitle(QApplication::translate("MainWindow", "Properties", 0));
    mToolsDockWidget->setWindowTitle(QApplication::translate("MainWindow", "Tools", 0));
    mProjectFilesDockWidget->setWindowTitle(QApplication::translate("MainWindow", "Files", 0));
    menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
}
//------------------------------------------------------------------------------
void MainWindow::addDockWidgets(QMainWindow* parent)
{
    mSceneViewWidget = new SceneViewWidget(parent);

    mExplorerToolBox = new QToolBox(parent);
    mExplorerToolBox->addItem(mSceneViewWidget, QIcon(":/icons/scene.svg"), tr("Scene"));

    mExplorerDockWidget = new QDockWidget(parent);
    mExplorerDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    mExplorerDockWidget->setObjectName(QString::fromUtf8("explorerDockWidget"));
    mExplorerDockWidget->setWidget(mExplorerToolBox);
    parent->addDockWidget(static_cast<Qt::DockWidgetArea>(1), mExplorerDockWidget);

    mLayerViewWidget = new LayerViewWidget(parent);
    mLayerDockWidget = new QDockWidget(parent);
    mLayerDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    mLayerDockWidget->setObjectName(QString::fromUtf8("layerDockWidget"));
    mLayerDockWidget->setWidget(mLayerViewWidget);
    parent->addDockWidget(static_cast<Qt::DockWidgetArea>(1), mLayerDockWidget);

    mGeneralPropertiesViewWidget = new GeneralPropertiesViewWidget(parent);
    mCustomPropertiesViewWidget = new CustomPropertiesViewWidget(parent);

    mPropertiesToolBox = new QToolBox(parent);
    mPropertiesToolBox->addItem(mGeneralPropertiesViewWidget, QIcon(":/icons/properties.svg"), tr("General") + " " + tr("Properties"));
    mPropertiesToolBox->addItem(mCustomPropertiesViewWidget, QIcon(":/icons/properties.svg"), tr("Custom") + " " + tr("Properties"));

    mPropertiesDockWidget = new QDockWidget(parent);
    mPropertiesDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    mPropertiesDockWidget->setObjectName(QString::fromUtf8("propertiesDockWidget"));
    mPropertiesDockWidget->setWidget(mPropertiesToolBox);
    parent->addDockWidget(static_cast<Qt::DockWidgetArea>(2), mPropertiesDockWidget);

    mObjectsViewWidget = new ObjectsViewWidget(parent);
    mEntityViewWidget = new EntityViewWidget(parent);
    mEntityViewWidget->setObjectName(QString::fromUtf8("entityViewWidget"));
    mTemplatesViewWidget = new TemplateViewWidget(parent);
    mProjectFilesViewWidget = new ProjectFilesViewWidget(parent);

    mResourcesDockWidget = new QDockWidget(parent);
    mResourcesDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    mResourcesDockWidget->setObjectName(QString::fromUtf8("resourcesDockWidget"));
    parent->addDockWidget(static_cast<Qt::DockWidgetArea>(2), mResourcesDockWidget);

    QMainWindow* stackedWidgetInnerDummy = new QMainWindow(mResourcesDockWidget);
    stackedWidgetInnerDummy->setWindowFlags(Qt::Widget); 

    mResourcesStackedWidget = new QStackedWidget(parent);
    mResourcesStackedWidget->addWidget(mObjectsViewWidget);
    mResourcesStackedWidget->addWidget(mEntityViewWidget);
    mResourcesStackedWidget->addWidget(mTemplatesViewWidget);

    mResourcesStackedToolBar = new QToolBar(stackedWidgetInnerDummy);
    mResourcesStackedToolBar->setMovable(false);
    mResourcesStackedToolBar->setIconSize(QSize(16, 16));
    mResourcesStackedToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    QAction *stackedAction;
    mResourcesStackedMapper = new QSignalMapper(parent);
    mResourcesStackedActions = new QActionGroup(this);
    stackedAction = mResourcesStackedActions->addAction(QIcon(":/icons/objects.svg"), tr("Objects"));
    stackedAction->setCheckable(true);
    stackedAction->setChecked(true);
    connect(stackedAction,  SIGNAL(triggered()), mResourcesStackedMapper, SLOT(map()));
    mResourcesStackedMapper->setMapping(stackedAction,  0);
    stackedAction = mResourcesStackedActions->addAction(QIcon(":/icons/entity.svg"), tr("Meshes"));
    stackedAction->setCheckable(true);
    connect(stackedAction,  SIGNAL(triggered()), mResourcesStackedMapper, SLOT(map()));
    mResourcesStackedMapper->setMapping(stackedAction,  1);
    stackedAction = mResourcesStackedActions->addAction(QIcon(":/icons/template.svg"), tr("Templates"));
    stackedAction->setCheckable(true);
    connect(stackedAction,  SIGNAL(triggered()), mResourcesStackedMapper, SLOT(map()));
    mResourcesStackedMapper->setMapping(stackedAction,  2);

    connect(mResourcesStackedMapper, SIGNAL(mapped(int)), this, SLOT(onSwitchStackedResources(int)));

    mResourcesStackedToolBar->addActions(mResourcesStackedActions->actions());

    stackedWidgetInnerDummy->addToolBar(Qt::BottomToolBarArea, mResourcesStackedToolBar);
    stackedWidgetInnerDummy->setCentralWidget(mResourcesStackedWidget);
    mResourcesDockWidget->setWidget(stackedWidgetInnerDummy);

    mToolsDockWidget = new QDockWidget(parent);
    mToolsDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    mToolsDockWidget->setObjectName(QString::fromUtf8("toolsDockWidget"));
    parent->addDockWidget(static_cast<Qt::DockWidgetArea>(2), mToolsDockWidget);

    mProjectFilesDockWidget = new QDockWidget(parent);
    mProjectFilesDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    mProjectFilesDockWidget->setObjectName(QString::fromUtf8("projectFilesDockWidget"));
    mProjectFilesDockWidget->setWidget(mProjectFilesViewWidget);
    parent->addDockWidget(static_cast<Qt::DockWidgetArea>(1), mProjectFilesDockWidget);

	connect( mProjectFilesDockWidget, SIGNAL( visibilityChanged(bool) ), this, SLOT( onProjectFilesVisibilityChanged(bool) ));

    parent->tabifyDockWidget(mPropertiesDockWidget, mResourcesDockWidget);
    parent->tabifyDockWidget(mPropertiesDockWidget, mToolsDockWidget);
    parent->tabifyDockWidget(mExplorerDockWidget, mLayerDockWidget);
    parent->tabifyDockWidget(mExplorerDockWidget, mProjectFilesDockWidget);
    mPropertiesDockWidget->raise();
    mExplorerDockWidget->raise();
    QSizePolicy size_pol;
    size_pol.setVerticalPolicy(QSizePolicy::Expanding);

    createCustomDockWidgets();

    const QObjectList& childwins = children();
    for(int i = 0;i < childwins.size();i++)
    {
        QString classname = childwins[i]->metaObject()->className();
        if(classname == "QTabBar")
        {
            QTabBar *widget = static_cast<QTabBar*>(childwins[i]);
            widget->setUsesScrollButtons(true);
            widget->setElideMode(Qt::ElideRight);
        }
    }
}
//------------------------------------------------------------------------------
void MainWindow::createSceneRenderWindow()
{
    QWidget *renderWindowWidget = new QWidget(mEditorTab);

    mCameraViewModeBox = new QComboBox();
    mCameraViewModeBox->setFixedSize(QSize(80, 20));
    mCameraViewModeBox->addItem("FREE");
    mCameraViewModeBox->addItem("FIXED");
    mCameraViewModeBox->addItem("G_LEFT");
    mCameraViewModeBox->addItem("G_RIGHT");
    mCameraViewModeBox->addItem("G_FRONT");
    mCameraViewModeBox->addItem("G_BACK");
    mCameraViewModeBox->addItem("G_TOP");
    mCameraViewModeBox->addItem("G_BOTTOM");
    mCameraViewModeBox->addItem("L_LEFT");
    mCameraViewModeBox->addItem("L_RIGHT");
    mCameraViewModeBox->addItem("L_FRONT");
    mCameraViewModeBox->addItem("L_BACK");
    mCameraViewModeBox->addItem("L_TOP");
    mCameraViewModeBox->addItem("L_BOTTOM");
    mCameraViewModeBox->setCurrentIndex(0);
    mCameraViewModeBox->setMaxVisibleItems(15);

    mSnapMultiplierBox = new QComboBox();
    mSnapMultiplierBox->setMaxVisibleItems(16);

    QString test_str("x99");

    QFontMetrics fm(mSnapMultiplierBox->font());
    int text_width = fm.width(test_str) + 28;
    mSnapMultiplierBox->setFixedSize(text_width, 20 );
    for(unsigned int s = 1;s < 17;s++)
        mSnapMultiplierBox->addItem(QString("x") + QString("%1").arg(s));
    mSnapMultiplierBox->setCurrentIndex(0);

    QToolBar *renderWindowToolBar = new QToolBar(renderWindowWidget);
    renderWindowToolBar->setObjectName("renderwindowtoolbar");
    renderWindowToolBar->setIconSize(QSize(20,20));
    renderWindowToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    renderWindowToolBar->addAction(actToggleWalkAround);
    renderWindowToolBar->addSeparator();
    renderWindowToolBar->addAction(actToggleWorldSpaceGizmo);
    renderWindowToolBar->addSeparator();
    renderWindowToolBar->addAction(actSelect);
    renderWindowToolBar->addAction(actMove);
    renderWindowToolBar->addAction(actRotate);
    renderWindowToolBar->addAction(actScale);
    renderWindowToolBar->addSeparator();

    QLabel *snapLabel = new QLabel(QString("  ") + tr("Snap:"));
    renderWindowToolBar->addWidget(snapLabel);
    renderWindowToolBar->addWidget(mSnapMultiplierBox);
    renderWindowToolBar->addSeparator();

    QSignalMapper *cameraMapper = new QSignalMapper(this);
    menuCameraPositionMain = new QMenu(this);
    menuCameraPositionMain->setIcon(QIcon(":/icons/camera.svg"));
    menuCameraPositionMain->addAction(actCamSave);

    mCameraSpeedSlider = new QSlider(Qt::Horizontal);
    mCameraSpeedSlider->setRange(1, 100);
    mCameraSpeedSlider->setTickInterval(5);
    mCameraSpeedSlider->setTickPosition(QSlider::TicksBelow);
    mCameraSpeedSlider->setMaximumWidth(100);

    QWidgetAction *sliderActionWidget = new QWidgetAction( this );
    sliderActionWidget->setDefaultWidget(mCameraSpeedSlider);
    sliderActionWidget->setText(tr("Camera Speed"));
    menuCameraPositionMain->addAction(sliderActionWidget);

    menuCameraPositionMain->addSeparator();
    for(unsigned int i = 0;i < 10;i++)
    {
        menuCameraPositions[i] = menuCameraPositionMain->addMenu(tr("(Empty Slot)"));
        QAction *actgo = menuCameraPositions[i]->addAction(tr("Select"), cameraMapper, SLOT(map()), 0);
        QAction *actremove = menuCameraPositions[i]->addAction(tr("Remove"), cameraMapper, SLOT(map()), 0);
        cameraMapper->setMapping(actgo, i);
        cameraMapper->setMapping(actremove, i + 100);
    }
    connect(cameraMapper, SIGNAL(mapped( int )), this, SLOT(cameraAction( int )));

    renderWindowToolBar->addAction(menuCamPolyMode->menuAction());
    renderWindowToolBar->addAction(menuCameraPositionMain->menuAction());
    renderWindowToolBar->addAction(actCamSpeedPlus);
    renderWindowToolBar->addAction(actCamSpeedMinus);
    renderWindowToolBar->addSeparator();

    QLabel *viewModeLabel = new QLabel(QString("  ") + tr("View :"));
    renderWindowToolBar->addWidget(viewModeLabel);
    renderWindowToolBar->addWidget(mCameraViewModeBox);

    mSnapGround = new QCheckBox(tr("Always Snap Ground"));
    renderWindowToolBar->addSeparator();
    renderWindowToolBar->addWidget(mSnapGround);

    mOgreWidget = new OgreWidget(renderWindowWidget, mHasFileArgs);

    QVBoxLayout *renderWindowLayout = new QVBoxLayout();
    renderWindowLayout->setSpacing(0);
    renderWindowLayout->setMargin(0);
    renderWindowLayout->addWidget(renderWindowToolBar, 0);
    renderWindowLayout->addWidget(mOgreWidget, 1);

    renderWindowWidget->setLayout(renderWindowLayout);

    mEditorTab->addTab(renderWindowWidget, tr("Render"));

    connect(mSnapMultiplierBox, SIGNAL( currentIndexChanged( int )), this, SLOT( snapMultiplierIndexChanged( int )));
    connect(mCameraViewModeBox, SIGNAL( currentIndexChanged( int )), this, SLOT( viewModeIndexChanged( int )));
    connect(mCameraSpeedSlider, SIGNAL( valueChanged( int )), this, SLOT( cameraSpeedValueChanged( int )));
    connect(mSnapGround, SIGNAL( stateChanged( int )), this, SLOT( snapGroundChanged( int )));
}
//------------------------------------------------------------------------------
void MainWindow::createHomeTab()
{
    mFileToolBar = new QToolBar(tr("File"));
    mFileToolBar->setObjectName("FileToolBar");
    mFileToolBar->setIconSize(QSize(24,24));
    mFileToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    mFileToolBar->addAction(actNew);
    mFileToolBar->addAction(actOpen);
    mFileToolBar->addAction(actSave);
    mFileToolBar->addAction(actSaveAs);
    mFileToolBar->addAction(actClose);

    mEditToolBar = new QToolBar(tr("Edit"));
    mEditToolBar->setObjectName("EditToolBar");
    mEditToolBar->setIconSize(QSize(24,24));
    mEditToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    mEditToolBar->addAction(actUndo);
    mEditToolBar->addAction(actRedo);
    mEditToolBar->addSeparator();
    mEditToolBar->addAction(actSceneOptions);
    mEditToolBar->addAction(actOpenPreferences);

    mHelpToolBar = new QToolBar(tr("Help"));
    mHelpToolBar->setObjectName("HelpToolBar");
    mHelpToolBar->setIconSize(QSize(24,24));
    mHelpToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    mHelpToolBar->addAction(actHelp);
    mHelpToolBar->addAction(actAbout);

    addToolBar(Qt::TopToolBarArea, mFileToolBar);
    addToolBar(Qt::TopToolBarArea, mEditToolBar);
    addToolBar(Qt::TopToolBarArea, mHelpToolBar);
}
//------------------------------------------------------------------------------
void MainWindow::createScriptActionsToolbar()
{
    actAddScriptToToolbar = new QAction(tr("Add Script Action"), this);
    actAddScriptToToolbar->setStatusTip(tr("Create a toolbar action that executes a script"));
    actAddScriptToToolbar->setIcon( QIcon( ":/icons/additional.svg" ));

    mScriptActionsBar = new ActionToolbar(tr("Script Actions"));
    mScriptActionsBar->setObjectName("ScriptActionsToolBar");
    mScriptActionsBar->setIconSize(QSize(24,24));
    mScriptActionsBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    mScriptActionsBar->addAction(actAddScriptToToolbar);
    mScriptActionsBar->addSeparator();

    connect(actAddScriptToToolbar, SIGNAL(triggered()), this, SLOT(onAddScriptAction()));

    addToolBar(Qt::TopToolBarArea, mScriptActionsBar);

    mScriptActionMap = new QSignalMapper(this);

    connect(mScriptActionMap, SIGNAL(mapped(int)), this, SLOT(onExecuteScriptAction(int)));

    QSettings settings;

    settings.beginGroup("ScriptActions");
    int count = settings.value("ActionCount",(int)0).toInt();

    mScriptActions.clear();

    for(int i = 0;i < count;i++)
    {
        _addScriptAction(settings.value(QString("IconPath%1").arg(i), "").toString(), settings.value(QString("ScriptPath%1").arg(i), "").toString());
    }

    settings.endGroup();
}
//------------------------------------------------------------------------------
void MainWindow::createPlayerToolbar()
{
    actPlayerRunPause = new QAction(tr("Run Scene Scripts"), this);
    actPlayerRunPause->setStatusTip(tr("Run scripts in the scene"));
    actPlayerRunPause->setIcon( QIcon( ":/icons/player_play.svg" ));
    actPlayerRunPause->setEnabled(false);

    actPlayerStop = new QAction(tr("Stop Scene Scripts"), this);
    actPlayerStop->setStatusTip(tr("Stop scripts in the scene"));
    actPlayerStop->setIcon( QIcon( ":/icons/player_stop.svg" ));
    actPlayerStop->setEnabled(false);

    mPlayerToolbar = new QToolBar(tr("Scene Player"));
    mPlayerToolbar->setObjectName("PlayerToolBar");
    mPlayerToolbar->setIconSize(QSize(24,24));
    mPlayerToolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    mPlayerToolbar->addAction(actPlayerRunPause);
    mPlayerToolbar->addAction(actPlayerStop);

    addToolBar(Qt::TopToolBarArea, mPlayerToolbar);

    connect(actPlayerRunPause, SIGNAL(triggered()), this, SLOT(onPlayerRunPause()));
    connect(actPlayerStop, SIGNAL(triggered()), this, SLOT(onPlayerStop()));
}
//------------------------------------------------------------------------------
void MainWindow::createCustomToolbars()
{
    std::vector<void*> toolbars = OgitorsRoot::getSingletonPtr()->GetToolBars();
    for(unsigned int i = 0;i < toolbars.size();i++)
    {
        QToolBar *widget = static_cast<QToolBar*>(toolbars[i]);
        if(widget)
        {
            try
            {
                widget->setIconSize(QSize(24,24));
                widget->setToolButtonStyle(Qt::ToolButtonIconOnly);
                addToolBar(Qt::TopToolBarArea, widget);
            }
            catch(...)
            {
                QMessageBox::warning(QApplication::activeWindow(), "qtOgitor", QString(tr("Invalid Toolbar ID:%1")).arg(QString::number(i)), QMessageBox::Ok);
            }
        }
    }
}
//------------------------------------------------------------------------------
void MainWindow::createCustomDockWidgets()
{
    Ogitors::DockWidgetDataList dockwidgets = OgitorsRoot::getSingletonPtr()->GetDockWidgets();

    for(unsigned int i = 0;i < dockwidgets.size();i++)
    {
        if(dockwidgets[i].mParent == Ogitors::DOCK_MAIN)
        {
            QDockWidget *widget = static_cast<QDockWidget*>(dockwidgets[i].mHandle);
            if(widget)
            {
                try
                {
                    widget->setObjectName(QString(dockwidgets[i].mCaption.c_str()));
                    widget->setWindowTitle(QString(dockwidgets[i].mCaption.c_str()));
                    addDockWidget(Qt::RightDockWidgetArea, widget);
                    QAction *toggleAction = widget->toggleViewAction();

                    if(dockwidgets[i].mIcon != "")
                    {
                        toggleAction->setIcon(QIcon(dockwidgets[i].mIcon.c_str()));
                        toggleAction->setText(dockwidgets[i].mCaption.c_str());
                        toggleAction->setStatusTip(QString("Toggle ") + QString(dockwidgets[i].mCaption.c_str()));
                        mCustomToggleActions.push_back(toggleAction);
                    }
                }
                catch(...)
                {
                    QMessageBox::warning(QApplication::activeWindow(), "qtOgitor", QString(tr("Invalid DockWidget ID:%1")).arg(QString(dockwidgets[i].mCaption.c_str())), QMessageBox::Ok);
                }
            }
        }
        else
        {
            QWidget *widget = static_cast<QDockWidget*>(dockwidgets[i].mHandle);
            if(widget)
            {
                QToolBox *toolbox = 0;
                QStackedWidget *stackedWidget = 0;
                if(dockwidgets[i].mParent == DOCK_PROPERTIES)
                    toolbox = mPropertiesToolBox;
                else if(dockwidgets[i].mParent == DOCK_RESOURCES)
                    stackedWidget = mResourcesStackedWidget;

                try
                {
                    widget->setObjectName(QString(dockwidgets[i].mCaption.c_str()));

                    if(toolbox)
                    {
                        if(dockwidgets[i].mIcon != "")
                            toolbox->addItem(widget, QIcon(dockwidgets[i].mIcon.c_str()), QString(dockwidgets[i].mCaption.c_str()));
                        else
                            toolbox->addItem(widget, QString(dockwidgets[i].mCaption.c_str()));
                    }
                    else if(stackedWidget)
                    {
                        QAction* button = mResourcesStackedActions->addAction(QIcon(dockwidgets[i].mIcon.c_str()), QString(dockwidgets[i].mCaption.c_str()));
                        button->setCheckable(true);

                        connect(button, SIGNAL(triggered()), mResourcesStackedMapper, SLOT(map()));
                        mResourcesStackedMapper->setMapping(button, mResourcesStackedActions->actions().size() - 1);

                        mResourcesStackedToolBar->addAction(button);

                        stackedWidget->addWidget(widget);
                    }                    
                }
                catch(...)
                {
                    QMessageBox::warning(QApplication::activeWindow(), "qtOgitor", QString(tr("Invalid DockWidget ID: %1")).arg(QString(dockwidgets[i].mCaption.c_str())), QMessageBox::Ok);
                }
            }
        }
    }
}
//------------------------------------------------------------------------------
void MainWindow::createCustomTabWindows()
{
    Ogitors::TabWidgetDataList tabwidgets = OgitorsRoot::getSingletonPtr()->GetTabWidgets();

    for(unsigned int i = 0;i < tabwidgets.size();i++)
    {
        QWidget *widget = static_cast<QWidget*>(tabwidgets[i].mHandle);
        if(widget)
        {
            try
            {
                widget->setObjectName(QString(tabwidgets[i].mCaption.c_str()));
                mEditorTab->addTab(widget, QString(tabwidgets[i].mCaption.c_str()));
            }
            catch(...)
            {
                QMessageBox::warning(QApplication::activeWindow(), "qtOgitor", QString(tr("Invalid TabWidget ID:%1")).arg(QString(tabwidgets[i].mCaption.c_str())), QMessageBox::Ok);
            }
        }
    }
}
//------------------------------------------------------------------------------
void MainWindow::addMenus()
{
    mMenuBar = new QMenuBar(this);
    mMenuBar->setObjectName(QString::fromUtf8("mMenuBar"));
    mMenuBar->setGeometry(QRect(0, 0, 616, 22));

    menuFile = new QMenu(tr("File"), mMenuBar);
    menuFile->setObjectName(QString::fromUtf8("menuFile"));
    mMenuBar->addAction(menuFile->menuAction());
    menuFile->addAction(actNew);
    menuFile->addAction(actOpen);
    menuFile->addAction(actSave);
    menuFile->addAction(actSaveAs);
    menuFile->addAction(actClose);
    menuFile->addSeparator();
    menuImport = menuFile->addMenu(tr("Import"));
    menuImport->setIcon(QIcon(":/icons/import.svg"));
    menuExport = menuFile->addMenu(tr("Export"));
    menuExport->setIcon(QIcon(":/icons/export.svg"));

    Ogre::StringVector theList;
    QSignalMapper *importMapper = new QSignalMapper(this);
    QSignalMapper *exportMapper = new QSignalMapper(this);

    OgitorsRoot::getSingletonPtr()->GetImportSerializerList(theList);

    for(unsigned int i = 0;i < theList.size();i++)
    {
        QAction *item = menuImport->addAction(QString(theList[i].c_str()));
        connect(item, SIGNAL(triggered()), importMapper, SLOT(map()));
        importMapper->setMapping(item, QString(theList[i].c_str()));
    }

    OgitorsRoot::getSingletonPtr()->GetExportSerializerList(theList);

    for(unsigned int i = 0;i < theList.size();i++)
    {
        QAction *item = menuExport->addAction(QString(theList[i].c_str()));
        connect(item, SIGNAL(triggered()), exportMapper, SLOT(map()));
        exportMapper->setMapping(item, QString(theList[i].c_str()));
    }

    connect(importMapper, SIGNAL(mapped( const QString & )), this, SLOT(importSerializer(  const QString & )));
    connect(exportMapper, SIGNAL(mapped( const QString & )), this, SLOT(exportSerializer(  const QString & )));

    menuRecentFiles = new QMenu(tr("Recent Files"), mMenuBar);
    menuRecentFiles->setObjectName(QString::fromUtf8("menuRecentFiles"));
    menuRecentFiles->setIcon(QIcon(":/icons/files.svg"));
    menuRecentFiles->setEnabled(false);
    updateRecentFiles();

    menuFile->addSeparator();
    menuFile->addAction(menuRecentFiles->menuAction());
    menuFile->addSeparator();
    menuFile->addAction(actExit);

    menuEdit = new QMenu(tr("Edit"), mMenuBar);
    menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
    mMenuBar->addAction(menuEdit->menuAction());
    menuEdit->addAction(actUndo);
    menuEdit->addAction(actRedo);
    menuEdit->addSeparator();
    menuEdit->addAction(actEditCopy);
    menuEdit->addAction(actEditCut);
    menuEdit->addAction(actEditPaste);
    menuEdit->addAction(actEditDelete);
    menuEdit->addAction(actEditRename);
    menuEdit->addAction(actEditCopyToTemplate);
    menuEdit->addAction(actEditCopyToTemplateWithChildren);
    menuEdit->addSeparator();
    menuEdit->addAction(actDecreaseGizmoScale);
    menuEdit->addAction(actIncreaseGizmoScale);
    menuEdit->addSeparator();
    menuEdit->addAction(actSceneOptions);
    menuEdit->addAction(actOpenPreferences);

    menuTools = new QMenu(tr("Tools"), mMenuBar);
    menuTools->setObjectName(QString::fromUtf8("menuTools"));
    mMenuBar->addAction(menuTools->menuAction());
    menuTools->addAction(actSelect);
    menuTools->addAction(actMove);
    menuTools->addAction(actRotate);
    menuTools->addAction(actScale);
    menuDefineSelectionList = menuTools->addMenu(tr("Define Selection List"));
    menuSelectSelectionList = menuTools->addMenu(tr("Activate Selection List"));
    for(int sa = 0;sa < 10;sa++)
    {
        menuDefineSelectionList->addAction(mSelectActions[sa]);
        menuSelectSelectionList->addAction(mSelectActions[sa + 10]);
    }

    menuView = new QMenu(tr("View"),mMenuBar);
    menuView->setObjectName(QString::fromUtf8("menuView"));
    mMenuBar->addAction(menuView->menuAction());
    menuView->addAction(actToggleExplorer);
    menuView->addAction(actToggleProjectFiles);
    menuView->addAction(actToggleLayer);
    menuView->addAction(actToggleProperties);
    menuView->addAction(actToggleTools);
    menuView->addAction(actToggleResources);
    menuView->addAction(actToggleLog);
    menuView->addSeparator();
    menuView->addAction(actToggleGrid);
    menuView->addAction(actToggleToolBar);
    menuView->addAction(actFullScreen);
    menuView->addAction(actSuperFullScreen);
    menuView->addSeparator();
    menuView->addAction(actLoadLayout);
    menuView->addAction(actSaveLayout);
    menuView->addSeparator();
    menuView->addAction(actHideMenuBar);

    menuLog = new QMenu(tr("Messages"),mMenuBar);
    menuLog->setObjectName(QString::fromUtf8("menuLog"));
    mMenuBar->addAction(menuLog->menuAction());
    menuLog->addAction(actSearchLog);
    menuLog->addAction(actClearLog);
    menuLog->addAction(actLogShowErrors);
    menuLog->addAction(actLogShowWarnings);
    menuLog->addAction(actLogShowInfo);
    menuLog->addAction(actLogShowDebug);

    mLogWidget->addAction(actSearchLog);
    mLogWidget->addAction(actClearLog);
    mLogWidget->addAction(actLogShowErrors);
    mLogWidget->addAction(actLogShowWarnings);
    mLogWidget->addAction(actLogShowInfo);
    mLogWidget->addAction(actLogShowDebug);

    menuCamera = new QMenu(tr("Camera"), mMenuBar);
    menuCamera->setObjectName(QString::fromUtf8("menuCamera"));
    mMenuBar->addAction(menuCamera->menuAction());
    menuCamera->addAction(actCamSave);
    menuView->addSeparator();
    menuCamera->addAction(actCamSpeedPlus);
    menuCamera->addAction(actCamSpeedMinus);

    menuTerrainTools = new QMenu(tr("Terrain Tools"), mMenuBar);
    menuTerrainTools->setObjectName(QString::fromUtf8("menuTerrainTools"));
    mMenuBar->addAction(menuTerrainTools->menuAction());
    menuTerrainTools->addAction(actDeform);
    menuTerrainTools->addAction(actSmooth);
    menuTerrainTools->addAction(actSplat);
    menuTerrainTools->addAction(actPaint);
    menuTerrainTools->addAction(actReLight);

    menuHelp = new QMenu(tr("Help"), mMenuBar);
    menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
    mMenuBar->addAction(menuHelp->menuAction());
    menuHelp->addAction(actHelp);
    menuHelp->addAction(actAbout);

    setMenuBar(mMenuBar);
}
//------------------------------------------------------------------------------
void MainWindow::setupStatusBar()
{
    mStatusBar = new QStatusBar(this);
    mStatusBar->setObjectName(QString::fromUtf8("mStatusBar"));
    setStatusBar(mStatusBar);

    mFPSLabel = new QLabel(tr("FPS: "));
    mFPSLabel->setMinimumWidth(87);
    mFPSSlider = new QSlider(Qt::Horizontal);
    mFPSSlider->setRange(0, 19);
    mFPSSlider->setTickInterval(1);
    mFPSSlider->setTickPosition(QSlider::TicksBelow);
    mFPSSlider->setMaximumWidth(100);
    mFPSSliderLabel = new QLabel(tr("FPS (30)"));
    mFPSSlider->setSliderPosition(5);
    mSelectedObjectsCountLabel = new QLabel();
    setSelectedObjectsCount(0);
    mSelectedObjectsCountLabel->setMinimumWidth(150);
    Ogitors::EventManager::getSingletonPtr()->connectEvent(Ogitors::EventManager::SELECTION_CHANGE, this, true, 0, true, 0, EVENT_CALLBACK(MainWindow, onSelectionChange));
    mTriangleCountLabel = new QLabel(tr("Triangles visible: %1").arg(0));
    mTriangleCountLabel->setMinimumWidth(120);
    mCamPosLabel = new QLabel(tr("Camera Position:"));
    mCamPosLabel->setMinimumWidth(300);
    mCamPosToolBar = new QToolBar();
    mCamPosToolBar->setIconSize(QSize(16, 16));
    mCamPosToolBar->addWidget(mSelectedObjectsCountLabel);
    mCamPosToolBar->addSeparator();
#if OGRE_MEMORY_TRACKER
    mMemoryUsageLabel = new QLabel("MU : 0000MB");
    mMemoryUsageLabel->setMinimumWidth(87);
    mCamPosToolBar->addWidget(mMemoryUsageLabel);
    mCamPosToolBar->addSeparator();
#endif
    mCamPosToolBar->addWidget(mTriangleCountLabel);
    mCamPosToolBar->addSeparator();
    mCamPosToolBar->addWidget(mCamPosLabel);
    mCamPosToolBar->addSeparator();
    mCamPosToolBar->addWidget(mFPSLabel);
    mCamPosToolBar->addSeparator();
    mCamPosToolBar->addWidget(mFPSSliderLabel);
    mCamPosToolBar->addWidget(mFPSSlider);

    mStatusViewToolBar = new QToolBar();
    mStatusViewToolBar->setIconSize(QSize(16, 16));
    mStatusViewToolBar->addAction(actToggleGrid);
    mStatusViewToolBar->addAction(actFullScreen);
    mStatusViewToolBar->addAction(actSuperFullScreen);

    mStatusShowHideToolBar = new QToolBar();
    mStatusShowHideToolBar->setIconSize(QSize(16, 16));
    mStatusShowHideToolBar->addAction(actToggleExplorer);
    mStatusShowHideToolBar->addAction(actToggleProjectFiles);
    mStatusShowHideToolBar->addAction(actToggleLayer);
    mStatusShowHideToolBar->addAction(actToggleProperties);
    mStatusShowHideToolBar->addAction(actToggleTools);
    mStatusShowHideToolBar->addAction(actToggleResources);
    mStatusShowHideToolBar->addAction(actToggleLog);

    for(unsigned int dw = 0; dw < mCustomToggleActions.size();dw++)
    {
        mStatusShowHideToolBar->addAction(mCustomToggleActions[dw]);
    }

    mStatusBar->insertPermanentWidget(0, mCamPosToolBar);
    mStatusBar->addPermanentWidget(mStatusViewToolBar);
    mStatusBar->addPermanentWidget(mStatusShowHideToolBar);

    connect(mFPSSlider, SIGNAL(valueChanged( int )), this, SLOT(fpsValueChanged( int )));
}
//------------------------------------------------------------------------------
static LogBuffer LOGBUFFER;

void MainWindow::messageLogged(const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName)
{
    LOGBUFFER.append(lml, message);
}
//------------------------------------------------------------------------------
void MainWindow::messageLogged(const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName, bool &skipThisMessage)
{
    LOGBUFFER.append(lml, message);
}
//------------------------------------------------------------------------------
void MainWindow::setupLog()
{
    mLogDockWidget = new QDockWidget(this);
    mLogDockWidget->setAllowedAreas(Qt::BottomDockWidgetArea);
    mLogDockWidget->setObjectName(QString::fromUtf8("logDockWidget"));
    mLogDockWidget->setWindowTitle(QApplication::translate("qtOgitor", "Ogitor Console", 0));

    mLogWidget = new QListWidget();
    mLogWidget->setObjectName(QString::fromUtf8("logWidget"));
    mLogWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    mLogWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mLogWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    mLogWidget->setWordWrap(true);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHeightForWidth(mLogWidget->sizePolicy().hasHeightForWidth());
    mLogWidget->setSizePolicy(sizePolicy);

    mScriptConsoleWidget = new QWidget();
    QVBoxLayout* verticalLayout = new QVBoxLayout(mScriptConsoleWidget);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    QVBoxLayout* verticalLayout_2 = new QVBoxLayout();
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    mTxtScriptInput = new LineEditWithHistory(mScriptConsoleWidget);
    mTxtScriptInput->setObjectName(QString::fromUtf8("txtScriptInput"));
    horizontalLayout->addWidget(mTxtScriptInput);
    QPushButton* btnRunScript = new QPushButton(tr("Execute"), mScriptConsoleWidget);
    btnRunScript->setObjectName(QString::fromUtf8("btnRunScript"));
    connect(btnRunScript, SIGNAL(clicked()), this, SLOT(runScriptClicked()));
    connect(mTxtScriptInput, SIGNAL(returnPressed()), this, SLOT(runScriptClicked()));
    horizontalLayout->addWidget(btnRunScript);
    verticalLayout_2->addLayout(horizontalLayout);
    mListScriptOutput = new QListWidget(mScriptConsoleWidget);
    mListScriptOutput->setObjectName(QString::fromUtf8("listScriptOutput"));
    mListScriptOutput->setContextMenuPolicy(Qt::ActionsContextMenu);
    mListScriptOutput->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mListScriptOutput->setSelectionBehavior(QAbstractItemView::SelectRows);
    mListScriptOutput->setWordWrap(true);
    mListScriptOutput->setSizePolicy(sizePolicy);
    verticalLayout_2->addWidget(mListScriptOutput);
    verticalLayout->addLayout(verticalLayout_2);
    mLogTabs = new QTabWidget(mLogDockWidget);
    mLogTabs->addTab(mLogWidget, tr("Log"));
    mLogTabs->addTab(mScriptConsoleWidget, tr("Script Console"));

    mLogDockWidget->setWidget(mLogTabs);
    mLogDockWidget->setVisible(false);

    addDockWidget(static_cast<Qt::DockWidgetArea>(8), mLogDockWidget);

    Ogre::LogManager::getSingleton().getDefaultLog()->addListener(this);
}
//------------------------------------------------------------------------------
void MainWindow::updateLog(QListWidgetItem* item)
{
    Q_ASSERT(item);
    item->setBackgroundColor(QColor("#000000"));
    mLogWidget->addItem(item);
    switch( item->type() )
    {
    case 4:
        // Warning
        item->setText(item->text());
        item->setTextColor(QColor("#FFCC00"));
        item->setHidden(!actLogShowWarnings->isChecked());
        break;
    case 3:
        // Error
        item->setText(item->text());
        item->setTextColor(QColor("#EE1100"));
        item->setHidden(!actLogShowErrors->isChecked());
        break;
    case 2:
        // Info
        item->setText(item->text());
        item->setTextColor(QColor("#C0C0C0"));
        item->setHidden(!actLogShowInfo->isChecked());
        break;
    case 1:
        // Debug
        item->setText(item->text());
        item->setTextColor(QColor("#C0C0C0"));
        item->setHidden(!actLogShowDebug->isChecked());
        break;
    default:
        break;
    }
}
//------------------------------------------------------------------------------
void MainWindow::setCameraPositions()
{
    Ogitors::PROJECTOPTIONS *pOpt = OgitorsRoot::getSingletonPtr()->GetProjectOptions();

    for(int i =0;i < pOpt->CameraSaveCount;i++)
    {
        QString menustr;
        menustr = QString(tr("Camera %1: <%2, %3, %4>")).arg(i).arg(pOpt->CameraPositions[i].x)
            .arg(pOpt->CameraPositions[i].y).arg(pOpt->CameraPositions[i].z);
        menuCameraPositions[i]->setTitle(menustr);
    }
    for(int i = pOpt->CameraSaveCount;i < 10;i++)
    {
        menuCameraPositions[i]->setTitle(tr("(Empty Slot)"));
    }

    mCameraSpeedSlider->setValue(pOpt->CameraSpeed);
}
//------------------------------------------------------------------------------
void MainWindow::setSelectedObjectsCount(int count)
{
    mSelectedObjectsCountLabel->setText(tr("%1 Object(s) selected").arg(count));
}
//------------------------------------------------------------------------------
void MainWindow::updateRecentFiles()
{
    if(!recentMapper)
        recentMapper = new QSignalMapper(this);
    else
    {
        disconnect(recentMapper, SIGNAL(mapped( const QString & )), this, SLOT(openRecentFile(  const QString & )));
    }

    UTFStringVector recentList;

    OgitorsRoot::getSingletonPtr()->GetRecentFiles(recentList);

    menuRecentFiles->clear();

    if(recentList.size() > 0)
    {
        for(unsigned int i = 0;i < recentList.size();i++)
        {
            QAction *item = menuRecentFiles->addAction(ConvertToQString(recentList[i]));
            item->setIcon(QIcon(":/icons/filenew.svg"));
            connect(item, SIGNAL(triggered()), recentMapper, SLOT(map()));
            recentMapper->setMapping(item, ConvertToQString(recentList[i]));
        }

        connect(recentMapper, SIGNAL(mapped( const QString & )), this, SLOT(openRecentFile(  const QString & )));
    }

    menuRecentFiles->setEnabled(recentList.size() > 0);
}
//------------------------------------------------------------------------------
unsigned int renderDelta = 0;
unsigned int totalMemoryUsage = 0;
unsigned int updateCount = 3;
unsigned int lastConsolePos = 0;

void MainWindow::timerLoop()
{
    //if(mHasFileArgs)
    //{
    //    if(mOgreWidget->mOgreInitialised)
    //    {
    //        OgitorsRoot::getSingletonPtr()->LoadScene(mArgsFile.toStdString());
    //        mHasFileArgs = false;
    //        mOgreWidget->setDoLoadFile(false);
    //    }
    //}

    bool isSceneLoaded = OgitorsRoot::getSingletonPtr()->IsSceneLoaded();

    if(isMinimized() || !mAppActive || !isSceneLoaded)
    {
        if(mTimer->interval() != 200)
            mTimer->setInterval(200);
    }
    else
    {
        if(mTimer->interval() != 0)
            mTimer->setInterval(0);
    }

    if(isSceneLoaded)
    {
        mOgreWidget->ProcessKeyActions();

        --updateCount;
        if(!updateCount)
        {
            updateActions();
            updateCount = 3;
        }
    }
    else
        updateCount = 1;

    LogDataVector messages;
    LOGBUFFER.getBuffer(messages);

    for(unsigned int i = 0;i < messages.size();i++)
    {
        updateLog(new QListWidgetItem(messages[i].mMessage, 0, messages[i].mLevel) );
    }

    if(messages.size() > 0)
        mLogWidget->scrollToBottom();

    Ogre::StringVector conMessages;
    lastConsolePos = OgitorsScriptConsole::getSingletonPtr()->getOutput(lastConsolePos, conMessages);

    for(unsigned int i = 0;i < conMessages.size();i++)
    {
        QListWidgetItem *itemScript = new QListWidgetItem(QString(conMessages[i].c_str()));
        itemScript->setBackgroundColor(QColor("#000000"));
        mListScriptOutput->addItem(itemScript);
    }

    if(conMessages.size() > 0)
        mListScriptOutput->scrollToBottom();

#if OGRE_MEMORY_TRACKER
    unsigned int total = Ogre::MemoryTracker::get().getTotalMemoryAllocated() / (1024 * 1024);
    if(total != totalMemoryUsage)
    {
        totalMemoryUsage = total;
        mMemoryUsageLabel->setText(QString("MU : %1MB").arg(total));
    }
#endif

    if(isSceneLoaded && mOgreWidget->isVisible())
    {
        QTime time = QTime::currentTime();
        unsigned int curtime = time.hour() * 60 + time.minute();
        curtime = (curtime * 60) + time.second();
        curtime = (curtime * 1000) + time.msec();

        unsigned int timediff = curtime - mLastTime;
        mLastTime = curtime;

        renderDelta += timediff;

        unsigned int TargetDelta;

        if(mTargetRenderCount > 0)
            TargetDelta = 1000 / mTargetRenderCount;
        else
            TargetDelta = 1000000;

        if(renderDelta >= TargetDelta)
        {
            renderDelta = renderDelta % TargetDelta;
            mOgreWidget->update();
        }
    }
}
//------------------------------------------------------------------------------------
bool inActivationEvent = false;

bool MainWindow::eventFilter(QObject *obj,  QEvent *event)
{
    if (obj == mApplicationObject  &&  !inActivationEvent)
    {
        if (event->type() == QEvent::ApplicationActivate)
        {
            // This gets called when the application starts, and when you switch back.
            inActivationEvent = true;
            mAppActive = true;
            inActivationEvent = false;
        }

        else if (event->type() == QEvent::ApplicationDeactivate)
        {
            // This gets called when the application closes, and when you switch away.
            inActivationEvent = true;
            mAppActive = false;
            inActivationEvent = false;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}
//------------------------------------------------------------------------------------
void MainWindow::onSceneRunStateChange(Ogitors::IEvent* evt)
{
    if(!mPlayerToolbar)
        return;

    RunStateChangeEvent *change_event = Ogitors::event_cast<RunStateChangeEvent*>(evt);

    if(change_event)
    {
        //reload the zone selection widget when a scene is loaded
        RunState state = change_event->getType();

        if(state == RS_STOPPED)
        {
            actPlayerRunPause->setEnabled(OgitorsRoot::getSingletonPtr()->IsSceneLoaded());
            actPlayerRunPause->setToolTip(tr("Run Scene Scripts"));
            actPlayerRunPause->setToolTip(tr("Run scripts in the scene"));
            actPlayerRunPause->setIcon(QIcon(":/icons/player_play.svg"));

            actPlayerStop->setEnabled(false);
        }
        else if(state == RS_PAUSED)
        {
            actPlayerRunPause->setEnabled(OgitorsRoot::getSingletonPtr()->IsSceneLoaded());
            actPlayerRunPause->setToolTip(tr("Run Scene Scripts"));
            actPlayerRunPause->setToolTip(tr("Run scripts in the scene"));
            actPlayerRunPause->setIcon(QIcon(":/icons/player_play.svg"));

            actPlayerStop->setEnabled(OgitorsRoot::getSingletonPtr()->IsSceneLoaded());
        }
        else if(state == RS_RUNNING)
        {
            actPlayerRunPause->setEnabled(OgitorsRoot::getSingletonPtr()->IsSceneLoaded());
            actPlayerRunPause->setToolTip(tr("Pause Scene Scripts"));
            actPlayerRunPause->setToolTip(tr("Pause scripts in the scene"));
            actPlayerRunPause->setIcon(QIcon(":/icons/player_pause.svg"));

            actPlayerStop->setEnabled(OgitorsRoot::getSingletonPtr()->IsSceneLoaded());
        }
    }
}
//------------------------------------------------------------------------------------
void MainWindow::onSceneEditorToolChange(Ogitors::IEvent* evt)
{
    EditorToolChangeEvent *change_event = Ogitors::event_cast<EditorToolChangeEvent*>(evt);

    if(change_event)
    {
        //reload the zone selection widget when a scene is loaded
        unsigned int tool = change_event->getType();

        actSelect->setChecked(tool == TOOL_SELECT);
        actMove->setChecked(tool == TOOL_MOVE);
        actRotate->setChecked(tool == TOOL_ROTATE);
        actScale->setChecked(tool == TOOL_SCALE);

        actDeform->setChecked(tool == TOOL_DEFORM);
        actSmooth->setChecked(tool == TOOL_SMOOTH);
        actSplat->setChecked(tool == TOOL_SPLAT);
        actSplatGrass->setChecked(tool == TOOL_SPLATGRASS);
        actPaint->setChecked(tool == TOOL_PAINT);

        mTerrainToolsWidget->switchToolWidget(tool);
    }
}
//------------------------------------------------------------------------------------
void MainWindow::onSceneModifiedChange(Ogitors::IEvent* evt)
{
    SceneModifiedChangeEvent *change_event = Ogitors::event_cast<SceneModifiedChangeEvent*>(evt);

    if(change_event)
    {
        bool state = change_event->getState();

        if(state != actSave->isEnabled())
        {
            QString appTitle;
            appTitle = "qtOgitor ";
            appTitle += Ogitors::Globals::OGITOR_VERSION.c_str();
            if(state)
                appTitle += QString(" - (*)");
            else
                appTitle += QString(" - ");

            appTitle += QString(OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ProjectName.c_str()) + QString(".ogscene");
            setWindowTitle(appTitle);
        }

        actSave->setEnabled(state);
    }
}
//------------------------------------------------------------------------------------
void MainWindow::onUndoManagerNotification(Ogitors::IEvent* evt)
{
    UndoManagerNotificationEvent *change_event = Ogitors::event_cast<UndoManagerNotificationEvent*>(evt);

    if(change_event)
    {
        if(change_event->getType() == 0)
        {
            actUndo->setEnabled(change_event->getEnabled());
            if(change_event->getEnabled())
                actUndo->setToolTip(tr("Undo") + QString(" ") + change_event->getCaption().c_str());
            else
                actUndo->setToolTip("");
        }
        else
        {
            actRedo->setEnabled(change_event->getEnabled());
            if(change_event->getEnabled())
                actRedo->setToolTip(tr("Redo") + QString(" ") + change_event->getCaption().c_str());
            else
                actRedo->setToolTip("");
        }
    }
}
//------------------------------------------------------------------------------------
void MainWindow::onTerrainEditorChange(Ogitors::IEvent* evt)
{
    TerrainEditorChangeEvent *change_event = Ogitors::event_cast<TerrainEditorChangeEvent*>(evt);

    if(change_event)
    {
        ITerrainEditor *terED = change_event->getHandle();

        actToggleWalkAround->setEnabled(terED);
        actDeform->setEnabled(terED && terED->canDeform());
        actSmooth->setEnabled(terED && terED->canDeform());
        actSplat->setEnabled(terED && terED->canSplat());
        actSplatGrass->setEnabled(terED && terED->canSplat());
        actPaint->setEnabled(terED && terED->canPaint());
        actReLight->setEnabled(terED);
    }
}
//------------------------------------------------------------------------------------
void MainWindow::onSelectionChange(Ogitors::IEvent* evt)
{
    SelectionChangeEvent *change_event = Ogitors::event_cast<SelectionChangeEvent*>(evt);

    setSelectedObjectsCount(change_event->getMultiSelEditor()->getSelection().size());
}
//------------------------------------------------------------------------------------
void MainWindow::autoSaveScene()
{
    bool modified = OgitorsRoot::getSingletonPtr()->IsSceneModified();

    // Build new file name
    QDateTime current = QDateTime::currentDateTime();
    QString exportfile = OgitorsRoot::getSingletonPtr()->GetProjectOptions()->AutoBackupFolder.c_str();
    exportfile.append(QDir::separator());
    exportfile.append(OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ProjectName.c_str());
    exportfile.append(current.toString("_yyyy_MM_dd_HH_mm"));

    // Limited number of backups -> count current and delete one if needed
    if(OgitorsRoot::getSingletonPtr()->GetProjectOptions()->AutoBackupNumber != 0)
    {
        QDir dir = QDir(OgitorsRoot::getSingletonPtr()->GetProjectOptions()->AutoBackupFolder.c_str());
        QFileInfoList fileList = dir.entryInfoList();
        QFileInfoList fileListBackup = QFileInfoList();
        QString regexString = OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ProjectName.c_str();
        regexString.append("{1}\\S*ofs{1}");
        QRegExp regex = QRegExp(regexString);

        for(int i = 0; i < fileList.size(); i++)
        {
            if(regex.exactMatch(fileList[i].fileName()))
                fileListBackup.append(fileList[i]);
        }

        // Got the find the oldest file and get rid of it.
        // To ensure all is correct, the creation dates of the files are compared,
        // although in most (if not any) cases, the first file of the fileListBackup will be
        // the one that was created first, since those file names are usually sorted by their names
        // and the backup date and time is encoded in the file name itself.
        if(fileListBackup.size() >= OgitorsRoot::getSingletonPtr()->GetProjectOptions()->AutoBackupNumber)
        {
            QDateTime oldestDateTime = fileListBackup[0].created();
            QString oldestFilePath = fileListBackup[0].filePath();

            for(int i = 0; i < fileListBackup.size(); i++)
                if(fileListBackup[i].created().toTime_t() < oldestDateTime.toTime_t())
                {
                    oldestDateTime = fileListBackup[i].created();
                    oldestFilePath = fileListBackup[i].filePath();
                }

                QFile::remove(oldestFilePath);
        }
    }

    if(exportfile[0] == '.')
        exportfile = QString(OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ProjectDir.c_str()) + QString("/") + exportfile;

    OgitorsRoot::getSingletonPtr()->SaveScene(false, exportfile.toStdString());

    OgitorsRoot::getSingletonPtr()->SetSceneModified(modified);
}
//------------------------------------------------------------------------------------
void MainWindow::onFocusOnObject()
{
    CBaseEditor *target = 0;
    QTreeWidgetItem *item = mSceneViewWidget->getTreeWidget()->selectedItems().at(0);

    if(item)
        target = OgitorsRoot::getSingletonPtr()->FindObject(item->text(0).toStdString());

    CViewportEditor *ovp = OgitorsRoot::getSingletonPtr()->GetViewport();

    if(target && ovp && target->supports(CAN_FOCUS))
    {
        OgitorsRoot::getSingletonPtr()->GetSelection()->setSelection(target);
        ovp->FocusSelectedObject();
    }
}
//------------------------------------------------------------------------------------
