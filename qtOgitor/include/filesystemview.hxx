/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2010 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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

#ifndef FILE_SYSTEM_VIEW_HXX
#define FILE_SYSTEM_VIEW_HXX

#include <QtGui/QWidget>
#include <QtGui/QSplitter>
#include <QtGui/QFileSystemModel>
#include <QtGui/QTreeView>
#include <QtGui/QListView>
#include <QtGui/QVBoxLayout>

#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"

#include "generictexteditor.hxx"

//----------------------------------------------------------------------------------------

class FileSystemViewWidget : public QWidget
{
    Q_OBJECT;
public:
    explicit FileSystemViewWidget(QWidget *parent = 0);
    virtual ~FileSystemViewWidget();

    void        prepareView();
    void        clearView();

private Q_SLOTS:
    void onDoubleClicked(const QModelIndex& index);

protected:
    QFileSystemModel    *mModel;
    QTreeView           *mTree;
    QStringList         mAllowedExtensions;
};

//----------------------------------------------------------------------------------------

#endif // FILE_SYSTEM_VIEW_HXX

//----------------------------------------------------------------------------------------
