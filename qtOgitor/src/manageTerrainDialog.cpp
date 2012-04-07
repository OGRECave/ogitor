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
#include "BaseEditor.h"
#include "TerrainEditor.h"
#include "TerrainPageEditor.h"
#include "uiterrainsquare.hxx"

ManageTerrainDialog::ManageTerrainDialog(QWidget *parent, Ogre::NameValuePairList &params) 
    : QDialog(parent, Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint)
{
    this->setWindowFlags(Qt::Window);
    setupUi(this);
    regenerateMapFlags();
    drawPageMap();
}

ManageTerrainDialog::~ManageTerrainDialog()
{
    // TODO: Do we need to clean up the UITerrainSquare objects or does QT clean those up?
}

void ManageTerrainDialog::writeTerrainFlag(const int x, const int y, Ogre::String flag)
{
    Ogre::String coords = Ogre::StringConverter::toString(x)+"x"+Ogre::StringConverter::toString(y);
    mTerrainPages[coords] = "1";
    mTerrainSize.united(QRect(x, y, 1, 1));
    mWidth = mTerrainSize.width();
    mHeight= mTerrainSize.height();
}

void ManageTerrainDialog::removeTerrainFlag(const int x, const int y)
{
    Ogre::String coords = Ogre::StringConverter::toString(x)+"x"+Ogre::StringConverter::toString(y);
    mTerrainPages.erase(coords);
    mTerrainSize.united(QRect(x, y, 1, 1));
    mWidth = mTerrainSize.width();
    mHeight= mTerrainSize.height();
}

void ManageTerrainDialog::regenerateMapFlags()
{
    mTerrainPages.clear();

    Ogitors::CBaseEditor* editor = Ogitors::OgitorsRoot::getSingletonPtr()->GetTerrainEditorObject();
    Ogitors::NameObjectPairList::iterator it;
    
    int minX = -1, minY = -1, maxX = 1, maxY = 1, PX, PY;
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

    mWidth = maxX - minX + 1;
    mHeight = maxY - minY + 1;

    mScene.setBackgroundBrush(QBrush(Qt::black));
     
    bool *mtx = OGRE_ALLOC_T(bool, mWidth * mHeight, Ogre::MEMCATEGORY_GEOMETRY);
    for(int i = 0; i < mWidth * mHeight;++i)
        mtx[i] = true;
 
    for(it = editor->getChildren().begin(); it != editor->getChildren().end();it++)
    {
         Ogitors::CTerrainPageEditor *terrain = static_cast<Ogitors::CTerrainPageEditor*>(it->second);
         PX = terrain->getPageX();
         PY = terrain->getPageY();

         mtx[((PY - minY) * mWidth) + (PX - minX)] = false;
    }

    for(int Y = 0;Y < mHeight;++Y)
    {
        for(int X = 0;X < mWidth;++X)
        {
            // Terrain exists
            if(!mtx[(Y * mWidth) + X])
            {
                Ogre::String coords = Ogre::StringConverter::toString(X + minX)+"x"+Ogre::StringConverter::toString(Y + minY);
                mTerrainPages[coords] = "1";
            }
        }
    }

    OGRE_FREE(mtx, Ogre::MEMCATEGORY_GEOMETRY);

    // Get actual size
    mTerrainSize = QRect(minX+1, minY+1, mWidth-2, mHeight-2);
}

void ManageTerrainDialog::drawPageMap()
{
    UITerrainSquare* rect;
    QColor green(71, 130, 71);
    QColor grey(52, 51, 49);

    for(int Y = mTerrainSize.y()-1;Y < mTerrainSize.height()+1;Y++)
    {
        for(int X = mTerrainSize.x()-1;X < mTerrainSize.width()+1;X++)
        {
            rect = new UITerrainSquare(mPageGraphics, this);
            rect->setRect(X*30, Y*30, 30, 30);
            Ogre::String coords = Ogre::StringConverter::toString(X)+"x"+Ogre::StringConverter::toString(Y);
            if(mTerrainPages.count(coords) > 0)
            {
                // Terrain exists
                rect->set(X, Y, QPen(Qt::black), QBrush(green), true);
            } else {
                // Terrain does not exist
                rect->set(X, Y, QPen(Qt::black), QBrush(grey), false);
            }
            mScene.addItem(rect);
        }
    }

    mPageGraphics->setScene(&mScene);
    mPageGraphics->centerOn(0,0);
    mPageGraphics->setDragMode(QGraphicsView::ScrollHandDrag);
    mPageGraphics->setRenderHint(QPainter::Antialiasing);
    mPageGraphics->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    mPageGraphics->show();
}

