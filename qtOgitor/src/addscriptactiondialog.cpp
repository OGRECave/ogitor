/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2015 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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

#include "addscriptactiondialog.hxx"

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <QtCore/QFile>

AddScriptActionDialog::AddScriptActionDialog(QWidget *parent, Qt::WindowFlags flags) :
    QDialog(parent, flags)
{
    setupUi(this);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(verifyOK()));
    connect(iconbrowse, SIGNAL(clicked()), this, SLOT(selectIcon()));
    connect(scriptbrowse, SIGNAL(clicked()), this, SLOT(selectScript()));
}

AddScriptActionDialog::~AddScriptActionDialog()
{
}

void AddScriptActionDialog::verifyOK()
{
    bool isOk = true;

    QString script = scriptpath->text();

    if(!QFile(script).exists())
        isOk = false;

    if(isOk)
        accept();
    else
    {
        QMessageBox::information(QApplication::activeWindow(),"qtOgitor", tr("The script file can not be found!!"));
    }
}

void AddScriptActionDialog::selectIcon()
{
    QString path = QFileDialog::getOpenFileName(QApplication::activeWindow(), tr("Select an Icon File"), "", tr("PNG Files") + " (*.png);;" + tr("SVG Files") + " (*.svg)", 0
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    , QFileDialog::DontUseNativeDialog );
#else
    );
#endif

    if(!path.isEmpty())
    {
        iconpath->setText(path);
        QPixmap pmap(path);
        iconpix->setPixmap(pmap);
    }
}

void AddScriptActionDialog::selectScript()
{
    QString path = QFileDialog::getOpenFileName(QApplication::activeWindow(), tr("Select a Script File"), "", tr("Script Files") + " ( *.as );;", 0
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    , QFileDialog::DontUseNativeDialog );
#else
    );
#endif

    if(!path.isEmpty())
    {
        scriptpath->setText(path);
    }
}
