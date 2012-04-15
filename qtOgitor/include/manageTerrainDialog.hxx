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
#pragma once

#include <QtGui/QMainWindow>
#include <QtGui/QDialog>
#include <Ogre.h>

#include <QtGui/QWidget>

//#include "ui_manageTerrainDialog.h"
#include "manageTerrainGraphicsView.hxx"

class ManageTerrainDialog : public QMainWindow//, public Ui::manageTerrainDialog, 
{
    Q_OBJECT
public:
    ManageTerrainDialog(QWidget *parent);
    virtual ~ManageTerrainDialog();

    void requestPageDraw();
    bool hasTerrain(int X, int Y);  
    void drawPageMap();

    inline Ogre::String getLastUsedDiffuse(){return mLastUsedDiffuse;};
    inline Ogre::String getLastUsedNormal(){return mLastUsedNormal;};

    void setLastUsedDiffuse(Ogre::String diffuse){mLastUsedDiffuse = diffuse;};
    void setLastUsedNormal(Ogre::String normal){mLastUsedNormal = normal;};

public slots:
    void update();

private:
    ManageTerrainGraphicsView *mPageGraphics;
    QToolBar        *mToolBar;

    QTimer*         mTimerDrawPage;
    bool            mDrawRequested;
    Ogre::String    mLastUsedDiffuse;
    Ogre::String    mLastUsedNormal;
    bool            mSelectionMode;
    
    bool*           mMtx;
    int             mWidth;
    int             mHeight;
    int             mMinY;
    int             mMinX;
    QGraphicsScene  mScene;
};

