/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2012 Andrew Fenn <andrewfenn@gmail.com> and the Ogitor Team
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

#include <QtGui/QDialog>
#include <QtGui/QToolBar>
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsItem>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QToolBar>
#include <QtGui/QAction>
#include <Ogre.h>

class UITerrainSquare;
class QMenu;

class ManageTerrainGraphicsView: public QGraphicsView
{
    Q_OBJECT
public:
    ManageTerrainGraphicsView(QWidget *parent, QToolBar *toolbar);
    virtual ~ManageTerrainGraphicsView();

    void updateActions();

public slots:
    void sltSetToolSelect(bool checked);
    void sltSetToolMove(bool checked);

    void sltAddPage();
    void sltRemovePage();
    void sltSelectAll();

protected:
    void mouseReleaseEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void keyReleaseEvent(QKeyEvent * event);
    void keyPressEvent(QKeyEvent * event);

private:

    void selectTerrainPage(UITerrainSquare *terrainSquare);
    void clearSelection();
    /** Added a terrain page to the scene **/
    void addTerrainPage(const int& x, const int& y, const Ogre::String& diffuse, const Ogre::String& normal);

    QToolBar   *mToolBar;
    QAction    *mActSelectAll;
    QAction    *mActSelect;
    QAction    *mActMove;
    QAction    *mActEditCut;
    QAction    *mActEditCopy;
    QAction    *mActEditPaste;
    QAction    *mActAddPage;
    QAction    *mActRemovePage;

    bool mSelIncTerrain;
    bool mSelIncEmpty;
    bool mSelectionMode;
    QList<UITerrainSquare*> mSelectedTerrain;
    unsigned int mSelectedTool;
    QMenu mContextMenu;
};

