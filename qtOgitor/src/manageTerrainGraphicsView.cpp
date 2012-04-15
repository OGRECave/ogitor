/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2012 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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

#include "manageTerrainGraphicsView.hxx"
#include "uiterrainsquare.hxx"

#include "Ogitors.h"
#include "OgitorsDefinitions.h"
#include <QtGui/QtGui>
#include <QtGui/QGraphicsItem>
#include <QtCore/QtCore>

#include "createterraindialog.hxx"
#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "BaseEditor.h"
#include "TerrainEditor.h"
#include "TerrainPageEditor.h"
#include "TerrainGroupEditor.h"


using namespace Ogitors;

ManageTerrainGraphicsView::ManageTerrainGraphicsView(QWidget *parent, QToolBar *toolbar) 
    : QGraphicsView(parent), mToolBar(toolbar), mSelectionMode(false), mSelectedTool(TOOL_SELECT), mContextMenu(this)
{

    setDragMode(QGraphicsView::NoDrag);
    centerOn(0, 0);
    //setDragMode(QGraphicsView::ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    mActSelect = new QAction(tr("Select"), this);
    mActSelect->setStatusTip(tr("Select"));
    mActSelect->setIcon(QIcon(":/icons/select.svg"));
    mActSelect->setEnabled(true);
    mActSelect->setCheckable(true);
    mActSelect->setChecked(true);

    mActMove = new QAction(tr("Move"), this);
    mActMove->setStatusTip(tr("Move"));
    mActMove->setIcon(QIcon(":/icons/translate.svg"));
    mActMove->setEnabled(true);
    mActMove->setCheckable(true);

    mActEditCopy = new QAction(tr("Copy"), this);
    mActEditCopy->setStatusTip(tr("Copy Selected Terrain"));
    mActEditCopy->setIcon(QIcon(":/icons/editcopy.svg"));
    mActEditCopy->setEnabled(false);

    mActEditCut = new QAction(tr("Cut"), this);
    mActEditCut->setStatusTip(tr("Cut Selected Terrain"));
    mActEditCut->setIcon(QIcon(":/icons/editcut.svg"));
    mActEditCut->setEnabled(false);

    mActEditPaste = new QAction(tr("Paste"), this);
    mActEditPaste->setStatusTip(tr("Paste Terrain"));
    mActEditPaste->setIcon(QIcon(":/icons/editpaste.svg"));
    mActEditPaste->setEnabled(false);

    mToolBar->addAction(mActSelect);
    mToolBar->addAction(mActMove);
    mToolBar->addSeparator();
    mToolBar->addAction(mActEditCut);
    mToolBar->addAction(mActEditCopy);
    mToolBar->addAction(mActEditPaste);

    connect(mActSelect, SIGNAL(triggered(bool)), this, SLOT(sltSetToolSelect(bool)));
    connect(mActMove, SIGNAL(triggered(bool)), this, SLOT(sltSetToolMove(bool)));
    
    // right click context menu
    actAddPage = new QAction(tr("Add Terrain Page"), (QObject*) this);
    actAddPage->setStatusTip(tr("Adds a new page to the terrain group"));
    connect(actAddPage, SIGNAL(triggered()), (QObject*) this, SLOT(sltAddPage()));

    actRemovePage = new QAction(tr("Remove Terrain Page"), (QObject*) this);
    actRemovePage->setStatusTip(tr("Removes a new page to the terrain group"));
    connect(actRemovePage, SIGNAL(triggered()), (QObject*) this, SLOT(sltRemovePage()));

    mContextMenu.addAction(actAddPage);
    mContextMenu.addAction(actRemovePage);

}

ManageTerrainGraphicsView::~ManageTerrainGraphicsView()
{
    
}

void ManageTerrainGraphicsView::sltSetToolSelect(bool checked)
{
    if (!checked)
        mActSelect->setChecked(true);
        
    mSelectedTool = TOOL_SELECT;
    updateActions();
}

void ManageTerrainGraphicsView::sltSetToolMove(bool checked)
{
    if (!mActMove->isChecked()) {
        sltSetToolSelect(false);
        return;
    }
    mSelectedTool = TOOL_MOVE;
    updateActions();
}

void ManageTerrainGraphicsView::updateActions()
{
    setDragMode(QGraphicsView::NoDrag);
    if (mSelectedTool == TOOL_MOVE)
        setDragMode(QGraphicsView::ScrollHandDrag);

    mActSelect->setChecked(mSelectedTool == TOOL_SELECT);
    mActMove->setChecked(mSelectedTool == TOOL_MOVE);
/*    actRotate->setChecked(mTool == TOOL_ROTATE);
    actScale->setChecked(mTool == TOOL_SCALE);
    mActSelect->setChecked()*/
}

void ManageTerrainGraphicsView::keyPressEvent(QKeyEvent * event)
{
    if (event->modifiers() & Qt::SHIFT)
        mSelectionMode = true;
}

void ManageTerrainGraphicsView::keyReleaseEvent(QKeyEvent * event)
{
    mSelectionMode = false;
}

void ManageTerrainGraphicsView::mouseReleaseEvent(QMouseEvent * event)
{
    if (mSelectedTool == TOOL_SELECT)
    {
        if (QGraphicsItem *item = itemAt(event->pos()))
            selectTerrainPage((UITerrainSquare*) item);
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void ManageTerrainGraphicsView::mousePressEvent(QMouseEvent *event)
{   
    if(event->button() != Qt::RightButton)
        return;

    QColor green(71, 130, 71);
    QColor grey(52, 51, 49);
    QColor orange(173, 81, 44);

    actAddPage->setEnabled(true);
    actRemovePage->setEnabled(true);

    QAction* actionSelected = mContextMenu.exec(QCursor::pos());
    mContextMenu.update();
}

void ManageTerrainGraphicsView::selectTerrainPage(UITerrainSquare *terrainSquare)
{
    if (!mSelectionMode)
        clearSelection();

    terrainSquare->setSelected(true);
    mSelectedTerrain.push_back(terrainSquare);
}

void ManageTerrainGraphicsView::clearSelection()
{
    foreach(UITerrainSquare *page, mSelectedTerrain) 
    {
        page->setSelected(false);
    }
    mSelectedTerrain.clear();
}

void ManageTerrainGraphicsView::sltAddPage()
{
    QSettings settings(QSettings::UserScope, "preferences/terrainmanager");

    QString lastDiffuseUsed = settings.value("lastDiffuseUsed", "").toString();
    QString lastNormalUsed = settings.value("lastNormalUsed", "").toString();

    CreateTerrainDialog dlg(QApplication::activeWindow(), lastDiffuseUsed.toStdString(), lastNormalUsed.toStdString());
    if(dlg.exec() == QDialog::Accepted)
    {
        lastDiffuseUsed = dlg.mDiffuseCombo->itemText(dlg.mDiffuseCombo->currentIndex());
        lastNormalUsed = dlg.mNormalCombo->itemText(dlg.mNormalCombo->currentIndex());
        settings.setValue("lastDiffuseUsed", lastDiffuseUsed);
        settings.setValue("lastNormalUsed", lastNormalUsed);


        foreach(UITerrainSquare *page, mSelectedTerrain) 
        {
            if (page->hasTerrain())
                continue;

            addTerrainPage(page->getPosX(), page->getPosY(), lastDiffuseUsed.toStdString(), lastNormalUsed.toStdString());
        }
        
        clearSelection();
        ((ManageTerrainDialog*)parentWidget())->drawPageMap();
    }
}

void ManageTerrainGraphicsView::addTerrainPage(const int& x, const int& y, const Ogre::String& diffuse, const Ogre::String& normal)
{
    CTerrainGroupEditor *terGroup = static_cast<CTerrainGroupEditor*>(OgitorsRoot::getSingletonPtr()->FindObject("Terrain Group"));
    terGroup->addPage(x, y, diffuse, normal);
}

void ManageTerrainGraphicsView::sltRemovePage()
{
    CTerrainGroupEditor *terGroup = static_cast<CTerrainGroupEditor*>(OgitorsRoot::getSingletonPtr()->FindObject("Terrain Group"));

    foreach(UITerrainSquare *page, mSelectedTerrain) 
    {
        if (!page->hasTerrain())
            continue;

        CTerrainPageEditor* terPage = terGroup->getPage(page->getPosX(), page->getPosY());
        terGroup->removePage(page->getPosX(), page->getPosY());
        terPage->destroy(true);
    }

    clearSelection();
    ((ManageTerrainDialog*)parentWidget())->drawPageMap();
}

