/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2011 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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
#include "addterraindialog.hxx"
#include "OgitorsPrerequisites.h"
#include "BaseEditor.h"
#include "OgitorsRoot.h"

AddTerrainDialog::AddTerrainDialog(QWidget *parent, QStringList& items) :
    QDialog(parent, Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint)
{
    setupUi(this);
    mPositionCombo->addItems(items);

    unsigned int i;
    Ogitors::PropertyOptionsVector *mapDiffuse = Ogitors::OgitorsRoot::GetTerrainDiffuseTextureNames();
    if(mapDiffuse->size() == 0)
        mapDiffuse = Ogitors::OgitorsRoot::GetTerrainDiffuseTextureNames();
    for(i = 0;i < mapDiffuse->size();i++)
    {
        mDiffuseCombo->addItem((*mapDiffuse)[i].mKey.c_str());
    }
    
    if(mapDiffuse->size())
        mDiffuseCombo->setCurrentIndex(1);

    Ogitors::PropertyOptionsVector *mapNormal = Ogitors::OgitorsRoot::GetTerrainNormalTextureNames();
    if(mapNormal->size() == 0)
        mapNormal = Ogitors::OgitorsRoot::GetTerrainNormalTextureNames();
    for(i = 0;i < mapNormal->size();i++)
    {
        mNormalCombo->addItem((*mapNormal)[i].mKey.c_str());
    }
    if(mapNormal->size())
        mNormalCombo->setCurrentIndex(1);
}

AddTerrainDialog::~AddTerrainDialog()
{
}

