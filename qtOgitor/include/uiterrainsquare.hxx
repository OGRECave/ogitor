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

#pragma once

#include <QtGui/QtGui>
#include <QtGui/QGraphicsItem>
#include <QtCore/QtCore>
#include <Ogre.h>
#include "manageTerrainDialog.hxx"

class UITerrainSquare : public QObject, public QGraphicsRectItem 
{
    Q_OBJECT

public:
    UITerrainSquare(QGraphicsView* view, ManageTerrainDialog *parent, const int x, const int y, const bool hasTerrain);
    /** Returns true if the terrain page has no neighbour terrain pages next to it **/
    bool hasFreeNeighbour();
    /** If the terrain page was click selected by the user */
    inline bool isSelected() { return mSelected; };
    /** Sets the terrain page to selected to perform actions against it */
    void setSelected(const bool& selected);
    /** returns if the terrain page has terrain already **/
    inline bool hasTerrain() { return mHasTerrain; };
    /** returns the X postion of where the terrain page is located **/
    inline int getPosX() { return mPosX; };
    /** returns the Y postion of where the terrain page is located **/
    inline int getPosY() { return mPosY; };
private:
    QGraphicsView* mView;
    int mPosX;
    int mPosY;
    bool mHasTerrain;
    bool mSelected;
    ManageTerrainDialog* mParent;
};

