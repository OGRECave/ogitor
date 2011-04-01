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

#include "Ogitors.h"
#include "OgitorsUndoManager.h"
#include "OgitorsClipboardManager.h"
#include "OgitorsScriptConsole.h"
#include "ViewGrid.h"
#include "DefaultEvents.h"
#include "EventManager.h"
#include "ofs.h"

#include "ogrewidget.hxx"
#include "aboutdialog.hxx"
#include "sceneview.hxx"
#include "layerview.hxx"
#include "propertiesviewgeneral.hxx"
#include "propertiesviewcustom.hxx"
#include "entityview.hxx"
#include "objectsview.hxx"
#include "templateview.hxx"
#include "settingsdialog.hxx"
#include "addtemplatedialog.hxx"
#include "addscriptactiondialog.hxx"
#include "actiontoolbar.hxx"
#include "preferencesmanager.hxx"
#include "lineeditwithhistory.hxx"
#include "terraintoolswidget.hxx"

using namespace Ogitors;

//------------------------------------------------------------------------------
void MainWindow::addActions()
{
    actSaveLayout = new QAction(tr("Save Layout"), this);
    actSaveLayout->setStatusTip(tr("Save Current Layout to File"));
    actSaveLayout->setIcon( QIcon( ":/icons/filesave.svg" ));

    actLoadLayout = new QAction(tr("Load Layout"), this);
    actLoadLayout->setStatusTip(tr("Load Layout from File"));
    actLoadLayout->setIcon( QIcon( ":/icons/fileopen.svg" ));

    actNew = new QAction(tr("New"), this);
    actNew->setStatusTip(tr("Create a new Ogitor scene"));
    actNew->setIcon( QIcon( ":/icons/ogscene.png" ));
    actNew->setShortcut(QKeySequence::New);

    actOpen = new QAction(tr("Open"), this);
    actOpen->setStatusTip(tr("Open an Ogitor scene"));
    actOpen->setIcon( QIcon( ":/icons/fileopen.svg" ));
    actOpen->setShortcut(QKeySequence::Open);

    actClose = new QAction(tr("Close"), this);
    actClose->setStatusTip(tr("Close Ogitor scene"));
    actClose->setIcon( QIcon( ":/icons/fileclose.svg" ));

    actSave = new QAction(tr("Save"), this);
    actSave->setStatusTip(tr("Save Ogitor scene"));
    actSave->setIcon( QIcon( ":/icons/filesave.svg" ));
    actSave->setShortcut(QKeySequence::Save);

    actSaveAs = new QAction(tr("Save As"), this);
    actSaveAs->setStatusTip(tr("Save Ogitor scene As"));
    actSaveAs->setIcon( QIcon( ":/icons/filesaveas.svg" ));
    actSaveAs->setShortcut(QKeySequence::SaveAs);

    actSceneOptions = new QAction(tr("Settings"), this);
    actSceneOptions->setStatusTip(tr("Ogitor scene Settings"));
    actSceneOptions->setIcon( QIcon( ":/icons/settings.svg" ));

    actExit = new QAction(tr("Exit"), this);
    actExit->setStatusTip(tr("Exit Application"));
    actExit->setIcon( QIcon( ":/icons/exit.svg" ));
    actExit->setShortcut(QKeySequence::Close);

    actSelect = new QAction(tr("Select"), this);
    actSelect->setStatusTip(tr("Select"));
    actSelect->setIcon( QIcon( ":/icons/select.svg"));
    actSelect->setShortcut(QKeySequence("Alt+T"));
    actSelect->setCheckable(true);

    actMove = new QAction(tr("Move"), this);
    actMove->setStatusTip(tr("Move"));
    actMove->setIcon( QIcon( ":/icons/translate.svg"));
    actMove->setShortcut(QKeySequence("Alt+M"));
    actMove->setCheckable(true);

    actRotate = new QAction(tr("Rotate"), this);
    actRotate->setStatusTip(tr("Rotate"));
    actRotate->setIcon( QIcon( ":/icons/rotate.svg"));
    actRotate->setShortcut(QKeySequence("Alt+R"));
    actRotate->setCheckable(true);

    actScale = new QAction(tr("Scale"), this);
    actScale->setStatusTip(tr("Scale"));
    actScale->setIcon( QIcon( ":/icons/scale.svg"));
    actScale->setShortcut(QKeySequence("Alt+S"));
    actScale->setCheckable(true);

    actDeform = new QAction(tr("Deform"), this);
    actDeform->setStatusTip(tr("Deform"));
    actDeform->setIcon( QIcon( ":/icons/deform.svg"));
    actDeform->setCheckable(true);

    actSmooth = new QAction(tr("Smooth"), this);
    actSmooth->setStatusTip(tr("Smooth"));
    actSmooth->setIcon( QIcon( ":/icons/smooth.svg"));
    actSmooth->setCheckable(true);

    actSplat = new QAction(tr("Splat"), this);
    actSplat->setStatusTip(tr("Splat"));
    actSplat->setIcon( QIcon( ":/icons/splat.svg"));
    actSplat->setCheckable(true);

    actSplatGrass = new QAction(tr("Splat Grass"), this);
    actSplatGrass->setStatusTip(tr("Splat Grass"));
    actSplatGrass->setIcon( QIcon( ":/icons/splatgrass.svg"));
    actSplatGrass->setCheckable(true);

    actPaint = new QAction(tr("Paint"), this);
    actPaint->setStatusTip(tr("Paint"));
    actPaint->setIcon( QIcon( ":/icons/paint.svg"));
    actPaint->setCheckable(true);

    actReLight = new QAction(tr("Re-Light"), this);
    actReLight->setStatusTip(tr("Re-Calculate Lighting"));
    actReLight->setIcon( QIcon( ":/icons/relight.svg"));
    actReLight->setCheckable(false);

    actAbout = new QAction(tr("About"), this);
    actAbout->setStatusTip(tr("About qtOgitor"));
    actAbout->setIcon( QIcon( ":/icons/about.svg"));

    actHelp = new QAction(tr("Help"), this);
    actHelp->setStatusTip(tr("qtOgitor Help"));
    actHelp->setIcon( QIcon( ":/icons/help.svg"));
    actHelp->setShortcut(QKeySequence::HelpContents);

    actUndo = new QAction(tr("Undo"), this);
    actUndo->setStatusTip(tr("Undo"));
    actUndo->setIcon( QIcon( ":/icons/undo.svg"));
    actUndo->setShortcut(QKeySequence::Undo);

    actRedo = new QAction(tr("Redo"), this);
    actRedo->setStatusTip(tr("Redo"));
    actRedo->setIcon( QIcon( ":/icons/redo.svg"));
    actRedo->setShortcut(QKeySequence::Redo);

    actEditCopy = new QAction(tr("Copy"), this);
    actEditCopy->setStatusTip(tr("Copy Selected Object"));
    actEditCopy->setIcon( QIcon( ":/icons/editcopy.svg"));

    actEditCut = new QAction(tr("Cut"), this);
    actEditCut->setStatusTip(tr("Cut Selected Object"));
    actEditCut->setIcon( QIcon( ":/icons/editcut.svg"));

    actEditDelete = new QAction(tr("Delete"), this);
    actEditDelete->setStatusTip(tr("Delete Selected Object"));
    actEditDelete->setIcon( QIcon( ":/icons/trash.svg"));

    actEditRename = new QAction(tr("Rename"), this);
    actEditRename->setStatusTip(tr("Rename Selected Object"));
    actEditRename->setIcon( QIcon( ":/icons/editrename.svg"));
    actEditRename->setShortcut(QKeySequence("F2"));

    actEditPaste = new QAction(tr("Paste"), this);
    actEditPaste->setStatusTip(tr("Paste Object From Clipboard"));
    actEditPaste->setIcon( QIcon( ":/icons/editpaste.svg"));

    actEditCopyToTemplate = new QAction(tr("Copy to Template"), this);
    actEditCopyToTemplate->setStatusTip(tr("Copy Object as a Template"));
    actEditCopyToTemplate->setIcon( QIcon( ":/icons/editcopy.svg"));

    actEditCopyToTemplateWithChildren = new QAction(tr("Copy to Template with Children"), this);
    actEditCopyToTemplateWithChildren->setStatusTip(tr("Copy Object and Children as a Template"));
    actEditCopyToTemplateWithChildren->setIcon( QIcon( ":/icons/editcopy.svg"));

    actFullScreen = new QAction(tr("Fullscreen"), this);
    actFullScreen->setStatusTip(tr("Fullscreen"));
    actFullScreen->setIcon(QIcon(":/icons/fullscreen.svg"));
    actFullScreen->setCheckable(true);
    actFullScreen->setShortcut(QKeySequence("F11"));

    actSuperFullScreen = new QAction(tr("Super Fullscreen"), this);
    actSuperFullScreen->setStatusTip(tr("Super Fullscreen"));
    actSuperFullScreen->setIcon(QIcon(":/icons/fullscreen_super.svg"));
    actSuperFullScreen->setCheckable(true);
    actSuperFullScreen->setShortcut(QKeySequence("F12"));

    actIncreaseGizmoScale = new QAction(tr("Increase Gizmo Scale"), this);
    actIncreaseGizmoScale->setStatusTip(tr("Increase Gizmo Scale"));
    actIncreaseGizmoScale->setIcon(QIcon(":/icons/sign_plus.svg"));
    actIncreaseGizmoScale->setShortcut(QKeySequence("+"));

    actDecreaseGizmoScale = new QAction(tr("Decrease Gizmo Scale"), this);
    actDecreaseGizmoScale->setStatusTip(tr("Decrease Gizmo Scale"));
    actDecreaseGizmoScale->setIcon(QIcon(":/icons/sign_minus.svg"));
    actDecreaseGizmoScale->setShortcut(QKeySequence("-"));

    QSettings settings;
    QString style = settings.value("preferences/customStyleSheet").toString();
    
	actToggleWorldSpaceGizmo = new QAction(tr("World Space Gizmo"), this);
    actToggleWorldSpaceGizmo->setStatusTip(tr("Toggle World Space Gizmo"));
    
    QString orient_icon = "";
    if(style == ":/stylesheets/dark.qss") {
        orient_icon = QString::fromLatin1(":/icons/orient_obj_dark_style.svg");
    } else {
        orient_icon = QString::fromLatin1(":/icons/orient_obj.svg");
    }
	actToggleWorldSpaceGizmo->setIcon(QIcon(orient_icon));
    actToggleWorldSpaceGizmo->setCheckable(true);

    actToggleWalkAround = new QAction(tr("Toggle Walk Around Mode"), this);
    actToggleWalkAround->setStatusTip(tr("Toggle Walk Around Mode"));

    QString fly_icon = "";
    if(style == ":/stylesheets/dark.qss") {
        fly_icon = QString::fromLatin1(":/icons/mode_fly_dark_style.svg");
    } else {
        fly_icon = QString::fromLatin1(":/icons/mode_fly.svg");
    }
	
	actToggleWalkAround->setIcon(QIcon(fly_icon));
    actToggleWalkAround->setCheckable(true);

    actToggleGrid = new QAction(tr("Grid"), this);
    actToggleGrid->setStatusTip(tr("Show/Hide Grid"));
    actToggleGrid->setIcon(QIcon(":/icons/grid.svg"));
    actToggleGrid->setCheckable(true);
    actToggleGrid->setChecked(true);
    actToggleGrid->setShortcut(QKeySequence("\""));

    actToggleExplorer = explorerDockWidget->toggleViewAction();
    actToggleLayer = layerDockWidget->toggleViewAction();
    actToggleResources = resourcesDockWidget->toggleViewAction();
    actToggleProperties = propertiesDockWidget->toggleViewAction();
    actToggleTools = toolsDockWidget->toggleViewAction();
    actToggleLog = logDockWidget->toggleViewAction();

    actToggleTools->setIcon(QIcon(":/icons/objects.svg"));
    actToggleTools->setText(tr("Tools"));
    actToggleTools->setStatusTip(tr("Toggle Tools Panel"));
    actToggleTools->setShortcut(QKeySequence("F4"));
    actToggleExplorer->setIcon(QIcon(":/icons/scene.svg"));
    actToggleExplorer->setText(tr("Explorer"));
    actToggleExplorer->setStatusTip(tr("Toggle Explorer Panel"));
    actToggleExplorer->setShortcut(QKeySequence("F5"));
    actToggleLayer->setIcon(QIcon(":/icons/visibility.svg"));
    actToggleLayer->setText(tr("Groups"));
    actToggleLayer->setStatusTip(tr("Toggle Groups Panel"));
    actToggleLayer->setShortcut(QKeySequence("F6"));
    actToggleResources->setIcon(QIcon(":/icons/entity.svg"));
    actToggleResources->setText(tr("Resources"));
    actToggleResources->setStatusTip(tr("Toggle Resources Panel"));
    actToggleResources->setShortcut(QKeySequence("F7"));
    actToggleProperties->setIcon(QIcon(":/icons/properties.svg"));
    actToggleProperties->setText(tr("Properties"));
    actToggleProperties->setStatusTip(tr("Toggle Properties Panel"));
    actToggleProperties->setShortcut(QKeySequence("F8"));
    actToggleLog->setIcon(QIcon(":/icons/messages.svg"));
    actToggleLog->setText(tr("Messages"));
    actToggleLog->setStatusTip(tr("Toggle Messages"));
    actToggleLog->setShortcut(QKeySequence("F9"));
    actOpenPreferences = new QAction(tr("Preferences"), this);
    actOpenPreferences->setIcon(QIcon(":/icons/preferences.svg"));
    actOpenPreferences->setStatusTip(tr("Preferences"));
    actToggleToolBar = new QAction(tr("ToolBar"), this);
    actToggleToolBar->setStatusTip(tr("Toggle ToolBar"));
    actToggleToolBar->setIcon( QIcon( ":/icons/toolbar.svg"));
    actToggleToolBar->setCheckable(true);
    actToggleToolBar->setShortcut(QKeySequence("F10"));

    actSearchLog = new QAction(tr("Search"), this);;
    actSearchLog->setStatusTip(tr("Search Message Log"));
    actSearchLog->setIcon( QIcon( ":/icons/search.svg" ));
    actClearLog = new QAction(tr("Clear"), this);;
    actClearLog->setStatusTip(tr("Clear Message Log"));
    actClearLog->setIcon( QIcon( ":/icons/clear.svg" ));
    actLogShowWarnings = new QAction(tr("Warning"), this);;
    actLogShowWarnings->setStatusTip(tr("Show Warning Messages"));
    actLogShowWarnings->setIcon( QIcon( ":/icons/warning.svg" ));
    actLogShowWarnings->setCheckable(true);
    actLogShowWarnings->setChecked(true);
    actLogShowErrors = new QAction(tr("Error"), this);;
    actLogShowErrors->setStatusTip(tr("Show Error Messages"));
    actLogShowErrors->setIcon( QIcon( ":/icons/error.svg" ));
    actLogShowErrors->setCheckable(true);
    actLogShowErrors->setChecked(true);
    actLogShowInfo = new QAction(tr("Info"), this);;
    actLogShowInfo->setStatusTip(tr("Show Info Messages"));
    actLogShowInfo->setIcon( QIcon( ":/icons/info.svg" ));
    actLogShowInfo->setCheckable(true);
    actLogShowInfo->setChecked(true);
    actLogShowDebug = new QAction(tr("Debug"), this);;
    actLogShowDebug->setStatusTip(tr("Show Debug Messages"));
    actLogShowDebug->setIcon( QIcon( ":/icons/debuginfo.svg" ));
    actLogShowDebug->setCheckable(true);
    actLogShowDebug->setChecked(true);

    connect(actSearchLog, SIGNAL(triggered()), this, SLOT(searchLog()));
    connect(actClearLog, SIGNAL(triggered()), logWidget, SLOT(clear()));
    connect(actLogShowWarnings, SIGNAL(triggered()), this, SLOT(toggleLogMessages()));
    connect(actLogShowErrors, SIGNAL(triggered()), this, SLOT(toggleLogMessages()));
    connect(actLogShowInfo, SIGNAL(triggered()), this, SLOT(toggleLogMessages()));
    connect(actLogShowDebug, SIGNAL(triggered()), this, SLOT(toggleLogMessages()));

    actCamSave = new QAction(tr("Save\nPosition"), this);
    actCamSave->setStatusTip(tr("Save Camera Position"));
    actCamSave->setIcon( QIcon( ":/icons/camsave.svg" ));

    actCamSpeedMinus = new QAction(tr("Speed-"), this);
    actCamSpeedMinus->setStatusTip(tr("Decrease Camera Speed"));
    actCamSpeedMinus->setIcon( QIcon( ":/icons/camspeedminus.svg" ));

    actCamSpeedPlus = new QAction(tr("Speed+"), this);
    actCamSpeedPlus->setStatusTip(tr("Increase Camera Speed"));
    actCamSpeedPlus->setIcon( QIcon( ":/icons/camspeedplus.svg" ));

    menuCamPolyMode = new QMenu(tr("Polygon\nMode"), this);
    menuCamPolyMode->setStatusTip(tr("Change Camera Polygon Mode"));
    menuCamPolyMode->setIcon( QIcon( ":/icons/polymode.svg" ));

    QSignalMapper *polymapper = new QSignalMapper(this);

    actCamModeSolid = menuCamPolyMode->addAction(tr("Solid"));
    actCamModeSolid->setStatusTip(tr("Set Polygon Mode to Solid"));
    actCamModeSolid->setIcon( QIcon( ":icons/pmsolid.svg" ));
    actCamModeSolid->setCheckable(true);
    connect(actCamModeSolid, SIGNAL(triggered()), polymapper, SLOT(map()));
    polymapper->setMapping(actCamModeSolid, (int)0 );

    actCamModeWireframe = menuCamPolyMode->addAction(tr("Wireframe"));
    actCamModeWireframe->setStatusTip(tr("Set Polygon Mode to Wireframe"));
    actCamModeWireframe->setIcon( QIcon( ":icons/pmwireframe.svg" ));
    actCamModeWireframe->setCheckable(true);
    connect(actCamModeWireframe, SIGNAL(triggered()), polymapper, SLOT(map()));
    polymapper->setMapping(actCamModeWireframe, (int)1 );

    actCamModePoints = menuCamPolyMode->addAction(tr("Points"));
    actCamModePoints->setStatusTip(tr("Set Polygon Mode to Points"));
    actCamModePoints->setIcon( QIcon( ":icons/pmpoints.svg" ));
    actCamModePoints->setCheckable(true);
    connect(actCamModePoints, SIGNAL(triggered()), polymapper, SLOT(map()));
    polymapper->setMapping(actCamModePoints, (int)2 );

    connect(polymapper, SIGNAL(mapped( int )), this, SLOT(setCameraPolyMode( int )));

    QSignalMapper *selmapper = new QSignalMapper(this);

    for(unsigned int sa = 0;sa < 10;sa++)
    {
        mSelectActions[sa] = new QAction(tr("Define Selection List") + QString::number(sa), this);
        mSelectActions[sa]->setShortcut(QKeySequence(QString("Ctrl+%1").arg(sa)));

        mSelectActions[sa + 10] = new QAction(tr("Select Selection List") + QString::number(sa), this);
        mSelectActions[sa + 10]->setShortcut(QKeySequence(QString("Alt+%1").arg(sa)));

        connect(mSelectActions[sa], SIGNAL(triggered()), selmapper, SLOT(map()));
        selmapper->setMapping(mSelectActions[sa], sa );
        connect(mSelectActions[sa + 10], SIGNAL(triggered()), selmapper, SLOT(map()));
        selmapper->setMapping(mSelectActions[sa + 10], sa + 10 );
    }

    connect(selmapper, SIGNAL(mapped( int )), this, SLOT(selectActionTriggered( int )));



    connect(actSaveLayout, SIGNAL(triggered()), this, SLOT(saveLayout()));
    connect(actLoadLayout, SIGNAL(triggered()), this, SLOT(loadLayout()));
    connect(actCamSave, SIGNAL(triggered()), this, SLOT(saveCamera()));
    connect(actCamSpeedPlus, SIGNAL(triggered()), this, SLOT(incCamera()));
    connect(actCamSpeedMinus, SIGNAL(triggered()), this, SLOT(decCamera()));
    connect(actUndo, SIGNAL(triggered()), this, SLOT(cmdUndo()));
    connect(actRedo, SIGNAL(triggered()), this, SLOT(cmdRedo()));
    connect(actAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
    connect(actHelp, SIGNAL(triggered()), this, SLOT(showHelp()));
    connect(actNew, SIGNAL(triggered()), this, SLOT(newScene()));
    connect(actOpen, SIGNAL(triggered()), this, SLOT(openScene()));
    connect(actClose, SIGNAL(triggered()), this, SLOT(closeScene()));
    connect(actSave, SIGNAL(triggered()), this, SLOT(saveScene()));
    connect(actSaveAs, SIGNAL(triggered()), this, SLOT(saveSceneAs()));
    connect(actFullScreen, SIGNAL(triggered()), this, SLOT(toggleFullScreen()));
    connect(actSuperFullScreen, SIGNAL(triggered()), this, SLOT(toggleSuperFullScreen()));
    connect(actExit, SIGNAL(triggered()), this, SLOT(exitApp()));
    connect(actSelect, SIGNAL(triggered()), this, SLOT(setToolSelect()));
    connect(actMove, SIGNAL(triggered()), this, SLOT(setToolMove()));
    connect(actRotate, SIGNAL(triggered()), this, SLOT(setToolRotate()));
    connect(actScale, SIGNAL(triggered()), this, SLOT(setToolScale()));
    connect(actDeform, SIGNAL(triggered()), this, SLOT(setToolDeform()));
    connect(actSmooth, SIGNAL(triggered()), this, SLOT(setToolSmooth()));
    connect(actSplat, SIGNAL(triggered()), this, SLOT(setToolSplat()));
    connect(actSplatGrass, SIGNAL(triggered()), this, SLOT(setToolSplatGrass()));
    connect(actPaint, SIGNAL(triggered()), this, SLOT(setToolPaint()));
    connect(actReLight, SIGNAL(triggered()), this, SLOT(relightTerrain()));
    connect(actToggleToolBar, SIGNAL(triggered(bool)), this, SLOT(toggleToolBar(bool)));
    connect(actOpenPreferences, SIGNAL(triggered()), this, SLOT(openPreferences()));
    connect(actSceneOptions, SIGNAL(triggered()), this, SLOT(openSceneOptions()));
    connect(actEditCopy, SIGNAL(triggered()), this, SLOT(editCopy()));
    connect(actEditCut, SIGNAL(triggered()), this, SLOT(editCut()));
    connect(actEditPaste, SIGNAL(triggered()), this, SLOT(editPaste()));
    connect(actEditDelete, SIGNAL(triggered()), this, SLOT(editDelete()));
    connect(actEditRename, SIGNAL(triggered()), this, SLOT(editRename()));
    connect(actEditCopyToTemplate, SIGNAL(triggered()), this, SLOT(editCopyToTemplate()));
    connect(actEditCopyToTemplateWithChildren, SIGNAL(triggered()), this, SLOT(editCopyToTemplateWithChildren()));
    connect(actIncreaseGizmoScale, SIGNAL(triggered()), this, SLOT(increaseGizmoScale()));
    connect(actDecreaseGizmoScale, SIGNAL(triggered()), this, SLOT(decreaseGizmoScale()));
    connect(actToggleGrid, SIGNAL(triggered()), this, SLOT(toggleGrid()));
    connect(actToggleWorldSpaceGizmo, SIGNAL(triggered()), this, SLOT(toggleWorldSpaceGizmo()));
    connect(actToggleWalkAround, SIGNAL(triggered()), this, SLOT(toggleWalkAround()));

    QMetaObject::connectSlotsByName(this);
}
//------------------------------------------------------------------------------
void MainWindow::updateActions()
{
    bool loaded = OgitorsRoot::getSingletonPtr()->IsSceneLoaded();

    if(!loaded)
        return;

    CViewportEditor *viewport = OgitorsRoot::getSingletonPtr()->GetViewport();
    
    if(viewport && viewport->getCameraEditor())
    {
        int mode = viewport->getCamPolyMode();
        actCamModeSolid->setChecked(mode == Ogre::PM_SOLID);
        actCamModeWireframe->setChecked(mode == Ogre::PM_WIREFRAME);
        actCamModePoints->setChecked(mode == Ogre::PM_POINTS);
        mCameraViewModeBox->setCurrentIndex(viewport->getCameraEditor()->getViewMode());
    }
}
//------------------------------------------------------------------------------
void MainWindow::updateLoadTerminateActions(bool loaded)
{
    menuImport->setEnabled(menuImport->actions().size() > 0);
    
    if(!loaded)
    {
        actEditRename->setEnabled(false);
        actSave->setEnabled(false);
        menuExport->setEnabled(false);
        actUndo->setEnabled(false);
        actRedo->setEnabled(false);
        actToggleWalkAround->setEnabled(false);

        for(int i = 0;i < 10;i++)
            menuCameraPositions[i]->setEnabled(false);

        actCamSave->setEnabled(false);
    }
    else
    {
        unsigned int tool = OgitorsRoot::getSingletonPtr()->GetEditorTool();
        actSelect->setChecked(tool == TOOL_SELECT);
        actMove->setChecked(tool == TOOL_MOVE);
        actRotate->setChecked(tool == TOOL_ROTATE);
        actScale->setChecked(tool == TOOL_SCALE);
    
        menuExport->setEnabled(menuExport->actions().size() > 0);

        Ogitors::PROJECTOPTIONS *pOpt = OgitorsRoot::getSingletonPtr()->GetProjectOptions();
        for(int i = 0;i < 10;i++)
        {
            menuCameraPositions[i]->setEnabled(i < pOpt->CameraSaveCount);
        }

        actCamSave->setEnabled(pOpt->CameraSaveCount < 10);
    }

    if(actPlayerRunPause)
    {
        actPlayerRunPause->setEnabled(loaded);
        if(!loaded)
            actPlayerStop->setEnabled(false);
    }

    menuDefineSelectionList->setEnabled(loaded);
    menuSelectSelectionList->setEnabled(loaded);
    actIncreaseGizmoScale->setEnabled(loaded);
    actDecreaseGizmoScale->setEnabled(loaded);
    mCameraViewModeBox->setEnabled(loaded);
#ifdef OPT_SHOW_HIDE_WINDOWS
    actSaveLayout->setEnabled(loaded);
    actLoadLayout->setEnabled(loaded);
#endif
    actClose->setEnabled(loaded);
    actSaveAs->setEnabled(loaded);
    actSceneOptions->setEnabled(loaded);
    actSelect->setEnabled(loaded);
    actMove->setEnabled(loaded);
    actRotate->setEnabled(loaded);
    actScale->setEnabled(loaded);
    actToggleWorldSpaceGizmo->setEnabled(loaded);
    actToggleWorldSpaceGizmo->setChecked(false);
	QSettings settings;
    QString style = settings.value("preferences/customStyleSheet").toString();
    QString orient_icon = "";
    if(style == ":/stylesheets/dark.qss") {
        orient_icon = QString::fromLatin1(":/icons/orient_obj_dark_style.svg");
    } else {
        orient_icon = QString::fromLatin1(":/icons/orient_obj.svg");
    }
    actToggleWorldSpaceGizmo->setIcon(QIcon(orient_icon));
    actToggleWalkAround->setEnabled(loaded);
    actToggleWalkAround->setChecked(false);
    
    QString fly_icon = "";
    if(style == ":/stylesheets/dark.qss") {
        fly_icon = QString::fromLatin1(":/icons/mode_fly_dark_style.svg");
    } else {
        fly_icon = QString::fromLatin1(":/icons/mode_fly.svg");
    }
    actToggleWalkAround->setIcon(QIcon(fly_icon));
    actCamSpeedMinus->setEnabled(loaded);
    actCamSpeedPlus->setEnabled(loaded);
    menuCamPolyMode->setEnabled(loaded);

    actEditCopy->setEnabled(loaded);
    actEditPaste->setEnabled(loaded);
    actEditCut->setEnabled(loaded);
    actEditDelete->setEnabled(loaded);
    actEditCopyToTemplate->setEnabled(loaded);
    actEditCopyToTemplateWithChildren->setEnabled(loaded);

    for(int i = 0; i < 10;i++)
        mSelectLists[i].clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//-----ACTION FUNCTIONS---------------------------------------------------------
//------------------------------------------------------------------------------
void MainWindow::fpsValueChanged ( int value )
{
    mTargetRenderCount = (value + 1) * 5;
    mFPSSliderLabel->setText(QString("FPS (%1)").arg(mTargetRenderCount));
}
//------------------------------------------------------------------------------
void MainWindow::showAbout()
{
    aboutdialog dialog(this, QString(OGITOR_VERSION));
    if(dialog.exec() == QDialog::Accepted)
        return;
}
//------------------------------------------------------------------------------
void MainWindow::showHelp()
{
    mOgitorAssistant->showDocumentation("index.html");
}
//------------------------------------------------------------------------------
void MainWindow::newScene()
{
    const char NewSceneDefinition[] = "  <OBJECT typename=\"%s\" scenemanagertype=\"%s\" name=\"SceneManager\"> <PROPERTY id=\"configfile\" type=\"7\" value=\"%s\"></PROPERTY></OBJECT>\n"
                                      "</OGITORSCENE>\n";

    Ogitors::OgitorsRoot *ogRoot = OgitorsRoot::getSingletonPtr();

    if(ogRoot->IsSceneLoaded())
        mOgreWidget->setSwitchingScene(true);

    if(!(ogRoot->TerminateScene()))
    {
        mOgreWidget->setSwitchingScene(false);
        return;
    }

    Ogitors::PROJECTOPTIONS *pOpt = ogRoot->GetProjectOptions();

    pOpt->IsNewProject = true;
    pOpt->ProjectName = "";
    pOpt->ProjectDir = Ogitors::OgitorsSystem::getSingletonPtr()->getProjectsDirectory();
    pOpt->SceneManagerName = "OctreeSceneManager";
    pOpt->TerrainDirectory = "Terrain";
    pOpt->HydraxDirectory = "Hydrax";
    pOpt->CaelumDirectory = "Caelum";
    pOpt->PagedGeometryDirectory = "PagedGeometry";
    pOpt->SceneManagerConfigFile = "";
    pOpt->CameraSaveCount = 0;
    pOpt->CameraSpeed = 1.0f;
    pOpt->ResourceDirectories.clear();
    pOpt->SelectionRectColour = Ogre::ColourValue(0.5f, 0, 1);
    pOpt->SelectionBBColour = Ogre::ColourValue(1, 1, 1);
    pOpt->HighlightBBColour = Ogre::ColourValue(0.91f, 0.19f, 0.19f);
    pOpt->SelectHighlightBBColour = Ogre::ColourValue(0.19f, 0.91f, 0.19f);
    pOpt->ObjectCount = 0;

    for(unsigned int v = 0;v < 31;v++)
    {
        pOpt->LayerNames[v] = "Layer " + Ogre::StringConverter::toString(v);
        pOpt->LayerVisible[v] = true;
    }
    
    pOpt->LayerCount = 1;

    SettingsDialog dlg(QApplication::activeWindow(), pOpt);
    
    if(dlg.exec() == QDialog::Accepted)
    {
        char buffer[5000];
        Ogre::String filename = ogRoot->GetProjectOptions()->ProjectDir + "/" + ogRoot->GetProjectOptions()->ProjectName + ".ofs";
        filename = Ogitors::OgitorsUtils::QualifyPath(filename);
        
        bool succeed = false;

        OFS::OfsPtr mFile;

        try
        {
            if(mFile.mount(filename.c_str(), OFS::OFS_MOUNT_CREATE) == OFS::OFS_OK)
                succeed = true;

            std::stringstream outfile;
            if(succeed)
            {
                outfile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
                outfile << "<OGITORSCENE version=\"2\">\n";
                ogRoot->WriteProjectOptions(outfile,true);
                sprintf_s(buffer,5000,NewSceneDefinition,ogRoot->GetProjectOptions()->SceneManagerName.c_str(),ogRoot->GetProjectOptions()->SceneManagerName.c_str(),ogRoot->GetProjectOptions()->SceneManagerConfigFile.c_str());
                outfile << buffer;

                OFS::OFSHANDLE handle;
                Ogre::String projfilename = "/";
                projfilename += ogRoot->GetProjectOptions()->ProjectName + ".ogscene";

                mFile->createFile(handle, projfilename.c_str(), outfile.tellp(), outfile.tellp(), outfile.str().c_str());
                mFile->closeFile(handle);
                mFile.unmount();
            }
        }
        catch(...)
        {
            succeed = false;
        }

        if(!succeed)
        {
            OgitorsSystem::getSingletonPtr()->DisplayMessageDialog("The path is Read-Only. Ogitor can not work with Read-Only Project Paths!", DLGTYPE_OK);
            return;
        }

        ogRoot->LoadScene(filename);
    }
    updateRecentFiles();
}
//------------------------------------------------------------------------------
void MainWindow::openScene()
{
    Ogitors::OgitorsRoot *ogRoot = OgitorsRoot::getSingletonPtr();

    if(ogRoot->IsSceneLoaded())
        mOgreWidget->setSwitchingScene(true);

    if(!(ogRoot->TerminateScene()))
    {
        mOgreWidget->setSwitchingScene(false);
        return;
    }

    int ret = ogRoot->LoadScene("");
    if(ret == SCF_CANCEL)
        return;

    if( ret != SCF_OK )
    {
        QMessageBox::warning(this,"qtOgitor", tr("Error Loading file") + ": " + CBaseSerializer::GetErrorString((SCENEFILERESULT)ret).c_str(), QMessageBox::Ok);
        return;
    }
    updateRecentFiles();
}
//------------------------------------------------------------------------------
void MainWindow::openRecentFile(const QString& value)
{
    if(!(OgitorsRoot::getSingletonPtr()->TerminateScene()))
        return;

    int ret = OgitorsRoot::getSingletonPtr()->LoadScene(value.toStdString());
    if(ret == SCF_CANCEL)
        return;

    if( ret != SCF_OK )
    {
        QMessageBox::warning(this,"qtOgitor", tr("Error Loading file"), QMessageBox::Ok);
        return;
    }
    updateRecentFiles();
}
//------------------------------------------------------------------------------
void MainWindow::closeScene()
{
    OgitorsRoot::getSingletonPtr()->TerminateScene();
}
//------------------------------------------------------------------------------
void MainWindow::exitApp()
{
    mUpdateLastSceneFile2 = OgitorsRoot::getSingletonPtr()->IsSceneLoaded();
    if(OgitorsRoot::getSingletonPtr()->TerminateScene())
        close();
}
//------------------------------------------------------------------------------
void MainWindow::saveScene(const QString& exportfile)
{
    ITerrainEditor *terED = OgitorsRoot::getSingletonPtr()->GetTerrainEditor();
    if(terED && terED->isBackgroundProcessActive())
    {
        if(QMessageBox::information(QApplication::activeWindow(), "qtOgitor", tr("Terrain is still making background calculations.") + "\n" + tr("Saving at this time may take much longer and cause temporary freeze.") + "\n" + tr("Do you want to continue?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
            return;
    }

    OgitorsRoot::getSingletonPtr()->SaveScene(false, exportfile.toStdString());
}
//------------------------------------------------------------------------------
void MainWindow::saveSceneAs()
{
    OgitorsRoot::getSingletonPtr()->SaveScene(true);
}
//------------------------------------------------------------------------------
void MainWindow::setToolSelect()
{
    CViewportEditor *vpe = OgitorsRoot::getSingletonPtr()->GetViewport();
    if(vpe)
        vpe->SetEditorTool(TOOL_SELECT);
    if(OgitorsRoot::getSingletonPtr()->GetTerrainEditor())
            OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->setEditMode(EM_NONE);
}
//------------------------------------------------------------------------------
void MainWindow::setToolMove()
{
    CViewportEditor *vpe = OgitorsRoot::getSingletonPtr()->GetViewport();
    if(vpe)
        vpe->SetEditorTool(TOOL_MOVE);
    if(OgitorsRoot::getSingletonPtr()->GetTerrainEditor())
            OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->setEditMode(EM_NONE);
}
//------------------------------------------------------------------------------
void MainWindow::setToolRotate()
{
    CViewportEditor *vpe = OgitorsRoot::getSingletonPtr()->GetViewport();
    if(vpe)
        vpe->SetEditorTool(TOOL_ROTATE);
    if(OgitorsRoot::getSingletonPtr()->GetTerrainEditor())
            OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->setEditMode(EM_NONE);
}
//------------------------------------------------------------------------------
void MainWindow::setToolScale()
{
    CViewportEditor *vpe = OgitorsRoot::getSingletonPtr()->GetViewport();
    if(vpe)
        vpe->SetEditorTool(TOOL_SCALE);
    if(OgitorsRoot::getSingletonPtr()->GetTerrainEditor())
            OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->setEditMode(EM_NONE);
}
//------------------------------------------------------------------------------
void MainWindow::setToolDeform()
{
    CViewportEditor *vpe = OgitorsRoot::getSingletonPtr()->GetViewport();
    if(vpe)
        vpe->SetEditorTool(TOOL_DEFORM);
    OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->setEditMode(EM_DEFORM);
}
//------------------------------------------------------------------------------
void MainWindow::setToolSmooth()
{
    CViewportEditor *vpe = OgitorsRoot::getSingletonPtr()->GetViewport();
    if(vpe)
        vpe->SetEditorTool(TOOL_SMOOTH);
    OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->setEditMode(EM_SMOOTH);
}
//------------------------------------------------------------------------------
void MainWindow::setToolSplat()
{
    CViewportEditor *vpe = OgitorsRoot::getSingletonPtr()->GetViewport();
    if(vpe)
        vpe->SetEditorTool(TOOL_SPLAT);
    OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->setEditMode(EM_SPLAT);
}
//------------------------------------------------------------------------------
void MainWindow::setToolSplatGrass()
{
    CViewportEditor *vpe = OgitorsRoot::getSingletonPtr()->GetViewport();
    if(vpe)
        vpe->SetEditorTool(TOOL_SPLATGRASS);
    OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->setEditMode(EM_SPLATGRASS);
}
//------------------------------------------------------------------------------
void MainWindow::setToolPaint()
{
    CViewportEditor *vpe = OgitorsRoot::getSingletonPtr()->GetViewport();
    if(vpe)
        vpe->SetEditorTool(TOOL_PAINT);
    OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->setEditMode(EM_PAINT);
}
//------------------------------------------------------------------------------
void MainWindow::relightTerrain()
{
    OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->recalculateLighting();
}
//------------------------------------------------------------------------------
void MainWindow::cmdUndo()
{
    if(OgitorsUndoManager::getSingletonPtr())
    {
        OgitorsUndoManager::getSingletonPtr()->Undo();
        actUndo->setToolTip(OgitorsUndoManager::getSingletonPtr()->GetUndoString().c_str());
        actRedo->setToolTip(OgitorsUndoManager::getSingletonPtr()->GetRedoString().c_str());
        actUndo->setEnabled(OgitorsUndoManager::getSingletonPtr()->CanUndo());
        actRedo->setEnabled(OgitorsUndoManager::getSingletonPtr()->CanRedo());
    }
}
//------------------------------------------------------------------------------
void MainWindow::cmdRedo()
{
    if(OgitorsUndoManager::getSingletonPtr())
    {
        OgitorsUndoManager::getSingletonPtr()->Redo();
        actUndo->setToolTip(OgitorsUndoManager::getSingletonPtr()->GetUndoString().c_str());
        actRedo->setToolTip(OgitorsUndoManager::getSingletonPtr()->GetRedoString().c_str());
        actUndo->setEnabled(OgitorsUndoManager::getSingletonPtr()->CanUndo());
        actRedo->setEnabled(OgitorsUndoManager::getSingletonPtr()->CanRedo());
    }
}
//------------------------------------------------------------------------------
bool wasMaximized;

void MainWindow::toggleFullScreen()
{
    if(actSuperFullScreen->isChecked())
    {
        actSuperFullScreen->setChecked(false);

        mMenuBar->setVisible(true);
        for(unsigned int i = 0;i < mSubWindowStateSave.size();i++)
        {
             mSubWindowStateSave[i]->setVisible(true);
        }
        mSubWindowStateSave.clear();
    }

    if(actFullScreen->isChecked())
    {
        if(!isFullScreen())
        {
            wasMaximized = isMaximized();
            showFullScreen();
        }
    }
    else
    {
        showNormal();

        if(wasMaximized)
            setWindowState(Qt::WindowMaximized);
            //showMaximized();
    }
}
//------------------------------------------------------------------------------
void MainWindow::toggleSuperFullScreen()
{
    if(actFullScreen->isChecked())
        actFullScreen->setChecked(false);

    if(actSuperFullScreen->isChecked())
    {
        mSubWindowStateSave.clear();

        if(!isFullScreen())
        {
            wasMaximized = isMaximized();
            showFullScreen();
        }

        mMenuBar->setVisible(false);
        actToggleToolBar->setChecked(false);

        const QObjectList& childwins = children();
        for(int i = 0;i < childwins.size();i++)
        {
            if(childwins[i]->isWidgetType())
            {
                QWidget *widget = static_cast<QWidget*>(childwins[i]);
                if(widget->objectName().endsWith("DockWidget", Qt::CaseInsensitive) && widget->isVisible())
                {
                    widget->setVisible(false);
                    mSubWindowStateSave.push_back(widget);
                }
            }
        }
    }
    else
    {
        showNormal();

        if(wasMaximized)
            setWindowState(Qt::WindowMaximized);
            //showMaximized();

        for(unsigned int i = 0;i < mSubWindowStateSave.size();i++)
        {
             mSubWindowStateSave[i]->setVisible(true);
        }
        mSubWindowStateSave.clear();
        mMenuBar->setVisible(true);
    }
}
//------------------------------------------------------------------------------
void MainWindow::snapMultiplierIndexChanged ( int index )
{
    CViewportEditor::SetSnapMultiplier(index + 1);
}
//------------------------------------------------------------------------------
void MainWindow::viewModeIndexChanged ( int index )
{
    CViewportEditor *ovp = OgitorsRoot::getSingletonPtr()->GetViewport();
    if(ovp && ovp->getCameraEditor())
        ovp->getCameraEditor()->setViewMode(index);
}
//------------------------------------------------------------------------------
void MainWindow::toggleGrid()
{
    bool bShow = actToggleGrid->isChecked();

    NameObjectPairList list = OgitorsRoot::getSingletonPtr()->GetObjectsByType(ETYPE_VIEWPORT);

    NameObjectPairList::iterator it = list.begin();
    while(it != list.end())
    {
        static_cast<CViewportEditor*>(it->second)->ShowGrid(bShow);
        it++;
    }
}
//------------------------------------------------------------------------------
void MainWindow::toggleToolBar(bool b)
{
}
//------------------------------------------------------------------------------
void MainWindow::exportSerializer(const QString& value)
{
    try
    {
        OgitorsRoot::getSingletonPtr()->TriggerExportSerializer(value.toStdString());
        Ogitors::AfterSceneExportEvent evt(value.toStdString());
        EventManager::getSingletonPtr()->sendEvent(this, 0, &evt);
    }
    catch(...)
    {
        OgitorsSystem::getSingletonPtr()->DisplayMessageDialog(tr("An Error Occured trying to Export File").toStdString(), DLGTYPE_OK);
    }
}
//------------------------------------------------------------------------------
void MainWindow::importSerializer(const QString& value)
{
    try
    {
        OgitorsRoot::getSingletonPtr()->TriggerImportSerializer(value.toStdString());
        updateRecentFiles();
    }
    catch(...)
    {
        OgitorsSystem::getSingletonPtr()->DisplayMessageDialog(tr("An Error Occured trying to Import File").toStdString(), DLGTYPE_OK);
    }
}
//------------------------------------------------------------------------------
void MainWindow::incCamera()
{
   CViewportEditor *ovp = OgitorsRoot::getSingletonPtr()->GetViewport();
   if(ovp)
   {
       ovp->MultiplyCameraSpeed(1.5f);
       mCameraSpeedSlider->setToolTip(QString("%1").arg((int)ovp->GetCameraSpeed()));
       mCameraSpeedSlider->setSliderPosition(std::min((int)ovp->GetCameraSpeed(), 100));
   }
}
//------------------------------------------------------------------------------
void MainWindow::decCamera()
{
   CViewportEditor *ovp = OgitorsRoot::getSingletonPtr()->GetViewport();
   if(ovp)
   {
       ovp->MultiplyCameraSpeed(1.0f / 1.5f);
       mCameraSpeedSlider->setToolTip(QString("%1").arg((int)ovp->GetCameraSpeed()));
       mCameraSpeedSlider->setSliderPosition(std::min((int)ovp->GetCameraSpeed(), 100));
   }
}
//------------------------------------------------------------------------------
void MainWindow::cameraSpeedValueChanged ( int value )
{
   CViewportEditor *ovp = OgitorsRoot::getSingletonPtr()->GetViewport();
   if(ovp)
       ovp->SetCameraSpeed( (float)value );
   mCameraSpeedSlider->setToolTip(QString("%1").arg(value));
}
//------------------------------------------------------------------------------
void MainWindow::saveCamera()
{
    Ogitors::PROJECTOPTIONS *pOpt = OgitorsRoot::getSingletonPtr()->GetProjectOptions();
    if(pOpt->CameraSaveCount == 10)
        return;
    if(!(OgitorsRoot::getSingletonPtr()->GetViewport()->getCameraEditor()->getCamera()))
        return;

    pOpt->CameraPositions[pOpt->CameraSaveCount] = OgitorsRoot::getSingletonPtr()->GetViewport()->getCameraEditor()->getCamera()->getDerivedPosition();
    pOpt->CameraOrientations[pOpt->CameraSaveCount] = OgitorsRoot::getSingletonPtr()->GetViewport()->getCameraEditor()->getCamera()->getDerivedOrientation();

    QString menustr;
    menustr = QString(tr("Camera %1: <%2, %3, %4>")).arg(pOpt->CameraSaveCount).arg(pOpt->CameraPositions[pOpt->CameraSaveCount].x)
                                                    .arg(pOpt->CameraPositions[pOpt->CameraSaveCount].y).arg(pOpt->CameraPositions[pOpt->CameraSaveCount].z);
    menuCameraPositions[pOpt->CameraSaveCount]->setTitle(menustr);
    pOpt->CameraSaveCount++;

    for(int i = 0;i < 10;i++)
    {
        menuCameraPositions[i]->setEnabled(i < pOpt->CameraSaveCount);
    }

    actCamSave->setEnabled(pOpt->CameraSaveCount < 10);
    OgitorsRoot::getSingletonPtr()->SetSceneModified(true);
}
//------------------------------------------------------------------------------
void MainWindow::deleteCamera(int id)
{
    Ogitors::PROJECTOPTIONS *pOpt = OgitorsRoot::getSingletonPtr()->GetProjectOptions();
    if(pOpt->CameraSaveCount == 0) 
        return;

    for(int i = id;i < pOpt->CameraSaveCount;i++)
    {
        pOpt->CameraPositions[i] = pOpt->CameraPositions[i + 1];
        pOpt->CameraOrientations[i] = pOpt->CameraOrientations[i + 1];
    
        QString menustr;
        menustr = QString(tr("Camera %1: <%2, %3, %4>")).arg(i).arg(pOpt->CameraPositions[i].x)
                                                        .arg(pOpt->CameraPositions[i].y).arg(pOpt->CameraPositions[i].z);
        menuCameraPositions[i]->setTitle(menustr);
    }
    menuCameraPositions[pOpt->CameraSaveCount - 1]->setTitle(tr("(Empty Slot)"));
    pOpt->CameraSaveCount--;
    OgitorsRoot::getSingletonPtr()->SetSceneModified(true);
}
//------------------------------------------------------------------------------------
void MainWindow::setCameraPolyMode(int value)
{
   CViewportEditor *ovp = OgitorsRoot::getSingletonPtr()->GetViewport();
   if(ovp && ovp->getCameraEditor())
   {
       switch(value)
       {
       case 0:ovp->getCameraEditor()->setPolygonMode(Ogre::PM_SOLID);break;
       case 1:ovp->getCameraEditor()->setPolygonMode(Ogre::PM_WIREFRAME);break;
       case 2:ovp->getCameraEditor()->setPolygonMode(Ogre::PM_POINTS);break;
       }
   }
}
//------------------------------------------------------------------------------
void MainWindow::cameraAction(int id)
{
    PROJECTOPTIONS *pOpt = OgitorsRoot::getSingletonPtr()->GetProjectOptions();
    if(pOpt->CameraSaveCount == 0) 
        return;

    if(id > 99) //Remove Camera Position
    {
        deleteCamera(id - 100);
    }
    else //Goto Camera Position
    {
        if(OgitorsRoot::getSingletonPtr()->GetViewport()->getCameraEditor() && id < pOpt->CameraSaveCount)
        {
            OgitorsRoot::getSingletonPtr()->GetViewport()->getCameraEditor()->setPosition(pOpt->CameraPositions[id]);
            OgitorsRoot::getSingletonPtr()->GetViewport()->getCameraEditor()->setOrientation(pOpt->CameraOrientations[id]);
        }
    }
}
//------------------------------------------------------------------------------
void MainWindow::increaseGizmoScale()
{
    Ogre::Real scale = OgitorsRoot::getSingletonPtr()->GetGizmoScale();
    scale += 0.15f;
    if(scale > 4.0f)
        scale = 4.0f;

    OgitorsRoot::getSingletonPtr()->SetGizmoScale(scale);
}
//------------------------------------------------------------------------------
void MainWindow::decreaseGizmoScale()
{
    Ogre::Real scale = OgitorsRoot::getSingletonPtr()->GetGizmoScale();
    scale -= 0.15f;
    if(scale < 0.25f)
        scale = 0.25f;

    OgitorsRoot::getSingletonPtr()->SetGizmoScale(scale);
}
//------------------------------------------------------------------------------
void MainWindow::toggleLogMessages()
{
    for (int i = 0; i < logWidget->count(); ++i)
    {
        QListWidgetItem* item = logWidget->item(i);
        switch( item->type() )
        {
        case 4:
            // Warning
            item->setHidden(!actLogShowWarnings->isChecked());
            break;
        case 3:
            // Error
            item->setHidden(!actLogShowErrors->isChecked());
            break;
        case 2:
            // Info
            item->setHidden(!actLogShowInfo->isChecked());
            break;
        case 1:
            // Debug
            item->setHidden(!actLogShowDebug->isChecked());
            break;
        default:
            break;
        }
    }
}
//------------------------------------------------------------------------------
void MainWindow::searchLog()
{
    QString text = QInputDialog::getText(this, tr("Search Ogitor Messages"), tr("What : "));
    if (!text.isEmpty())
    {
        QList<QListWidgetItem*> items = logWidget->findItems(text, Qt::MatchContains);
        if (items.isEmpty())
            mStatusBar->showMessage(tr("%1 not found in Ogitor Messages").arg(text), 5000);
        else
        {
            logDockWidget->setVisible(true);
            int nextIndex = 0;
            for (int i = 0; i < items.size(); ++i)
            {
                if (logWidget->row(items[i]) > logWidget->currentRow())
                {
                    nextIndex = i;
                    break;
                }
            }
            logWidget->setCurrentRow(logWidget->row(items[nextIndex]));
        }
    }
}
//------------------------------------------------------------------------------
void MainWindow::openPreferences()
{
    mPrefManager->showDialog();
}
//------------------------------------------------------------------------------
void MainWindow::openSceneOptions()
{
    Ogitors::OgitorsRoot *ogRoot = OgitorsRoot::getSingletonPtr();
    Ogitors::PROJECTOPTIONS *pOpt = ogRoot->GetProjectOptions();

    SettingsDialog dlg(QApplication::activeWindow(), pOpt);
    
    Ogre::StringVector directories = pOpt->ResourceDirectories;

    if(dlg.exec() == QDialog::Accepted)
    {
        bool identical = true;
        if(directories.size() == pOpt->ResourceDirectories.size())
        {
            for(unsigned int i = 0;i < directories.size();i++)
            {
                bool found = false;
                for(unsigned int j = 0;j < directories.size();j++)
                {
                    if(directories[i] == pOpt->ResourceDirectories[i])
                        found = true;
                }
                if(!found)
                {
                    identical = false;
                    break;
                }
            }
        }
        else
            identical = false;

        if(!identical)
        {
            Ogitors::OgitorsRoot::getSingletonPtr()->ReloadUserResources();
            mEntityViewWidget->prepareView();
            mTerrainToolsWidget->updateTools();
        }
    }

    Ogre::MaterialPtr matPtr = Ogre::MaterialManager::getSingleton().getByName("SelectionBBMaterial", "General");
    if(!matPtr.isNull())
    {
        matPtr->getTechnique(0)->getPass(0)->setDiffuse(pOpt->SelectionBBColour);
        matPtr->getTechnique(0)->getPass(0)->setAmbient(pOpt->SelectionBBColour);
        matPtr->getTechnique(0)->getPass(0)->setSelfIllumination(pOpt->SelectionBBColour);
        matPtr.setNull();
    }
    
    matPtr = Ogre::MaterialManager::getSingleton().getByName("HighlightBBMaterial", "General");
    if(!matPtr.isNull())
    {
        matPtr->getTechnique(0)->getPass(0)->setDiffuse(pOpt->HighlightBBColour);
        matPtr->getTechnique(0)->getPass(0)->setAmbient(pOpt->HighlightBBColour);
        matPtr->getTechnique(0)->getPass(0)->setSelfIllumination(pOpt->HighlightBBColour);
        matPtr.setNull();
    }

    matPtr = Ogre::MaterialManager::getSingleton().getByName("SelectHighlightBBMaterial", "General");
    if(!matPtr.isNull())
    {
        matPtr->getTechnique(0)->getPass(0)->setDiffuse(pOpt->SelectHighlightBBColour);
        matPtr->getTechnique(0)->getPass(0)->setAmbient(pOpt->SelectHighlightBBColour);
        matPtr->getTechnique(0)->getPass(0)->setSelfIllumination(pOpt->SelectHighlightBBColour);
        matPtr.setNull();
    }

    matPtr = Ogre::MaterialManager::getSingleton().getByName("mtSELECTION", "EditorResources");
    if(!matPtr.isNull())
    {
        matPtr->getTechnique(0)->getPass(0)->setDiffuse(pOpt->SelectionRectColour);
        matPtr->getTechnique(0)->getPass(0)->setAmbient(pOpt->SelectionRectColour);
        matPtr->getTechnique(0)->getPass(0)->setSelfIllumination(pOpt->SelectionRectColour);
        matPtr.setNull();
    }

    Ogitors::ViewportGrid::setGridSpacing(pOpt->GridSpacing);
    Ogitors::ViewportGrid::setGridColour(pOpt->GridColour);
}
//------------------------------------------------------------------------------
void MainWindow::editCopy()
{
    CBaseEditor *object = OgitorsRoot::getSingletonPtr()->GetSelection()->getFirstObject();
    if(object)
        OgitorsClipboardManager::getSingletonPtr()->copy(object);
}
//------------------------------------------------------------------------------
void MainWindow::editCut()
{
    CBaseEditor *object = OgitorsRoot::getSingletonPtr()->GetSelection()->getFirstObject();
    if(object)
    {
        OgitorsClipboardManager::getSingletonPtr()->copy(object);
        OgitorsRoot::getSingletonPtr()->GetViewport()->DeleteSelectedObject(true, true);
    }
}
//------------------------------------------------------------------------------
void MainWindow::editPaste()
{
    CBaseEditor *object = OgitorsRoot::getSingletonPtr()->GetSelection()->getFirstObject();
    if(object)
    {
        OgitorsClipboardManager::getSingletonPtr()->paste(object);
    }
}
//------------------------------------------------------------------------------
void MainWindow::editDelete()
{
    OgitorsRoot::getSingletonPtr()->GetViewport()->DeleteSelectedObject();
}
//------------------------------------------------------------------------------
void MainWindow::editRename()
{
    CBaseEditor *object = OgitorsRoot::getSingletonPtr()->GetSelection()->getFirstObject();
    if(object)
    {
        QString text = QInputDialog::getText(this, tr("Enter a new name"), tr("Name : "), QLineEdit::Normal, QString(object->getName().c_str()));
        text = text.trimmed();
        if(text.toStdString() != object->getName())
        {
            OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
            undoMgr->BeginCollection(object->getName() + "'s Name change");
            object->setName(text.toStdString());
            undoMgr->EndCollection(true);
        }
    }
}
//------------------------------------------------------------------------------
void MainWindow::editCopyToTemplate()
{
    CBaseEditor *object = OgitorsRoot::getSingletonPtr()->GetSelection()->getAsSingle();
    if(!object)
        return;

    AddTemplateDialog dialog(this);

    if(dialog.exec() == QDialog::Accepted)
    {
        Ogre::String templatename = dialog.mNameEdit->text().toStdString();
        Ogre::StringUtil::trim(templatename);

        bool success = false;

        if(object->getEditorType() == ETYPE_MULTISEL)
            success = OgitorsClipboardManager::getSingletonPtr()->copyToTemplateMulti(static_cast<CMultiSelEditor*>(object), templatename, dialog.mScope->checkState() == Qt::Checked);
        else
            success = OgitorsClipboardManager::getSingletonPtr()->copyToTemplate(object, templatename, dialog.mScope->checkState() == Qt::Checked);
        
        if(success)
        {
            mTemplatesViewWidget->prepareView();
        }
        else
            OgitorsSystem::getSingletonPtr()->DisplayMessageDialog(tr("Error saving the template").toStdString(), DLGTYPE_OK);
    }
}
//------------------------------------------------------------------------------
void MainWindow::editCopyToTemplateWithChildren()
{
    CBaseEditor *object = OgitorsRoot::getSingletonPtr()->GetSelection()->getAsSingle();
    if(!object || object == OgitorsRoot::getSingletonPtr()->GetSelection())
        return;

    AddTemplateDialog dialog(this);

    if(dialog.exec() == QDialog::Accepted)
    {
        Ogre::String templatename = dialog.mNameEdit->text().toStdString();
        Ogre::StringUtil::trim(templatename);

        if(OgitorsClipboardManager::getSingletonPtr()->copyToTemplateWithChildren(object, templatename, dialog.mScope->checkState() == Qt::Checked))
        {
            mTemplatesViewWidget->prepareView();
        }
        else
            OgitorsSystem::getSingletonPtr()->DisplayMessageDialog(tr("Error saving the template").toStdString(), DLGTYPE_OK);
    }
}
//------------------------------------------------------------------------------
void MainWindow::toggleWorldSpaceGizmo()
{
    bool state = actToggleWorldSpaceGizmo->isChecked();

    OgitorsRoot::getSingletonPtr()->SetWorldSpaceGizmoOrientation(state);
    
	QSettings settings;
    QString style = settings.value("preferences/customStyleSheet").toString();
    QString orient_world_icon = "";
    QString orient_obj_icon = "";
    if(style == ":/stylesheets/dark.qss") {
        orient_obj_icon = QString::fromLatin1(":/icons/orient_obj_dark_style.svg");
        orient_world_icon = QString::fromLatin1(":/icons/orient_world_dark_style.svg");
    } else {
        orient_obj_icon = QString::fromLatin1(":/icons/orient_obj.svg");
        orient_world_icon = QString::fromLatin1(":/icons/orient_world.svg");
    }
    if(state)
        actToggleWorldSpaceGizmo->setIcon(QIcon(orient_world_icon));
    else
        actToggleWorldSpaceGizmo->setIcon(QIcon(orient_obj_icon));
}
//------------------------------------------------------------------------------
void MainWindow::toggleWalkAround()
{
    bool state = actToggleWalkAround->isChecked();

    OgitorsRoot::getSingletonPtr()->SetWalkAroundMode(state);
    
	QSettings settings;
    QString style = settings.value("preferences/customStyleSheet").toString();
    QString fly_icon = "";
    QString walk_icon = "";
    if(style == ":/stylesheets/dark.qss") {
        fly_icon = QString::fromLatin1(":/icons/mode_fly_dark_style.svg");
        walk_icon = QString::fromLatin1(":/icons/mode_walk_dark_style.svg");
    } else {
        fly_icon = QString::fromLatin1(":/icons/mode_fly.svg");
        walk_icon = QString::fromLatin1(":/icons/mode_walk.svg");
    }

	if(state)
        actToggleWalkAround->setIcon(QIcon(walk_icon));
    else
        actToggleWalkAround->setIcon(QIcon(fly_icon));
}
//------------------------------------------------------------------------------
void MainWindow::saveLayout()
{
    QString path = QFileDialog::getSaveFileName(QApplication::activeWindow(), tr("Save Layout"), "", tr("Layout Files (*.oglayout)"), 0
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    , QFileDialog::DontUseNativeDialog );
#else
    );
#endif
    if(path != "")
    {
        writeSettings(path);
    }
}
//------------------------------------------------------------------------------
void MainWindow::loadLayout()
{
    QString path = QFileDialog::getOpenFileName(QApplication::activeWindow(), tr("Load Layout"), "", tr("Layout Files (*.oglayout)"), 0
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    , QFileDialog::DontUseNativeDialog );
#else
    );
#endif
    if(path != "")
    {
        readSettings(path);
    }
}
//------------------------------------------------------------------------------
void MainWindow::selectActionTriggered(int value)
{
    if(OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
    {
        if(value > 9)
        {
            value -= 10;

            NameObjectPairList list;

            for(unsigned int i = 0;i < mSelectLists[value].size();i++)
            {
                CBaseEditor *object = OgitorsRoot::getSingletonPtr()->FindObject(mSelectLists[value][i]);
                if(object)
                    list.insert(NameObjectPairList::value_type(object->getName(), object));
            }

            OgitorsRoot::getSingletonPtr()->GetSelection()->setSelection(list);
        }
        else
        {
            NameObjectPairList list = OgitorsRoot::getSingletonPtr()->GetSelection()->getSelection();
            NameObjectPairList::iterator it = list.begin();

            mSelectLists[value].clear();

            while(it != list.end())
            {
                mSelectLists[value].push_back(it->second->getObjectID());
                it++;
            }
        }
    }
}
//------------------------------------------------------------------------------
bool findScriptFile(QString &filename)
{
    if(QFile(filename).exists())
        return true;

    Ogre::String fileN = OgitorsUtils::ExtractFileName(filename.toStdString());
    Ogre::String file = OgitorsUtils::QualifyPath("../Scripts/" + fileN);
        
    if(QFile(file.c_str()).exists())
    {
        filename = QString(file.c_str());
        return true;
    }

    if(OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
    {
        filename = QString("proj:/Scripts/") + QString(fileN.c_str());
        
        return true;
    }

    return false;
}
//------------------------------------------------------------------------------
void MainWindow::runScriptClicked()
{
    QString command = txtScriptInput->text();
    command = command.trimmed();
    std::string commandString = command.toStdString();
    
    if(command.startsWith("/run "))
    {
        command = command.right(command.length() - 5);
        
        bool canundo = false;
        if(OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
            canundo = true;

        if(canundo)
            OgitorsUndoManager::getSingletonPtr()->BeginCollection("Script " + command.toStdString());
        
        if(findScriptFile(command))
        {
            commandString = command.toStdString();
            OgitorsScriptConsole::getSingletonPtr()->runScript(commandString);
        }
        else
        {
            listScriptOutput->addItem(QString("Script %1 can not be found!").arg(command));
        }

        if(canundo)
            OgitorsUndoManager::getSingletonPtr()->EndCollection(true);
    }
    else if(command.startsWith("/test "))
    {
        command = command.right(command.length() - 6);
        commandString = command.toStdString();

        if(findScriptFile(command))
        {
            commandString = command.toStdString();
            OgitorsScriptConsole::getSingletonPtr()->testScript(commandString);
        }
        else
        {
            listScriptOutput->addItem(QString("Script %1 can not be found!").arg(command));
        }
    }
    else
    {
        OgitorsScriptConsole::getSingletonPtr()->execString(commandString);
    }
    txtScriptInput->StoreLine();
    txtScriptInput->clear();
}
//------------------------------------------------------------------------------
void MainWindow::onAddScriptAction()
{
    AddScriptActionDialog dlg(QApplication::activeWindow());

    if(dlg.exec() == QDialog::Accepted)
    {
        _addScriptAction(dlg.iconpath->text(), dlg.scriptpath->text());
        
        _saveScriptActions();
    }
}
//------------------------------------------------------------------------------
void MainWindow::onExecuteScriptAction(int index)
{
    if((unsigned int)index < mScriptActions.size())
    {
        Ogre::String scriptfile = mScriptActions[index].script.toStdString();

        OgitorsUndoManager::getSingletonPtr()->BeginCollection("Run Script Action");
        OgitorsScriptConsole::getSingletonPtr()->runScript(scriptfile);
        OgitorsUndoManager::getSingletonPtr()->EndCollection(true);
    }
}
//------------------------------------------------------------------------------
void MainWindow::_saveScriptActions()
{
        QSettings settings;

        settings.beginGroup("ScriptActions");
        int pos = 0;
        for(unsigned int i = 0;i < mScriptActions.size();i++)
        {
            if(!mScriptActions[i].active)
                continue;

            settings.setValue(QString("IconPath%1").arg(pos), mScriptActions[i].icon);
            settings.setValue(QString("ScriptPath%1").arg(pos), mScriptActions[i].script);
            ++pos;
        }

        settings.setValue("ActionCount", pos);
        settings.endGroup();
}
//------------------------------------------------------------------------------
void MainWindow::_addScriptAction(const QString& iconpath, const QString& scriptpath)
{
    ScriptActionData data;
    data.active = true;
    data.icon = iconpath;
    data.script = scriptpath;

    int index = mScriptActions.size();

    Ogre::String actionname = scriptpath.toStdString();
    actionname = OgitorsUtils::ExtractFileName(actionname);

    QString actionName = QString("Run %1").arg(actionname.c_str());

    QAction *act = new QAction(QIcon(data.icon), actionName, this);
    data.action = act;
    mScriptActions.push_back(data);

    act->setToolTip(actionName);
    act->setStatusTip(actionName);
    connect(act, SIGNAL(triggered()), mScriptActionMap, SLOT(map()));
    mScriptActionMap->setMapping(act, index );

    mScriptActionsBar->addAction(act);
}
//------------------------------------------------------------------------------
void MainWindow::_removeScriptAction(QAction *action)
{
    for(unsigned int i = 0;i < mScriptActions.size();i++)
    {
        if(mScriptActions[i].action == action)
        {
            mScriptActionsBar->removeAction(action);
            delete action;
            mScriptActions[i].action = 0;
            mScriptActions[i].active = false;

            _saveScriptActions();
            
            return;
        }
    }
}
//------------------------------------------------------------------------------
void MainWindow::_editScriptAction(QAction *action)
{
    for(unsigned int i = 0;i < mScriptActions.size();i++)
    {
        if(mScriptActions[i].action == action)
        {
            AddScriptActionDialog dlg(QApplication::activeWindow());
            dlg.iconpath->setText(mScriptActions[i].icon);
            dlg.scriptpath->setText(mScriptActions[i].script);
            QPixmap pmap(mScriptActions[i].icon);
            dlg.iconpix->setPixmap(pmap);

            if(dlg.exec() == QDialog::Accepted)
            {
                mScriptActions[i].icon = dlg.iconpath->text();
                mScriptActions[i].script = dlg.scriptpath->text();

                Ogre::String actionname = mScriptActions[i].script.toStdString();
                actionname = OgitorsUtils::ExtractFileName(actionname);

                QString actionName = QString("Run %1").arg(actionname.c_str());


                mScriptActions[i].action->setIcon(QIcon(mScriptActions[i].icon));
                mScriptActions[i].action->setToolTip(actionName);
                mScriptActions[i].action->setStatusTip(actionName);

                _saveScriptActions();
            }
            
            return;
        }
    }
}
//------------------------------------------------------------------------------
void MainWindow::onPlayerRunPause()
{
    if(OgitorsRoot::getSingletonPtr()->GetRunState() == RS_RUNNING)
        OgitorsRoot::getSingletonPtr()->SetRunState(RS_PAUSED);
    else
        OgitorsRoot::getSingletonPtr()->SetRunState(RS_RUNNING);
}
//------------------------------------------------------------------------------
void MainWindow::onPlayerStop()
{
    OgitorsRoot::getSingletonPtr()->SetRunState(RS_STOPPED);
}
//------------------------------------------------------------------------------