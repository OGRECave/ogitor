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

#include <QtGui/QtGui>
#include "uiterrainsquare.hxx"

UITerrainSquare::UITerrainSquare(QGraphicsView* view, ManageTerrainDialog *parent, const int x, const int y, const bool hasTerrain):
    QObject(view),
    QGraphicsRectItem(),
    mSelected(false)
{
    // Set location specific details
    setRect(x*30, y*-30, 30, 30);
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
}

void UITerrainSquare::setSelected(const bool& selected)
{
    QColor green(71, 130, 71);
    QColor grey(52, 51, 49);
    QColor orange(173, 81, 44);

    mSelected = selected;

    if (mSelected)
    {
       setBrush(QBrush(orange));
       return;
    }

    if (!mHasTerrain) {
        setBrush(QBrush(grey));     
    } else {
        setBrush(QBrush(green));
    }
}

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

