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
#include "addtemplatedialog.hxx"
#include "OgitorsPrerequisites.h"
#include "OgitorsSystem.h"
#include "OgitorsClipboardManager.h"

AddTemplateDialog::AddTemplateDialog(QWidget *parent) :
    QDialog(parent, Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint)
{
    setupUi(this);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(verifyOK()));
}

AddTemplateDialog::~AddTemplateDialog()
{
}

void AddTemplateDialog::verifyOK()
{
    Ogre::String name = mNameEdit->text().toStdString();
    const Ogitors::ObjectTemplateMap& map = Ogitors::OgitorsClipboardManager::getSingletonPtr()->getGeneralTemplates();
    bool isOk = true;
    if(map.find(name) != map.end())
        isOk = false;
    const Ogitors::ObjectTemplateMap& map2 = Ogitors::OgitorsClipboardManager::getSingletonPtr()->getProjectTemplates();
    if(map2.find(name) != map2.end())
        isOk = false;

    if(isOk)
        accept();
    else
    {
        Ogitors::OgitorsSystem::getSingletonPtr()->DisplayMessageDialog(tr("The template name is not unique!!").toStdString(), Ogitors::DLGTYPE_OK);
    }
}

