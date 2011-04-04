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

#include "projectfilesview.hxx"
#include "ofstreewidget.hxx"
#include "mainwindow.hxx"
#include "generictexteditor.hxx"

#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"

//----------------------------------------------------------------------------------------
ProjectFilesViewWidget::ProjectFilesViewWidget(QWidget *parent) :
    QWidget(parent), ofsWidget(0)
{
    vboxLayout = new QVBoxLayout(this);
    vboxLayout->setMargin(0);
    menuCommands = new QMenu();

    // Generic text editor extensions
    mAllowedGenericExtensions.append("txt");
    mAllowedGenericExtensions.append("html");
    mAllowedGenericExtensions.append("htm");
    mAllowedGenericExtensions.append("scene");
    mAllowedGenericExtensions.append("ogscene");
    mAllowedGenericExtensions.append("ini");
    mAllowedGenericExtensions.append("xml");
    mAllowedGenericExtensions.append("log");
    mAllowedGenericExtensions.append("cfg");

    // Material editor extensions
    mAllowedMaterialExtensions.append("cg");
    mAllowedMaterialExtensions.append("hlsl");
    mAllowedMaterialExtensions.append("glsl");
    mAllowedMaterialExtensions.append("frag");
    mAllowedMaterialExtensions.append("vert");
    mAllowedMaterialExtensions.append("material");
    mAllowedMaterialExtensions.append("program");
    mAllowedMaterialExtensions.append("compositor");

    // Script editor extensions
    mAllowedScriptExtensions.append("as");

    // Combine into one joined list
    mAllowedCombinedExtensions.append(mAllowedGenericExtensions);
    mAllowedCombinedExtensions.append(mAllowedMaterialExtensions);
    mAllowedCombinedExtensions.append(mAllowedScriptExtensions);
}
//----------------------------------------------------------------------------------------
ProjectFilesViewWidget::~ProjectFilesViewWidget()
{
    delete menuCommands;
    menuCommands = 0;
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::prepareView()
{
    ofsWidget = new OfsTreeWidget(this, OfsTreeWidget::CAP_FULL_FUNCTIONS);
    
    vboxLayout->addWidget(ofsWidget);

    connect(ofsWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));
    connect(ofsWidget, SIGNAL(customContextMenuRequested ( const QPoint &)), this, SLOT(ofsWidgetCustomContextMenuRequested ( const QPoint &)));
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::clearView()
{
    if(ofsWidget != 0)
    {
        disconnect(ofsWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));
        delete ofsWidget;
        ofsWidget = 0;
    }
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::itemDoubleClicked(QTreeWidgetItem * item, int column)
{
    if(item != NULL)
    {
        QString path = item->whatsThis(0);

        if(!path.endsWith("/"))
        {
            int pos = path.lastIndexOf(".");
            QString extension = path.right(path.size() - pos - 1);

            if(mAllowedCombinedExtensions.indexOf(extension) != -1) 
            {
                path = QString(Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile()->getFileSystemName().c_str()) + QString("::") + path;
                
                if(mAllowedGenericExtensions.indexOf(extension) != -1) 
                {
                    mOgitorMainWindow->getGenericTextEditor()->displayTextFromFile(path);
                    mOgitorMainWindow->mEditorTab->setCurrentIndex(mOgitorMainWindow->mEditorTab->indexOf(mOgitorMainWindow->getGenericTextEditor()));
                }
                else if(mAllowedMaterialExtensions.indexOf(extension) != -1)
                {
                    // ToDo: Call the material editor plugin if available
                }
                else if(mAllowedScriptExtensions.indexOf(extension) != -1)
                {
                    // ToDo: Call the script editor plugin if available
                }
            }
        }
    }
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::ofsWidgetCustomContextMenuRequested( const QPoint & pos )
{
    QPoint globalPos = ofsWidget->mapToGlobal(pos);
    menuCommands->exec( globalPos );
}
//----------------------------------------------------------------------------------------
