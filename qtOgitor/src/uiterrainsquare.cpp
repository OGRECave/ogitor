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

UITerrainSquare::UITerrainSquare(QGraphicsView* view, ManageTerrainDialog *parent)
{
    this->view = view;
    this->parent = parent;

    setAcceptedMouseButtons(Qt::RightButton);

    actAddPage = new QAction(tr("Add Terrain Page"), (QObject*) this);
    actAddPage->setStatusTip(tr("Adds a new page to the terrain group"));
    connect(actAddPage, SIGNAL(triggered()), (QObject*) this, SLOT(addPage()));

    actAddNeighbourPage = new QAction(tr("Add Neighbour Pages"), (QObject*) this);
    actAddNeighbourPage->setStatusTip(tr("Creates terrain pages around this page"));
    connect(actAddNeighbourPage, SIGNAL(triggered()), (QObject*) this, SLOT(addNeighbourPage()));

    actRemovePage = new QAction(tr("Remove Terrain Page"), (QObject*) this);
    actRemovePage->setStatusTip(tr("Removes a new page to the terrain group"));
    connect(actRemovePage, SIGNAL(triggered()), (QObject*) this, SLOT(removePage()));


    contextMenu.addAction(actAddPage);
    contextMenu.addAction(actAddNeighbourPage);
//TODO: Implement remove page
//    contextMenu.addAction(actRemovePage);
}

void UITerrainSquare::set(const signed int x, const signed int y, const QPen pen, const QBrush brush, const bool hasTerrain)
{
    this->x = x;
    this->y = y;
    this->setPen(pen);
    this->setBrush(brush);
    mHasTerrain = hasTerrain;
}

bool UITerrainSquare::hasFreeNeighbour()
{
    int X,Y;
    for(int Y = this->y-1;Y < this->y+2;Y++)
    {
        for(int X = this->x-1;X < this->x+2;X++)
        {
            if (Y == this->y && X == this->x)
                continue;

            Ogre::String coords = Ogre::StringConverter::toString(X)+"x"+Ogre::StringConverter::toString(Y);
            if (parent->getTerrainPageFlags()->count(coords) == 0)
                return true;
        }
    }
    return false;
}

void UITerrainSquare::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    QColor green(71, 130, 71);
    QColor grey(52, 51, 49);
    QColor orange(173, 81, 44);

    actAddPage->setEnabled(!mHasTerrain);
    actRemovePage->setEnabled(mHasTerrain);
    actAddNeighbourPage->setEnabled(hasFreeNeighbour() && mHasTerrain);

    this->setBrush(QBrush(orange));
    QAction* actionSelected = contextMenu.exec(QCursor::pos());
    this->setBrush(QBrush(grey));

    update();
    QGraphicsItem::mousePressEvent(event);

    if (!mHasTerrain)
        return;

    this->setBrush(QBrush(green));
}

void UITerrainSquare::addPage()
{
    CreateTerrainDialog dlg(QApplication::activeWindow());
    if(dlg.exec() == QDialog::Accepted)
    {
        Ogre::String diffuse = dlg.mDiffuseCombo->itemText(dlg.mDiffuseCombo->currentIndex()).toStdString();
        Ogre::String normal = dlg.mNormalCombo->itemText(dlg.mNormalCombo->currentIndex()).toStdString();

        // FIXME: Uncomment when displaying a progress dialog becomes threaded
        //OgitorsSystem::getSingletonPtr()->DisplayProgressDialog("Creating terrain page", 0,1,0);

        CTerrainGroupEditor *terGroup = static_cast<CTerrainGroupEditor *>(OgitorsRoot::getSingletonPtr()->FindObject("Terrain Group"));
        terGroup->addPage(this->x, this->y, diffuse, normal);

        //OgitorsSystem::getSingletonPtr()->HideProgressDialog();
        Ogre::String coords = Ogre::StringConverter::toString(this->x)+"x"+Ogre::StringConverter::toString(this->y);
        parent->writeTerrainFlag(this->x, this->y, "1");
        parent->drawPageMap();
    }
}

void UITerrainSquare::addNeighbourPage()
{
    CreateTerrainDialog dlg(QApplication::activeWindow());

    if(dlg.exec() != QDialog::Accepted)
        return;

    Ogre::String diffuse = dlg.mDiffuseCombo->itemText(dlg.mDiffuseCombo->currentIndex()).toStdString();
    Ogre::String normal = dlg.mNormalCombo->itemText(dlg.mNormalCombo->currentIndex()).toStdString();

    CTerrainGroupEditor *terGroup = static_cast<CTerrainGroupEditor *>(OgitorsRoot::getSingletonPtr()->FindObject("Terrain Group"));
    
    int X,Y;
    for(int Y = this->y-1;Y < this->y+2;Y++)
    {
        for(int X = this->x-1;X < this->x+2;X++)
        {
            if (Y == this->y && X == this->x)
                continue;

            Ogre::String coords = Ogre::StringConverter::toString(X)+"x"+Ogre::StringConverter::toString(Y);
            if (parent->getTerrainPageFlags()->count(coords) != 0)
                continue;

            //terGroup->addPage(X, Y, diffuse, normal);
            parent->writeTerrainFlag(X, Y, "1");
        }
    }

    parent->drawPageMap();
}

void UITerrainSquare::removePage()
{

}
