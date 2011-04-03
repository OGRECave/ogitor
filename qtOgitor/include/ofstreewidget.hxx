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

#ifndef OFSTREEWIDGET_HXX
#define OFSTREEWIDGET_HXX

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>

#include "ofs.h"

class OfsTreeWidget : public QTreeWidget
{
    Q_OBJECT;
public:

    enum Capabilities
    {
        CAP_SHOW_DIRS = 0,
        CAP_SHOW_FILES = 1,
        CAP_ALLOW_DROPS = 2,
        CAP_FULL_FUNCTIONS = 0xFFFF
    };

    OfsTreeWidget(QWidget *parent = 0, unsigned int capabilities = CAP_SHOW_DIRS, std::string initialSelection = "/");
    virtual ~OfsTreeWidget() {};

    const std::string& getSelected() { return mSelected; }

public Q_SLOTS:
    void onSelectionChanged();
    void onItemCollapsed( QTreeWidgetItem * item );
    void onItemExpanded( QTreeWidgetItem * item );

protected:
    typedef std::map<std::string, QTreeWidgetItem*> NameTreeWidgetMap;

    OFS::OfsPtr       mFile;
    std::string       mSelected;
    NameTreeWidgetMap mItemMap;
    unsigned int      mCapabilities;
    QIcon             mUnknownFileIcon;

    void contextMenuEvent(QContextMenuEvent *evt);
    void dragEnterEvent(QDragEnterEvent *evt);
    void dragMoveEvent(QDragMoveEvent *evt);
    void dropEvent(QDropEvent *evt);
    
    void fillTree(QTreeWidgetItem *pItem, std::string path);
    void fillTreeFiles(QTreeWidgetItem *pItem, std::string path);
};

#endif // OFSTREEWIDGET_HXX
