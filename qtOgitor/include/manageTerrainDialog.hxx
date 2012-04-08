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
#ifndef ADDTERRAINDIALOG_HXX
#define ADDTERRAINDIALOG_HXX

#include <QtGui/QDialog>
#include <Ogre.h>

#include "ui_manageTerrainDialog.h"

class ManageTerrainDialog : public QDialog, public Ui::manageTerrainDialog {
    Q_OBJECT
public:
    ManageTerrainDialog(QWidget *parent, Ogre::NameValuePairList &params);
    virtual ~ManageTerrainDialog();
    void requestPageDraw();
    bool hasTerrain(int X, int Y);

public Q_SLOTS:
    void update();
private:
    QTimer* mTimerDrawPage;
    bool mDrawRequested;
    void drawPageMap();
    bool *mtx;
    int width;
    int height;
    int minY;
    int minX;
    QGraphicsScene mScene;
    Ogre::NameValuePairList mTerrainPages;
};

#endif // ADDTERRAINDIALOG_HXX
