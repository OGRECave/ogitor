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

#include <QtCore/QtCore>
#include <QtGui/QFileDialog>
#include "importheightmapdialog.hxx"
#include "Ogitors.h"

ImportHeightMapDialog::ImportHeightMapDialog(QWidget *parent) :
    QDialog(parent, Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint)
{
    setupUi(this);
    mInputScale->setText("256");
    mInputBias->setText("0");

    QSettings settings(QSettings::UserScope, "preferences/terrainmanager");

    Ogitors::OgitorsRoot *ogitorRoot = Ogitors::OgitorsRoot::getSingletonPtr();
    Ogitors::PropertyOptionsVector* diffuseList = ogitorRoot->GetTerrainDiffuseTextureNames();
    Ogre::String lastUsedDiffuse = settings.value("lastDiffuseUsed", "").toString().toStdString();

    unsigned int i, index = 1;
    for(i = 0;i < diffuseList->size(); i++)
    {
        if (lastUsedDiffuse == (*diffuseList)[i].mKey)
            index = i;

        mDiffuseCombo->addItem((*diffuseList)[i].mKey.c_str());
    }

    if (lastUsedDiffuse.empty())
        index = 1;

    if(diffuseList->size())
        mDiffuseCombo->setCurrentIndex(index);

    index = 1;
    Ogre::String lastNormalUsed = settings.value("lastNormalUsed", "").toString().toStdString();
    Ogitors::PropertyOptionsVector* normalList = ogitorRoot->GetTerrainNormalTextureNames();
    for(i = 0;i < normalList->size(); i++)
    {
        if (lastNormalUsed == (*normalList)[i].mKey)
            index = i;

        mNormalCombo->addItem((*normalList)[i].mKey.c_str());
    }

    if (lastNormalUsed.empty())
        index = 1;

    if(normalList->size())
        mNormalCombo->setCurrentIndex(index);
}

ImportHeightMapDialog::~ImportHeightMapDialog()
{
}
