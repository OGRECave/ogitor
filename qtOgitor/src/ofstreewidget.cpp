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

#include "mainwindow.hxx"
#include "ofstreewidget.hxx"
#include "projectfilesview.hxx"

#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"

#define MAX_BUFFER_SIZE 0xFFFFFF

//----------------------------------------------------------------------------------------
OfsTreeWidget::OfsTreeWidget(QWidget *parent, unsigned int capabilities, QStringList initialSelection) : QTreeWidget(parent), mCapabilities(capabilities) 
{
    mSelectedItems = initialSelection;

    setColumnCount(1);
    setHeaderHidden(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setDragDropOverwriteMode(false);
    
    if(capabilities & CAP_ALLOW_DROPS)
        setDragDropMode(QAbstractItemView::DragDrop);

    mUnknownFileIcon = mOgitorMainWindow->mIconProvider.icon(QFileIconProvider::File);

    mFile = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();
    mFile->addTrigger(this, OFS::_Ofs::CLBK_CREATE, &triggerCallback);
    mFile->addTrigger(this, OFS::_Ofs::CLBK_DELETE, &triggerCallback);

    refreshWidget();

    mAddFilesThread = new AddFilesThread();
    mExtractorThread = new ExtractorThread();
    connect(mAddFilesThread, SIGNAL(finished()), this, SLOT(threadFinished()));
    connect(mExtractorThread, SIGNAL(finished()), this, SLOT(threadFinished()));
}
//----------------------------------------------------------------------------------------
OfsTreeWidget::~OfsTreeWidget()
{
    delete mAddFilesThread;
    mAddFilesThread = 0;

    delete mExtractorThread;
    mExtractorThread = 0;
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

        if(mCapabilities & CAP_SHOW_COLORS)
        {
            bool isReadOnly = (list[i].flags & OFS::OFS_READONLY) > 0;
            bool isHidden = (list[i].flags & OFS::OFS_HIDDEN) > 0;

            QColor textColor = Qt::black;

            if(isReadOnly && isHidden)
                textColor = QColor(255, 210, 210);
            else if(isReadOnly)
                textColor = QColor(255, 0, 0);
            else if(isHidden)
                textColor = QColor(210, 210, 210);

            item->setTextColor(0, textColor);
        }

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

        if(mCapabilities & CAP_SHOW_COLORS)
        {
            bool isReadOnly = (list[i].flags & OFS::OFS_READONLY) > 0;
            bool isHidden = (list[i].flags & OFS::OFS_HIDDEN) > 0;

            QColor textColor = Qt::black;

            if(isReadOnly && isHidden)
                textColor = QColor(255, 210, 210);
            else if(isReadOnly)
                textColor = QColor(255, 0, 0);
            else if(isHidden)
                textColor = QColor(210, 210, 210);

            item->setTextColor(0, textColor);
        }

        pItem->addChild(item);

        mItemMap.insert(NameTreeWidgetMap::value_type(fullpath, item));
    }
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::refreshWidget()
{
    disconnect(this, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));

    if(mCapabilities & CAP_SHOW_FILES)
    {
        disconnect(this, SIGNAL(itemCollapsed( QTreeWidgetItem * )), this, SLOT(onItemCollapsed( QTreeWidgetItem * )));
        disconnect(this, SIGNAL(itemExpanded( QTreeWidgetItem * )), this, SLOT(onItemExpanded( QTreeWidgetItem * )));
    }

    // Save current expansion state
    QStringList list;
    NameTreeWidgetMap::iterator it = mItemMap.begin();
    
    while(it != mItemMap.end())
    {
        if(it->second->isExpanded())
            list << it->second->whatsThis(0);

        it++;
    }

    clear();

    mItemMap.clear();
    
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

    if(mCapabilities & CAP_SHOW_FILES)
        fillTreeFiles(pItem, "/");

    expandItem(pItem);

    for(int i = 0; i < mSelectedItems.size(); i++)
    {
        if(mSelectedItems.at(i) == "/")
            setItemSelected(pItem, true);
        else
        {
            NameTreeWidgetMap::iterator it = mItemMap.find(mSelectedItems.at(i).toStdString());

            if(it != mItemMap.end())
            {
                clearSelection();
                scrollToItem(it->second);
                setItemSelected(it->second, true);
            }
        }  
    }      

    if(mCapabilities & CAP_SHOW_FILES)
    {
        connect(this, SIGNAL(itemCollapsed( QTreeWidgetItem * )), this, SLOT(onItemCollapsed( QTreeWidgetItem * )));
        connect(this, SIGNAL(itemExpanded( QTreeWidgetItem * )), this, SLOT(onItemExpanded( QTreeWidgetItem * )));
    }

    // Restore expansion state
    foreach(QString string, list)
    {
        it = mItemMap.find(string.toStdString());

        if(it != mItemMap.end())
        {
            it->second->setExpanded(true);
            onItemExpanded(it->second);
        }
    }

    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::onSelectionChanged()
{
    mSelectedItems.clear();

    QList<QTreeWidgetItem*> selectionList = selectedItems();

    for(int i = 0; i < selectionList.size(); i++)
    {
        mSelectedItems.push_back(selectionList[i]->whatsThis(0));
    }
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::onItemCollapsed(QTreeWidgetItem* item)
{
    std::vector<QTreeWidgetItem*> deleteList;

    if(item != NULL && item != topLevelItem(0))
    {
        int total = item->childCount();

        for(int i = 0;i < total;i++)
        {
            if(!item->child(i)->whatsThis(0).endsWith("/"))
                deleteList.push_back(item->child(i));
        }

        NameTreeWidgetMap::iterator it;

        for(unsigned int i = 0;i < deleteList.size();i++)
        {
            QTreeWidgetItem *chItem = deleteList[i];

            QString value = chItem->whatsThis(0);

            it = mItemMap.find(value.toStdString());

            mItemMap.erase(it);

            item->removeChild(chItem);
        }

        if(total > 0 && item->childCount() == 0)
        {
            QTreeWidgetItem* chItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(".")));
            chItem->setTextColor(0, Qt::black);
            chItem->setWhatsThis(0, item->whatsThis(0));
            item->addChild(chItem);
        }
    }

    clearSelection();
    setItemSelected(item, true);
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::onItemExpanded(QTreeWidgetItem* item)
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
QStringList OfsTreeWidget::getFilenames(const QMimeData* data)
{
   QStringList result;

   result = data->formats();

   QList<QUrl> urls = data->urls();
   for(int i = 0; i < urls.size(); ++i)
      result.push_back(urls.at(i).toLocalFile());

   return result;
}
//----------------------------------------------------------------------------------
void OfsTreeWidget::dragEnterEvent(QDragEnterEvent *evt)
{
    if(mCapabilities & CAP_ALLOW_DROPS)
    {
        // Get the filenames
        QStringList filenames = getFilenames(evt->mimeData());

        // Don't accept empty drags
        if(!filenames.empty())
        {
            evt->accept();
            return;
        }
    }

    evt->ignore();
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::dragMoveEvent(QDragMoveEvent *evt)
{
    if(mCapabilities & CAP_ALLOW_DROPS)
    {
        QTreeWidgetItem *item = itemAt(evt->pos());

        if(item != NULL && item->whatsThis(0).endsWith("/"))
        {
            evt->accept();
            return;
        }
    }

    evt->ignore();
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::dropEvent(QDropEvent *evt)
{
    if(mCapabilities & CAP_ALLOW_DROPS)
    {
        QTreeWidgetItem *item = itemAt(evt->pos());

        if(item != NULL && item->whatsThis(0).endsWith("/"))
        {
            QByteArray itemData = evt->mimeData()->data("application/x-qabstractitemmodeldatalist");
            QDataStream stream(&itemData, QIODevice::ReadOnly);

            int r, c;
            QMap<int, QVariant> variant;
            stream >> r >> c >> variant;

            QString source = variant[5].toString();
            QString target = item->whatsThis(0).append(variant[0].toString());
            if(source != target)
            {
                if(!source.endsWith(".OGSCENE", Qt::CaseInsensitive))
                {
                    mFile->moveFile(source.toAscii(), target.toAscii());
                    refreshWidget();
                    evt->accept();
                    return;
                }
                else
                {
                    QMessageBox::information(QApplication::activeWindow(), "Ogitor", tr("The Ogitor scene file cannot be moved!"));
                    evt->ignore();
                    return;
                }
            }
        }
    }

    evt->ignore();
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::addFiles(QString rootDir, QStringList list)
{
    if(!mAddFilesThread->isRunning())
    {
        emit busyState(true);
        mAddFilesThread->addFiles(mFile, rootDir.toStdString(), list);
    }
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::extractFiles()
{
    if(!mFile.valid())
        return;

    QString path = QFileDialog::getExistingDirectory(QApplication::activeWindow(), "", QApplication::applicationDirPath()
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    );
#else
    , QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly);
#endif

    if(!path.isEmpty())
    {
        OFS::FileList theList;
       
        QList<QTreeWidgetItem*> selItems = selectedItems();

        std::string baseDir = "";

        if(selItems.size() == 0)
        {
            theList = mFile->listFiles("/");
            baseDir = "/";
        }
        else
        {
            for(int i = 0;i < selItems.size();i++)
            {
                OFS::FileEntry entry;

                QString name = selItems[i]->whatsThis(0);

                if(name.endsWith("/"))
                    mFile->getDirEntry(name.toStdString().c_str(), entry);
                else
                    mFile->getFileEntry(name.toStdString().c_str(), entry);

                entry.name = name.toStdString();
                theList.push_back(entry);
            }
        }

        if(!mExtractorThread->isRunning())
        {
            emit busyState(true);
            mExtractorThread->extract(mFile, baseDir, path, theList);
        }
    }
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::threadFinished()
{
    refreshWidget();
    emit busyState(false);
}
//----------------------------------------------------------------------------------------
void OfsTreeWidget::triggerCallback(void* userData, OFS::_Ofs::OfsEntryDesc* arg1, const char* arg2)
{
    emit mOgitorMainWindow->getProjectFilesViewWidget()->triggerRefresh();
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
void ExtractorThread::extract(const OFS::OfsPtr& _ofsFile, const std::string& _currentDir, const QString& _path, const OFS::FileList& _list)
{
    path = _path;
    mlist = _list;
    currentDir = _currentDir;
    ofsFileName = _ofsFile->getFileSystemName();

    start();
}
//------------------------------------------------------------------------------------
void ExtractorThread::extractFiles(QString path, const OFS::FileList& list)
{
    std::ofstream out_handle;
    OFS::OFSHANDLE in_handle;

    unsigned int output_amount = 0;

    mutex.lock();
    currentPos = 0.0f; 
    msgProgress = "";
    mutex.unlock();

    for(unsigned int i = 0;i < list.size();i++)
    {
        if(list[i].flags & OFS::OFS_DIR)
        {
            QString dir_path = path + QString("/") + QString(list[i].name.c_str());
            QDir directory(dir_path);
            directory.mkpath(dir_path);
        }
        else
        {
            std::string file_path = path.toStdString() + std::string("/") + list[i].name;
            std::string file_ofs_path = list[i].name;

            QFileInfo info(QString(file_path.c_str()));
            QDir directory(info.absolutePath());
            directory.mkpath(info.absolutePath());

            mutex.lock();
            msgProgress = file_ofs_path.c_str();
            mutex.unlock();

            out_handle.open(file_path.c_str(), std::ofstream::out | std::ofstream::binary);

            if(out_handle.is_open())
            {
                try
                {
                    OFS::OfsResult ret = ofsFile->openFile(in_handle, file_ofs_path.c_str());
                    if(ret != OFS::OFS_OK)
                    {
                        out_handle.close();
                        continue;
                    }

                    unsigned int total = list[i].file_size;

                    while(total > 0)
                    {
                        if(total < MAX_BUFFER_SIZE)
                        {
                            ofsFile->read(in_handle, tmp_buffer, total);
                            out_handle.write(tmp_buffer, total);
                            output_amount += total;
                            total = 0;
                        }
                        else
                        {
                            ofsFile->read(in_handle, tmp_buffer, MAX_BUFFER_SIZE);
                            out_handle.write(tmp_buffer, MAX_BUFFER_SIZE);
                            total -= MAX_BUFFER_SIZE;
                            output_amount += MAX_BUFFER_SIZE;
                        }

                        if(mTotalFileSize > 0)
                        {
                            mutex.lock();
                            currentPos = (float)output_amount / (float)mTotalFileSize; 
                            mutex.unlock();
                        }
                    }
                }
                catch(OFS::Exception& e)
                {
                    QMessageBox::information(QApplication::activeWindow(),"Ofs Exception:", tr("Error Extracting File : ") + QString(file_ofs_path.c_str()) + QString("\n") + QString(e.getDescription().c_str()), QMessageBox::Ok);
                }

                out_handle.close();
                ofsFile->closeFile(in_handle);
            }
        }
    }

    mutex.lock();
    currentPos = 1.0f;
    msgProgress = tr("Finished Extracting");
    mutex.unlock();
}
//------------------------------------------------------------------------------------
unsigned int ExtractorThread::generateList(OFS::FileList& list)
{
    unsigned int list_max = list.size();
    unsigned int file_size = 0;
    
    std::string tmpSaveCurrentDir;
    OFS::FileList subList;

    for(unsigned int i = 0;i < list_max;i++)
    {
        list[i].name = currentDir + list[i].name;
        file_size += list[i].file_size;

        if(list[i].flags & OFS::OFS_DIR)
        {
            tmpSaveCurrentDir = currentDir;
            currentDir = list[i].name + "/";

            subList = ofsFile->listFiles(currentDir.c_str());

            file_size += generateList(subList);

            for(unsigned int z = 0;z < subList.size();z++)
                list.push_back(subList[z]);

            currentDir = tmpSaveCurrentDir;
        }
    }

    return file_size;
}
//------------------------------------------------------------------------------------
void ExtractorThread::run()
{
    try
    {
        ofsFile.mount(ofsFileName.c_str());
    }
    catch(...)
    {
        return;
    }

    mTotalFileSize = generateList(mlist);

    std::sort(mlist.begin(), mlist.end(), OFS::FileEntry::Compare);
    
    tmp_buffer = new char[MAX_BUFFER_SIZE];
    
    extractFiles(path, mlist);

    delete [] tmp_buffer;

    ofsFile.unmount();
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
bool AddFilesListCompare ( AddFilesData elem1, AddFilesData elem2 )
{
    return (strcmp(elem1.ofsName.toStdString().c_str(), elem2.ofsName.toStdString().c_str()) < 0);
}
//------------------------------------------------------------------------------------
void AddFilesThread::addFiles(const OFS::OfsPtr& _ofsFile, const std::string& _currentDir, const QStringList& _list)
{
    mlist.clear();
    AddFilesData data;
    for(int i = 0;i < _list.size();i++)
    {
        data.fileName = _list.at(i);
        data.onFS = true;
        data.isDir = false;
        mlist.push_back(data);
    }

    currentDir = _currentDir;
    ofsFileName = _ofsFile->getFileSystemName();

    start();
}
//------------------------------------------------------------------------------------
void AddFilesThread::addFiles(const AddFilesList& list)
{
    OFS::OFSHANDLE fhandle;

    unsigned int output_amount = 0;

    mutex.lock();
    currentPos = 0.0f; 
    msgProgress = "";
    mutex.unlock();

    // Handle the items
    for(unsigned int i = 0; i < list.size(); ++i)
    {
        if(!list[i].isDir)
        {
            QString path = list[i].fileName;
            
            std::string file_ofs_path = list[i].ofsName.toStdString();
            
            mutex.lock();
            msgProgress = file_ofs_path.c_str();
            mutex.unlock();

            std::ifstream stream;
            stream.open(path.toStdString().c_str(), std::fstream::in | std::fstream::binary);
            
            if(stream.is_open())
            {
                stream.seekg(0, std::fstream::end);
                unsigned int stream_size = stream.tellg();

                stream.seekg(0, std::fstream::beg);
 
                if(stream_size >= MAX_BUFFER_SIZE)
                {
                    //createFile accepts initial data to be written during allocation
                    //its an optimization, thats why we don't have to call Ofs::write after createFile
                    stream.read(tmp_buffer, MAX_BUFFER_SIZE);
                    try
                    {
                        if(ofsFile->createFile(fhandle, file_ofs_path.c_str(), MAX_BUFFER_SIZE, MAX_BUFFER_SIZE, tmp_buffer) != OFS::OFS_OK)
                        {
                            stream.close();
                            continue;
                        }

                        stream_size -= MAX_BUFFER_SIZE;
                        output_amount += MAX_BUFFER_SIZE;
                    }
                    catch(OFS::Exception& e)
                    {
                        QMessageBox::information(QApplication::activeWindow(),"Ofs Exception:", QString(e.getDescription().c_str()), QMessageBox::Ok);
                        stream.close();
                        continue;
                    }

                    if(mTotalFileSize > 0)
                    {
                        mutex.lock();
                        currentPos = (float)output_amount / (float)mTotalFileSize; 
                        mutex.unlock();
                    }

                    try
                    {
                        while(stream_size > 0)
                        {
                            if(stream_size >= MAX_BUFFER_SIZE)
                            {
                                stream.read(tmp_buffer, MAX_BUFFER_SIZE);
                                ofsFile->write(fhandle, tmp_buffer, MAX_BUFFER_SIZE);
                                stream_size -= MAX_BUFFER_SIZE;
                                output_amount += MAX_BUFFER_SIZE;
                            }
                            else
                            {
                                stream.read(tmp_buffer, stream_size);
                                ofsFile->write(fhandle, tmp_buffer, stream_size);
                                output_amount += stream_size;
                                stream_size = 0;
                            }

                            if(mTotalFileSize > 0)
                            {
                                mutex.lock();
                                currentPos = (float)output_amount / (float)mTotalFileSize; 
                                mutex.unlock();
                            }
                        }
                    }
                    catch(OFS::Exception& e)
                    {
                        QMessageBox::information(QApplication::activeWindow(), "Ofs Exception:", QString(e.getDescription().c_str()), QMessageBox::Ok);
                    }

                    stream.close();
                    ofsFile->closeFile(fhandle);
                }
                else
                {
                    stream.read(tmp_buffer, stream_size);

                    try
                    {
                        output_amount += stream_size;
                        if(mTotalFileSize > 0)
                        {
                            mutex.lock();
                            currentPos = (float)output_amount / (float)mTotalFileSize; 
                            mutex.unlock();
                        }
                        // createFile() accepts initial data to be written during allocation.
                        // It's an optimization, thats why we don't have to call Ofs::write after createFile()
                        if(ofsFile->createFile(fhandle, file_ofs_path.c_str(), stream_size, stream_size, tmp_buffer) != OFS::OFS_OK)
                        {
                            QMessageBox::information(QApplication::activeWindow(),"File Copy Error", tr("File copy failed for : ")+ QString(file_ofs_path.c_str()), QMessageBox::Ok);
                            stream.close();
                            continue;
                        }
                    }
                    catch(OFS::Exception& e)
                    {
                        QMessageBox::information(QApplication::activeWindow(),"Ofs Exception:", QString(e.getDescription().c_str()), QMessageBox::Ok);
                    }

                    stream.close();
                    ofsFile->closeFile(fhandle);
                }
            }
        }
        else
        {
            std::string dir_ofs_path = list[i].ofsName.toStdString();

            try
            {
                OFS::OfsResult ret = ofsFile->createDirectory(dir_ofs_path.c_str());
                if(ret != OFS::OFS_OK)
                {
                    QMessageBox::information(QApplication::activeWindow(), "Ofs Exception:", tr("Cannot create directory : ") + QString(dir_ofs_path.c_str()), QMessageBox::Ok);
                    continue;
                }
            }
            catch(OFS::Exception& e)
            {
                QMessageBox::information(QApplication::activeWindow(), "Ofs Exception:", tr("Cannot create directory : ") + QString(dir_ofs_path.c_str()) + QString("\n") + QString(e.getDescription().c_str()), QMessageBox::Ok);
                continue;
            }
        }
    }

    mutex.lock();
    currentPos = 1.0f; 
    mutex.unlock();
}
//------------------------------------------------------------------------------------
unsigned int AddFilesThread::generateList(AddFilesList& list)
{
    unsigned int list_max = list.size();
    unsigned int file_size = 0;
    
    AddFilesData data;
    AddFilesList subList;
    std::string tmpSaveCurrentDir;

    for(unsigned int i = 0;i < list_max;i++)
    {
        QFileInfo file(list[i].fileName);

        if(!file.isDir() && (list[i].onFS == true))
        {
            list[i].isDir = false;
            list[i].ofsName = QString(currentDir.c_str()) + file.fileName();
            file_size += file.size();
        }
        else if(file.isDir() || (list[i].onFS == false && list[i].isDir == true))
        {
            QString path = file.baseName();

            list[i].isDir = true;
            list[i].ofsName = QString(currentDir.c_str()) + path;

            subList.clear();
            QDirIterator it(file.absoluteFilePath());
            while(it.hasNext())
            {
                QFileInfo file2(it.next());

                if(file2.isDir())
                {
                    QString path2 = file2.baseName();

                    if(path2.isEmpty() || path2 == QString("..") || path2 == QString("."))
                        continue;
                }

                data.fileName = file2.absoluteFilePath();
                data.isDir = file2.isDir();
                data.ofsName = list[i].ofsName + "/" + file2.fileName();
                subList.push_back(data);
            }

            if(subList.size() > 0)
            {
                tmpSaveCurrentDir = currentDir;
                currentDir = currentDir + path.toStdString() + "/";
    
                file_size += generateList(subList);
    
                for(unsigned int z = 0;z < subList.size();z++)
                    list.push_back(subList[z]);

                currentDir = tmpSaveCurrentDir;
            }
        }
    }

    return file_size;
}
//------------------------------------------------------------------------------------
void AddFilesThread::run()
{
    try
    {
        ofsFile.mount(ofsFileName.c_str());
    }
    catch(...)
    {
        return;
    }

    mTotalFileSize = generateList(mlist);

    std::sort(mlist.begin(), mlist.end(), AddFilesListCompare);

    tmp_buffer = new char[MAX_BUFFER_SIZE];
    
    addFiles(mlist);

    delete [] tmp_buffer;

    tmp_buffer = 0;

    ofsFile.unmount();
}
//------------------------------------------------------------------------------------

