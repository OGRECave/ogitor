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
#include "Ogitors.h"
#include "OgitorsDefinitions.h"

using namespace Ogitors;

ManageTerrainGraphicsView::ManageTerrainGraphicsView(QWidget *parent, QToolBar *toolbar) 
    : QGraphicsView(parent), mToolBar(toolbar), mSelectionMode(false)
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

    connect(mActSelect, SIGNAL(triggered(bool)), this, SLOT(setToolSelect(bool)));
    connect(mActMove, SIGNAL(triggered(bool)), this, SLOT(setToolMove(bool)));

}

ManageTerrainGraphicsView::~ManageTerrainGraphicsView()
{
    
}

void ManageTerrainGraphicsView::setToolSelect(bool checked)
{
    if (!checked)
        mActSelect->setChecked(true);
        
    mSelectedTool = TOOL_SELECT;
    updateActions();
}

void ManageTerrainGraphicsView::setToolMove(bool checked)
{
    if (!mActMove->isChecked()) {
        setToolSelect(false);
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
        itemAt(event.pos());
    }

    QGraphicsView::mouseReleaseEvent(event);
}

