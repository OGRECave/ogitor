/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2013 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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
#include <../QtGui/qgraphicsitem.h>

#include "createterraindialog.hxx"
#include "OgitorsPrerequisites.h"
#include "OgitorsSystem.h"
#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "CameraEditor.h"
#include "ViewportEditor.h"
#include "TerrainEditor.h"
#include "TerrainPageEditor.h"
#include "addtemplatedialog.hxx"

CreateTerrainDialog::CreateTerrainDialog(QWidget *parent, Ogre::String lastUsedDiffuse, Ogre::String lastUsedNormal) :
    QDialog(parent, Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint)
{
    setupUi(this);

    unsigned int i, index = 1;
    Ogitors::PropertyOptionsVector *mapDiffuse = Ogitors::OgitorsRoot::GetTerrainDiffuseTextureNames();
    for(i = 0;i < mapDiffuse->size();i++)
    {
        if (lastUsedDiffuse == (*mapDiffuse)[i].mKey)
            index = i;
        mDiffuseCombo->addItem((*mapDiffuse)[i].mKey.c_str());
    }

    if (lastUsedDiffuse.empty())
        index = 1;

    if(mapDiffuse->size())
        mDiffuseCombo->setCurrentIndex(index);

    index = 1;
    Ogitors::PropertyOptionsVector *mapNormal = Ogitors::OgitorsRoot::GetTerrainNormalTextureNames();
    for(i = 0;i < mapNormal->size();i++)
    {
        if (lastUsedNormal == (*mapNormal)[i].mKey)
            index = i;
        mNormalCombo->addItem((*mapNormal)[i].mKey.c_str());
    }

    if (lastUsedNormal.empty())
        index = 1;

    if(mapNormal->size())
        mNormalCombo->setCurrentIndex(index);
}

CreateTerrainDialog::~CreateTerrainDialog()
{
}
