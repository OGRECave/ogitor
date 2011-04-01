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

#include "mainwindow.hxx"
#include "ofstreewidget.hxx"

#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"

//----------------------------------------------------------------------------------------
OfsTreeWidget::OfsTreeWidget(QWidget *parent, unsigned int capabilities, std::string initialSelection) : QTreeWidget(parent), mCapabilities(capabilities) 
{
    mSelected = initialSelection;

    setColumnCount(1);
    setHeaderHidden(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setDragDropOverwriteMode(false);
    setDragDropMode(QAbstractItemView::DragDrop);

    mUnknownFileIcon = mOgitorMainWindow->mIconProvider.icon(QFileIconProvider::File);

    mFile = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();

    QTreeWidgetItem* item = 0;
    QTreeWidgetItem* pItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("Project")));
    pItem->setIcon(0, mOgitorMainWindow->mIconProvider.icon(QFileIconProvider::Folder));
    pItem->setTextColor(0, Qt::black);
    QFont fnt = pItem->font(0);
    fnt.setBold(true);
    pItem->setFont(0, fnt);
    pItem->setWhatsThis(0, QString("/"));
    
    addTopLevelItem(pItem);

    fillTree(pItem, "/");

    if(capabilities & CAP_SHOW_FILES)
        fillTreeFiles(pItem, "/");

    expandItem(pItem);

    if(mSelected == "/")
        setItemSelected(pItem, true);
    else
    {
        NameTreeWidgetMap::iterator it = mItemMap.find(mSelected);

        if(it != mItemMap.end())
        {
            clearSelection();
            scrollToItem(it->second);
            setItemSelected(it->second, true);
        }
    }

    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));

    if(capabilities & CAP_SHOW_FILES)
    {
        connect(this, SIGNAL(itemCollapsed( QTreeWidgetItem * )), this, SLOT(onItemCollapsed( QTreeWidgetItem * )));
        connect(this, SIGNAL(itemExpanded( QTreeWidgetItem * )), this, SLOT(onItemExpanded( QTreeWidgetItem * )));
    }
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::fillTree(QTreeWidgetItem *pItem, std::string path)
{
    OFS::FileList list = mFile->listFiles(path.c_str(), OFS::OFS_DIR);

    std::sort(list.begin(), list.end(), OFS::FileEntry::Compare);

    for(unsigned int i = 0;i < list.size();i++)
    {
        Ogre::String name = list[i].name;

        QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(name.c_str())));
        item->setIcon(0, mOgitorMainWindow->mIconProvider.icon(QFileIconProvider::Folder));
        item->setTextColor(0, Qt::black);

        std::string fullpath = path + name + "/";
        item->setWhatsThis(0, QString(fullpath.c_str()));

        pItem->addChild(item);

        mItemMap.insert(NameTreeWidgetMap::value_type(fullpath, item));


        fillTree(item, fullpath);
    }

    if(path != "/" && list.size() == 0)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(".")));
        item->setTextColor(0, Qt::black);
        item->setWhatsThis(0, QString(path.c_str()));
        pItem->addChild(item);
    }
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::fillTreeFiles(QTreeWidgetItem *pItem, std::string path)
{
    OFS::FileList list = mFile->listFiles(path.c_str(), OFS::OFS_FILE);

    std::sort(list.begin(), list.end(), OFS::FileEntry::Compare);

    for(unsigned int i = 0;i < list.size();i++)
    {
        Ogre::String name = list[i].name;
        Ogre::String ext_name = name;

        QIcon icon = mUnknownFileIcon;
        int ext_pos = ext_name.find_last_of(".");

        if(ext_pos > 0)
        {
            ext_name.erase(0, ext_pos);
            
            FileIconMap::iterator it = mOgitorMainWindow->mFileIconMap.find(ext_name);
            if(it == mOgitorMainWindow->mFileIconMap.end())
            {
                std::string filename = "./qtOgitor_icontest";
                filename += ext_name;

                std::fstream stream;
                stream.open(filename.c_str(), std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
                stream.close();

                QFileInfo info(QString(filename.c_str()));
                icon = mOgitorMainWindow->mIconProvider.icon(info);
                if(icon.isNull())
                    icon = mUnknownFileIcon;
                    
                mOgitorMainWindow->mFileIconMap.insert(FileIconMap::value_type(ext_name, icon));

                QFile::remove(QString(filename.c_str()));
            }
            else
                icon = it->second;
        }

        QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(name.c_str())));
        item->setIcon(0, icon);
        item->setTextColor(0, Qt::black);

        std::string fullpath = path + name;
        item->setWhatsThis(0, QString(fullpath.c_str()));

        pItem->addChild(item);

        mItemMap.insert(NameTreeWidgetMap::value_type(fullpath, item));
    }
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::onSelectionChanged()
{
    mSelected = "/";

    QList<QTreeWidgetItem*> selectionList = selectedItems();

    if(selectionList.size() > 0)
    {
        mSelected = selectionList[0]->whatsThis(0).toStdString();
    }
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::onItemCollapsed( QTreeWidgetItem * item )
{
    std::vector<QTreeWidgetItem*> deleteList;

    if(item != NULL && item != topLevelItem(0))
    {
        int total = item->childCount();

        for(int i = 0;i < total;i++)
        {
            if(item->child(i)->whatsThis(0).endsWith("/"))
                deleteList.push_back(item);
        }

        for(unsigned int i = 0;i < deleteList.size();i++)
        {
            QTreeWidgetItem *chItem = deleteList[i];

            mItemMap.erase(mItemMap.find(chItem->whatsThis(0).toStdString()));

            item->removeChild(chItem);
        }
    }

    clearSelection();
    setItemSelected(item, true);
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::onItemExpanded( QTreeWidgetItem * item )
{
    if(item != NULL && item != topLevelItem(0))
    {
        int total = item->childCount();

        for(int i = 0;i < total;i++)
        {
            if(item->child(i)->whatsThis(0) == item->whatsThis(0))
            {
                item->removeChild(item->child(i));
                break;
            }
        }

        fillTreeFiles(item, item->whatsThis(0).toStdString());
    }

    clearSelection();
    setItemSelected(item, true);
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::dragEnterEvent(QDragEnterEvent *evt)
{
    evt->ignore();
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::dragMoveEvent(QDragMoveEvent *evt)
{
    evt->ignore();
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::dropEvent(QDropEvent *evt)
{
    evt->ignore();
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::contextMenuEvent(QContextMenuEvent *evt)
{
    evt->ignore();
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::mouseDoubleClickEvent(QMouseEvent *evt)
{
    evt->ignore();
}
//----------------------------------------------------------------------------------------
