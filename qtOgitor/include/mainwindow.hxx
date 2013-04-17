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
#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <QtGui/QtGui>
#include <QtCore/QtCore>

#include "OgitorsPrerequisites.h"
#include "OgitorsDefinitions.h"

//-----------------------------------------------------------------------------------------

class OgreWidget;
class SceneViewWidget;
class LayerViewWidget;
class GeneralPropertiesViewWidget;
class CustomPropertiesViewWidget;
class EntityViewWidget;
class TemplateViewWidget;
class ColourPickerWidget;
class ObjectsViewWidget;
class ActionToolbar;
class OgitorPreferencesWidget;
class PreferencesManager;
class LineEditWithHistory;
class TerrainToolsWidget;
class GenericTextEditor;
class ProjectFilesViewWidget;
class GenericImageEditor;

extern int FPSLIST[12];
extern int BrushValueTable[25];

class QProcess;

//-----------------------------------------------------------------------------------------

class OgitorAssistant
{
public:
    OgitorAssistant();
    ~OgitorAssistant();
    void showDocumentation(const QString &file);

private:
    bool startOgitorAssistant();
    QProcess *proc;
};

//-----------------------------------------------------------------------------------------

struct ScriptActionData
{
    QString icon;
    QString script;
    bool    active;
    QAction *action;
};

//-----------------------------------------------------------------------------------------
typedef std::map<std::string, QIcon> FileIconMap;

class MainWindow : public QMainWindow, Ogre::LogListener
{
    Q_OBJECT

public:
    OgreWidget*             mOgreWidget;
    QMenuBar*               mMenuBar;
    QStatusBar*             mStatusBar;
    QTabWidget*             mEditorTab;
    QToolBox*               mPropertiesToolBox;
    QToolBox*               mExplorerToolBox;
    QStackedWidget*         mResourcesStackedWidget;
    QWidget*                mTerrainTab;
    QListWidget*            mLogWidget;
    QWidget*                mScriptConsoleWidget;
    QListWidget*            mListScriptOutput;
    LineEditWithHistory*    mTxtScriptInput;
    QDockWidget*            mExplorerDockWidget;
    QDockWidget*            mLayerDockWidget;
    QDockWidget*            mResourcesDockWidget;
    QToolBar*               mResourcesStackedToolBar;
    QSignalMapper*          mResourcesStackedMapper;
    QDockWidget*            mPropertiesDockWidget;
    QDockWidget*            mToolsDockWidget;
    QDockWidget*            mProjectFilesDockWidget;
    QTabWidget*             mLogTabs;
    QDockWidget*            mLogDockWidget;
    TerrainToolsWidget*     mTerrainToolsWidget;

    QToolBar*           mFileToolBar;
    QToolBar*           mHelpToolBar;
    QToolBar*           mEditToolBar;
    QToolBar*           mViewToolBar;
    ActionToolbar*      mScriptActionsBar;
    QToolBar*           mPlayerToolbar;
    QAction*            actPlayerRunPause;
    QAction*            actPlayerStop;
    QAction*            actAddScriptToToolbar;
    QCheckBox*          moveX;
    QCheckBox*          moveY;
    QCheckBox*          moveZ;
    QCheckBox*          mSnapGround;
    QComboBox*          mSnapMultiplierBox;
    QComboBox*          mCameraViewModeBox;

    QAction*  actHideMenuBar;
    QAction*  actSaveLayout;
    QAction*  actLoadLayout;
    QAction*  actNew;
    QAction*  actOpen;
    QAction*  actClose;
    QAction*  actSave;
    QAction*  actSaveAs;
    QAction*  actSceneOptions;
    QAction*  actExit;
    QAction*  actFullScreen;
    QAction*  actSuperFullScreen;
    QAction*  actAbout;
    QAction*  actHelp;
    QAction*  actSelect;
    QAction*  actMove;
    QAction*  actRotate;
    QAction*  actScale;
    QAction*  actToggleGrid;
    QAction*  actToggleToolBar;
    QAction*  actToggleExplorer;
    QAction*  actToggleLayer;
    QAction*  actToggleTools;
    QAction*  actToggleProperties;
    QAction*  actToggleResources;
    QAction*  actOpenPreferences;
    QAction*  actToggleProjectFiles;
    QAction*  actToggleLog;
    QAction*  actUndo;
    QAction*  actRedo;
    QAction*  actDeform;
    QAction*  actSmooth;
    QAction*  actSplat;
    QAction*  actSplatGrass;
    QAction*  actPaint;
    QAction*  actReLight;
    QAction*  actCamSave;
    QAction*  actCamSpeedMinus;
    QAction*  actCamSpeedPlus;
    QAction*  actCamModeShaded;
    QAction*  actCamModeWireframe;
    QAction*  actCamModeHiddenline;
    QAction*  actCamModeTextured;
    QAction*  actLogShowWarnings;
    QAction*  actLogShowErrors;
    QAction*  actLogShowInfo;
    QAction*  actLogShowDebug;
    QAction*  actSearchLog;
    QAction*  actClearLog;
    QAction*  actIncreaseGizmoScale;
    QAction*  actDecreaseGizmoScale;
    QAction*  actToggleWorldSpaceGizmo;
    QAction*  actToggleWalkAround;
    QAction*  actFocus;

    QAction*  actEditCopy;
    QAction*  actEditCut;
    QAction*  actEditDelete;
    QAction*  actEditRename;
    QAction*  actEditPaste;
    QAction*  actEditCopyToTemplate;
    QAction*  actEditCopyToTemplateWithChildren;

    QActionGroup* mResourcesStackedActions; 

    QMenu*    menuCompactMenuBar;
    QMenu*    menuFile;
    QMenu*    menuImport;
    QMenu*    menuExport;
    QMenu*    menuTools;
    QMenu*    menuEdit;
    QMenu*    menuCamera;
    QMenu*    menuView;
    QMenu*    menuLog;
    QMenu*    menuPlugins;
    QMenu*    menuCamTools;
    QMenu*    menuHelp;
    QMenu*    menuCamPolyMode;
    QMenu*    menuTerrainTools;
    QMenu*    menuCameraPositionMain;
    QMenu*    menuCameraPositions[10];
    QMenu*    menuRecentFiles;
    QMenu*    menuDefineSelectionList;
    QMenu*    menuSelectSelectionList;
    QAction*  mSelectActions[20];

    QSignalMapper *recentMapper;

    QSlider*  mCameraSpeedSlider;
    QSlider*  mFPSSlider;
    QLabel*   mFPSSliderLabel;
#if OGRE_MEMORY_TRACKER
    QLabel*   mMemoryUsageLabel;
#endif
    QLabel*   mSelectedObjectsCountLabel;
    QLabel*   mTriangleCountLabel;
    QLabel*   mFPSLabel;
    QLabel*   mCamPosLabel;
    QToolBar* mCamPosToolBar;
    QToolBar* mStatusShowHideToolBar;
    QToolBar* mStatusViewToolBar;

    QFileIconProvider        mIconProvider;
    FileIconMap              mFileIconMap; 
    OgitorPreferencesWidget *mPreferencesWidget;

    MainWindow(QString args = "", QWidget *parent = 0);
    ~MainWindow();

    void retranslateUi();
    void readSettings(QString filename = "");
    void writeSettings(QString filename = "");

	SceneViewWidget                 *getSceneViewWidget() {return mSceneViewWidget;};
    EntityViewWidget                *getEntityViewWidget() {return mEntityViewWidget;};
    ObjectsViewWidget               *getObjectsViewWidget() {return mObjectsViewWidget;};
    TemplateViewWidget              *getTemplatesViewWidget() {return mTemplatesViewWidget;};
    LayerViewWidget                 *getLayersViewWidget() {return mLayerViewWidget;};
    GeneralPropertiesViewWidget     *getGeneralPropertiesViewWidget() {return mGeneralPropertiesViewWidget;}; 
    OgreWidget                      *getOgreWidget() {return mOgreWidget;};
    TerrainToolsWidget              *getTerrainToolsWidget() {return mTerrainToolsWidget;};
    ProjectFilesViewWidget          *getProjectFilesViewWidget() {return mProjectFilesViewWidget;};
    GenericTextEditor               *getGenericTextEditor() {return mGenericTextEditor;};
    GenericImageEditor              *getGenericImageEditor() {return mGenericImageEditor;};
    QTimer                          *getAutoBackupTimer() {return mAutoBackupTimer;};
    QTabWidget                      *getEditorTab() {return mEditorTab;};

    void                            setCameraPositions();
    void                            updateLoadTerminateActions(bool loaded);
    void                            setSelectedObjectsCount(int count);

    void updateLog(QListWidgetItem* item);
    void scrollLogToBottom() {mLogWidget->scrollToBottom();};

    void showSubWindows();
    void hideSubWindows();

    void setApplicationObject(QObject *obj);
    bool getAppActive() { return mAppActive; };

    void _saveScriptActions();
    void _addScriptAction(const QString& iconpath, const QString& scriptpath);
    void _removeScriptAction(QAction *action);
    void _editScriptAction(QAction *action);

    /**
    * Parses the contents of a menu list and appends them to a QMenu. Also a signal mapper
    * is set up and connected.
    * @param contextMenu the menu to which the new menu options will be added
    * @param menuList a list of menu options that should be added. Each string in that vector
    *        consists of a the text to be displayed followed by an optional ";" and an 
    *        optional icon resource path. 
    *        If a string starts with a ">" it is considered a
    *        sub menu. All following entries starting with a "#" will then be considered 
    *        elements of that submenu.
    *        The third string element is an int (0 or 1) indicting whether this context 
    *        menu element should be enabled or not.
    * @param receiver the object to which the signal mapper will be connected to
    */
    static void parseAndAppendContextMenuList(QMenu* contextMenu, Ogitors::UTFStringVector menuList, QObject* receiver);

public Q_SLOTS:
    void hideMenuBar();
    void onAddScriptAction();
    void onExecuteScriptAction(int index);
    void runScriptClicked();
    void saveLayout();
    void loadLayout();
    void timerLoop();
    void showAbout();
    void showHelp();
    void exitApp();
    void cmdUndo();
    void cmdRedo();
    void newScene();
    void openScene();
    void openRecentFile(const QString& value);
    void closeScene();
    void saveScene(const QString& exportfile = "");
    void saveSceneAs();
    void setToolSelect();
    void setToolMove();
    void setToolRotate();
    void setToolScale();
    void setToolDeform();
    void setToolSmooth();
    void setToolSplat();
    void setToolSplatGrass();
    void setToolPaint();
    void relightTerrain();
    void toggleGrid();
    void toggleFullScreen();
    void toggleSuperFullScreen();
    void snapMultiplierIndexChanged(int index);
    void snapGroundChanged(int state);
    void viewModeIndexChanged(int index);
    void toggleToolBar(bool b);
    void exportSerializer(const QString& value);
    void importSerializer(const QString& value);
    void saveCamera();
    void deleteCamera(int id);
    void incCamera();
    void decCamera();
    void toggleWorldSpaceGizmo();
    void toggleWalkAround();
    void cameraSpeedValueChanged(int value);
    void fpsValueChanged(int value);
    void setCameraPolyMode(int value);
    void selectActionTriggered(int value);
    void cameraAction(int id);
    void toggleLogMessages();
    void searchLog();
    void openPreferences();
    void openSceneOptions();
    void editCopy();
    void editCut();
    void editPaste();
    void editDelete();
    void editRename();
    void editCopyToTemplate();
    void editCopyToTemplateWithChildren();
    void increaseGizmoScale();
    void decreaseGizmoScale();
    void onPlayerRunPause();
    void onPlayerStop();
    void onFocusOnObject();
    void onSwitchStackedResources(int index);
    void onProjectFilesVisibilityChanged(bool visible);

private:
    SceneViewWidget                 *mSceneViewWidget;
    LayerViewWidget                 *mLayerViewWidget;
    GeneralPropertiesViewWidget     *mGeneralPropertiesViewWidget;
    CustomPropertiesViewWidget      *mCustomPropertiesViewWidget;
    EntityViewWidget                *mEntityViewWidget;
    ObjectsViewWidget               *mObjectsViewWidget;
    TemplateViewWidget              *mTemplatesViewWidget;
    ProjectFilesViewWidget          *mProjectFilesViewWidget;
    PreferencesManager              *mPrefManager;
    OgitorAssistant                 *mOgitorAssistant;
    GenericTextEditor               *mGenericTextEditor;
    GenericImageEditor              *mGenericImageEditor;
    std::vector<QWidget*>           mSubWindowStateSave;
    QTimer                          *mTimer;
    unsigned int                    mLastTime;
    QString                         mArgsFile;
    bool                            mSubWindowsVisible;
    int                             mTargetRenderCount;
    QObject                         *mApplicationObject;
    bool                            mAppActive;
    std::vector<QAction*>           mCustomToggleActions;
    std::vector<unsigned long>      mSelectLists[10];
    std::vector<ScriptActionData>   mScriptActions;
    QSignalMapper*                  mScriptActionMap;
    QTimer                          *mAutoBackupTimer;

    void createScriptActionsToolbar();
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj,  QEvent *event);
    void initHiddenRenderWindow();
    void updateRecentFiles();
    bool mHasFileArgs;
    std::string mLastLoadedScene;
    void addDockWidgets(QMainWindow* parent);
    void createSceneRenderWindow();
    void createHomeTab();
    void createCustomTabWindows();
    void createCustomToolbars();
    void createCustomDockWidgets();
    void createPlayerToolbar();
    void addActions();
    void addMenus();
    void setupStatusBar();
    void updateActions();
    void setupLog();
    
    //Keeping the old version for compatibility
    void messageLogged (const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName);
    void messageLogged (const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName, bool &skipThisMessage);
    
    void onSceneRunStateChange(Ogitors::IEvent* evt);
    
    void onSceneEditorToolChange(Ogitors::IEvent* evt);
    void onSceneModifiedChange(Ogitors::IEvent* evt);
    void onUndoManagerNotification(Ogitors::IEvent* evt);
    void onTerrainEditorChange(Ogitors::IEvent* evt);
    void onSelectionChange(Ogitors::IEvent* evt);

private Q_SLOTS:
    void autoSaveScene();
    //void toggleLogMessages();
};

//-----------------------------------------------------------------------------------------

extern MainWindow *mOgitorMainWindow;

#endif // MAINWINDOW_HXX

//-----------------------------------------------------------------------------------------
