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
#include <../QtGui/qgraphicsitem.h>

#include "manageTerrainDialog.hxx"

#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "BaseEditor.h"
#include "TerrainEditor.h"
#include "TerrainPageEditor.h"
#include "uiterrainsquare.hxx"

using namespace Ogitors;

ManageTerrainDialog::ManageTerrainDialog(QWidget *parent, Ogre::NameValuePairList &params) 
    : QDialog(parent, Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint)
{
    this->setWindowFlags(Qt::Window);
    setupUi(this);

    QTimer *mTimerDrawPage = new QTimer(this);
    connect(mTimerDrawPage, SIGNAL(timeout()), this, SLOT(update()));
    mTimerDrawPage->start(300);
    mDrawRequested = false;

    mPageGraphics->setScene(&mScene);
    mPageGraphics->centerOn(0,0);
    mPageGraphics->setDragMode(QGraphicsView::ScrollHandDrag);
    mPageGraphics->setRenderHint(QPainter::Antialiasing);
    mPageGraphics->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    mPageGraphics->show();
    drawPageMap();
}

ManageTerrainDialog::~ManageTerrainDialog()
{
    // TODO: Find out if I need to clean up the UITerrainSquare objects or does QT clean those up?
    OGRE_FREE(mtx, Ogre::MEMCATEGORY_GEOMETRY);
}

bool ManageTerrainDialog::hasTerrain(int X, int Y)
{
    X = X - minX;
    Y = Y - minY;

    if (X < 0 || Y < 0 || X > width || Y > height)
        return false;

    if (!mtx[(Y * width) + X]) {
        return true;
    }

    return false;
}

void ManageTerrainDialog::drawPageMap()
{
    // Remove all items
	mScene.clear();
    mScene.setBackgroundBrush(QBrush(Qt::black));

    Ogitors::CBaseEditor* editor = Ogitors::OgitorsRoot::getSingletonPtr()->GetTerrainEditorObject();
    Ogitors::NameObjectPairList::iterator it;

    minX = -1, minY = -1;
    int maxX = 1, maxY = 1, PX, PY;
    for(it = editor->getChildren().begin(); it != editor->getChildren().end();it++)
    {
         Ogitors::CTerrainPageEditor *terrain = static_cast<Ogitors::CTerrainPageEditor*>(it->second);
         PX = terrain->getPageX();
         PY = terrain->getPageY();
         minX = std::min(minX, PX - 1);
         minY = std::min(minY, PY - 1);
         maxX = std::max(maxX, PX + 1);
         maxY = std::max(maxY, PY + 1);
    }

    width = maxX - minX + 1;
    height = maxY - minY + 1;

    mtx = OGRE_ALLOC_T(bool, width * height, Ogre::MEMCATEGORY_GEOMETRY);
    for(int i = 0; i < width * height;++i)
        mtx[i] = true;
 
    for(it = editor->getChildren().begin(); it != editor->getChildren().end();it++)
    {
         Ogitors::CTerrainPageEditor *terrain = static_cast<Ogitors::CTerrainPageEditor*>(it->second);
         PX = terrain->getPageX();
         PY = terrain->getPageY();

         mtx[((PY - minY) * width) + (PX - minX)] = false;
    }

    UITerrainSquare * rect;
    for(int Y = 0;Y < height;++Y)
    {
        for(int X = 0;X < width;++X)
        {
            // Terrain exists
            rect = new UITerrainSquare(mPageGraphics, this, X + minX, Y + minY, !mtx[(Y * width) + X]);
            mScene.addItem(rect);
        }
    }
}

void ManageTerrainDialog::requestPageDraw()
{
    mDrawRequested = true;
}

void ManageTerrainDialog::update()
{
    if (!mDrawRequested)
        return;

    mDrawRequested = false;
    OGRE_FREE(mtx, Ogre::MEMCATEGORY_GEOMETRY);
    drawPageMap();
}

