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

#include "filesystemview.hxx"
#include "mainwindow.hxx"

using namespace Ogitors;

//----------------------------------------------------------------------------------------
FileSystemViewWidget::FileSystemViewWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *pBoxlayout = new QVBoxLayout(this);
    pBoxlayout->setMargin(0);
    
    mModel = new QFileSystemModel();
    mTree = new QTreeView(parent);
    mTree->setModel(mModel);
   
    pBoxlayout->addWidget(mTree);

    mAllowedExtensions.append("txt");
    mAllowedExtensions.append("html");
    mAllowedExtensions.append("htm");
    mAllowedExtensions.append("scene");
    mAllowedExtensions.append("ogscene");
    mAllowedExtensions.append("ini");
    mAllowedExtensions.append("xml");
    mAllowedExtensions.append("log");
    mAllowedExtensions.append("cfg");
    mAllowedExtensions.append("ofs");

    connect(mTree, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(onDoubleClicked(const QModelIndex&)));
}
//----------------------------------------------------------------------------------------
FileSystemViewWidget::~FileSystemViewWidget()
{
}
//----------------------------------------------------------------------------------------
void FileSystemViewWidget::prepareView()
{
   mModel->setRootPath(Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ProjectDir.c_str());

   QModelIndex index = mModel->index(Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ProjectDir.c_str());
   mTree->setRootIndex(index);
}
//----------------------------------------------------------------------------------------
void FileSystemViewWidget::clearView()
{
    mModel = new QFileSystemModel();
    mTree->reset();
}
//----------------------------------------------------------------------------------------
void FileSystemViewWidget::onDoubleClicked(const QModelIndex& index)
{   
    QString filePath = mModel->filePath(index);
    int pos = filePath.lastIndexOf(".");
    QString extension = filePath.right(filePath.size() - pos - 1);

    if(mAllowedExtensions.indexOf(extension) != -1) 
    {
        mOgitorMainWindow->getGenericTextEditor()->displayTextFromFile(mModel->filePath(index));
        mOgitorMainWindow->mEditorTab->setCurrentIndex(mOgitorMainWindow->mEditorTab->indexOf(mOgitorMainWindow->getGenericTextEditor()));
    }
}
//----------------------------------------------------------------------------------------