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
    void drawPageMap();
    
    /** Runs through all the terrain pages and creates a new list **/
    void regenerateMapFlags();
    /** Flags terrain as being added for the map renderer. If flag already exists it is overwritten. **/ 
    void writeTerrainFlag(const int x, const int y, Ogre::String flag);
    /** Flags terrain as not existing for the map renderer **/ 
    void removeTerrainFlag(const int x, const int y);
    /** Retrives a list of terrain page flags. Anything in the list is terrain. **/
    const Ogre::NameValuePairList* getTerrainPageFlags() { return &mTerrainPages; }
    /** returns how wide the terrain pages extend to **/ 
    const int getWidth() { return mWidth; }
    /** returns how high the terrain pages extend to **/
    const int getHeight() { return mHeight; }
private:
    QGraphicsScene mScene;
    int mWidth;
    int mHeight;
    QRect mTerrainSize;
    Ogre::NameValuePairList mTerrainPages;
};

#endif // ADDTERRAINDIALOG_HXX
