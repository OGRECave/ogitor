/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2011 Andrew Fenn <andrewfenn@gmail.com> and the Ogitor Team
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

#include <QtGui/QtGui>

#include "uiterrainsquare.hxx"

#include "createterraindialog.hxx"

#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "BaseEditor.h"
#include "TerrainEditor.h"
#include "TerrainPageEditor.h"
#include "TerrainGroupEditor.h"

using namespace Ogitors;

UITerrainSquare::UITerrainSquare(QGraphicsView* view, ManageTerrainDialog *parent, const int x, const int y, const bool hasTerrain):
    QObject(view),
    QGraphicsRectItem()
{
    // Set location specific details
    setRect(x*30, y*30, 30, 30);
    mPosX = x;
    mPosY = y;
    mHasTerrain = hasTerrain;

    // Set square colour
    QColor green(71, 130, 71);
    QColor gray(52, 51, 49);
    QPen pen(Qt::black);
    QBrush brush(gray);

    if (hasTerrain)
        brush = QBrush(green);

    setPen(pen);
    setBrush(brush);

    mView = view;
    mParent = parent;

//    setFlag(QGraphicsItem::ItemIsPanel);
//    setPanelModality(QGraphicsItem::SceneModal);

    actAddPage = new QAction(tr("Add Terrain Page"), (QObject*) this);
    actAddPage->setStatusTip(tr("Adds a new page to the terrain group"));
    connect(actAddPage, SIGNAL(triggered()), (QObject*) this, SLOT(addPage()));

    actAddNeighbourPage = new QAction(tr("Add Neighbour Pages"), (QObject*) this);
    actAddNeighbourPage->setStatusTip(tr("Creates terrain pages around this page"));
    connect(actAddNeighbourPage, SIGNAL(triggered()), (QObject*) this, SLOT(addNeighbourPage()));

    actRemovePage = new QAction(tr("Remove Terrain Page"), (QObject*) this);
    actRemovePage->setStatusTip(tr("Removes a new page to the terrain group"));
    connect(actRemovePage, SIGNAL(triggered()), (QObject*) this, SLOT(removePage()));

    mContextMenu.addAction(actAddPage);
    mContextMenu.addAction(actAddNeighbourPage);
    mContextMenu.addAction(actRemovePage);
}
//-------------------------------------------------------------------------------
bool UITerrainSquare::hasFreeNeighbour()
{
    for(int y = mPosY-1; y < mPosY+2; y++)
    {
        for(int x = mPosX-1; x < mPosX+2; x++)
        {
            if (y == mPosY && x == mPosX)
                continue;

            if (!mParent->hasTerrain(x, y))
                return true;
        }
    }
    return false;
}
//-------------------------------------------------------------------------------
void UITerrainSquare::mousePressEvent(QGraphicsSceneMouseEvent *event)
{   
    QColor green(71, 130, 71);
    QColor grey(52, 51, 49);
    QColor orange(173, 81, 44);

    if(event->button() != Qt::RightButton)
        return;

    actAddPage->setEnabled(!mHasTerrain);
    actAddNeighbourPage->setEnabled(hasFreeNeighbour() && mHasTerrain);
    actRemovePage->setEnabled(mHasTerrain);

 //   setBrush(QBrush(orange));
    QAction* actionSelected = mContextMenu.exec(QCursor::pos());
    mContextMenu.update();
return;
    if (!mHasTerrain)
        setBrush(QBrush(grey));     
    else
        setBrush(QBrush(green));
}
//-------------------------------------------------------------------------------
void UITerrainSquare::addPage()
{
    CreateTerrainDialog dlg(QApplication::activeWindow(), mParent->getLastUsedDiffuse(), mParent->getLastUsedNormal());
    if(dlg.exec() == QDialog::Accepted)
    {
        mParent->setLastUsedDiffuse(dlg.mDiffuseCombo->itemText(dlg.mDiffuseCombo->currentIndex()).toStdString());
        mParent->setLastUsedNormal(dlg.mNormalCombo->itemText(dlg.mNormalCombo->currentIndex()).toStdString());

        CTerrainGroupEditor *terGroup = static_cast<CTerrainGroupEditor*>(OgitorsRoot::getSingletonPtr()->FindObject("Terrain Group"));
        terGroup->addPage(mPosX, mPosY, mParent->getLastUsedDiffuse(), mParent->getLastUsedNormal());

        mParent->requestPageDraw();
    }
}
//-------------------------------------------------------------------------------
void UITerrainSquare::addNeighbourPage()
{
    CreateTerrainDialog dlg(QApplication::activeWindow(), mParent->getLastUsedDiffuse(), mParent->getLastUsedNormal());

    if(dlg.exec() != QDialog::Accepted)
        return;

    mParent->setLastUsedDiffuse(dlg.mDiffuseCombo->itemText(dlg.mDiffuseCombo->currentIndex()).toStdString());
    mParent->setLastUsedNormal(dlg.mNormalCombo->itemText(dlg.mNormalCombo->currentIndex()).toStdString());

    CTerrainGroupEditor *terGroup = static_cast<CTerrainGroupEditor*>(OgitorsRoot::getSingletonPtr()->FindObject("Terrain Group"));

    for(int y = mPosY-1; y < mPosY+2; y++)
    {
        for(int x = mPosX-1; x < mPosX+2; x++)
        {
            if (y == mPosY && x == mPosX)
                continue;

            if (mParent->hasTerrain(x, y))
                continue;

            terGroup->addPage(x, y, mParent->getLastUsedDiffuse(), mParent->getLastUsedNormal());
        }
    }

    mParent->requestPageDraw();
}
//-------------------------------------------------------------------------------
void UITerrainSquare::removePage()
{
    CTerrainGroupEditor *terGroup = static_cast<CTerrainGroupEditor*>(OgitorsRoot::getSingletonPtr()->FindObject("Terrain Group"));
    CTerrainPageEditor* terPage = terGroup->getPage(mPosX, mPosY);
    terGroup->removePage(mPosX, mPosY);
    terPage->destroy(true);

    mParent->requestPageDraw();
}
//-------------------------------------------------------------------------------
