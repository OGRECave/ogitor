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

#include "projectfilesview.hxx"
#include "ofstreewidget.hxx"
#include "mainwindow.hxx"
#include "generictexteditor.hxx"
#include "entityview.hxx"
#include "templateview.hxx"
#include "genericimageeditor.hxx"

#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "OgitorsUtils.h"
#include "DefaultEvents.h"
#include "EventManager.h"

//----------------------------------------------------------------------------------------
ProjectFilesViewWidget::ProjectFilesViewWidget(QWidget *parent) :
    QWidget(parent), mOfsTreeWidget(0)
{
    mVBoxLayout = new QVBoxLayout(this);
    mVBoxLayout->setMargin(0);
    mVBoxLayout->setSpacing(0);
    mMenu = new QMenu();

    mActRefresh = new QAction(tr("Refresh"), this);
    mActRefresh->setStatusTip(tr("Refresh file contents"));
    mActRefresh->setIcon(QIcon(":/icons/refresh.svg"));
     
    mActExtract = new QAction(tr("Extract To..."), this);
    mActExtract->setStatusTip(tr("Extract file contents to.."));
    mActExtract->setIcon(QIcon(":/icons/extract.svg"));

    mActDefrag = new QAction(tr("Defrag"), this);
    mActDefrag->setStatusTip(tr("Defragment file"));
    mActDefrag->setIcon(QIcon(":/icons/defrag.svg"));

    mActDelete = new QAction(tr("Remove"), this);
    mActDelete->setStatusTip(tr("Remove a file/folder from your Ogitor project"));
    mActDelete->setIcon(QIcon(":/icons/trash.svg"));

    mActAddFolder = new QAction(tr("Add Empty Folder"), this);
    mActAddFolder->setStatusTip(tr("Add a new empty folder to your Ogitor project"));
    mActAddFolder->setIcon(QIcon(":/icons/folder.svg"));

    mActImportFolder = new QAction(tr("Import Folder"), this);
    mActImportFolder->setStatusTip(tr("Import a folder into your Ogitor project"));
    mActImportFolder->setIcon(QIcon(":/icons/import.svg"));

    mActImportFile = new QAction(tr("Import File"), this);
    mActImportFile->setStatusTip(tr("Import a single file into your Ogitor project"));
    mActImportFile->setIcon(QIcon(":/icons/additional.svg"));

    mActMakeAsset = new QAction(tr("Make scene asset"), this);
    mActMakeAsset->setStatusTip(tr("Make the resource usable as a scene asset"));
    mActMakeAsset->setIcon(QIcon(":/icons/objects.svg"));
    mActMakeAsset->setCheckable(true);
    mActMakeAsset->setChecked(false);

    mActRename = new QAction(tr("Rename"), this);
    mActRename->setStatusTip(tr("Rename Selected File/Folder"));
    mActRename->setIcon(QIcon(":/icons/editrename.svg"));

    mActReadOnly = new QAction(tr("Read Only"), this);
    mActReadOnly->setStatusTip(tr("Set/UnSet File/Folder as Read Only"));
    mActReadOnly->setIcon(QIcon(":/icons/lock.svg"));
    mActReadOnly->setCheckable(true);
    mActReadOnly->setChecked(false);

    mActHidden = new QAction(tr("Hidden"), this);
    mActHidden->setStatusTip(tr("Set/UnSet File/Folder as Hidden"));
    mActHidden->setCheckable(true);
    mActHidden->setChecked(false);

    mMenu->addAction(mActAddFolder);
    mMenu->addSeparator();
    mMenu->addAction(mActImportFile);
    mMenu->addAction(mActImportFolder);
    mMenu->addSeparator();
    mMenu->addAction(mActMakeAsset);
    mMenu->addSeparator();
    mMenu->addAction(mActRename);
    mMenu->addAction(mActReadOnly);
    mMenu->addAction(mActHidden);
    mMenu->addSeparator();
    mMenu->addAction(mActDelete);

    mActAddFolder->setEnabled(false);
    mActImportFile->setEnabled(false);
    mActImportFolder->setEnabled(false);
    mActMakeAsset->setEnabled(false);
    mActRefresh->setEnabled(false);
    mActExtract->setEnabled(false);
    mActDefrag->setEnabled(false);
    mActDelete->setEnabled(false);
    mActRename->setEnabled(false);
    mActReadOnly->setEnabled(false);
    mActHidden->setEnabled(false);

    connect(mActAddFolder,      SIGNAL(triggered()),    this,   SLOT(onAddFolder()));
    connect(mActImportFile,     SIGNAL(triggered()),    this,   SLOT(onImportFile()));
    connect(mActImportFolder,   SIGNAL(triggered()),    this,   SLOT(onImportFolder()));
    connect(mActMakeAsset,      SIGNAL(triggered()),    this,   SLOT(onMakeAsset()));
    connect(mActRefresh,        SIGNAL(triggered()),    this,   SLOT(onRefresh()));
    connect(mActExtract,        SIGNAL(triggered()),    this,   SLOT(onExtract()));
    connect(mActDefrag,         SIGNAL(triggered()),    this,   SLOT(onDefrag()));
    connect(mActDelete,         SIGNAL(triggered()),    this,   SLOT(onDelete()));
    connect(mActRename,         SIGNAL(triggered()),    this,   SLOT(onRename()));
    connect(mActReadOnly,       SIGNAL(triggered()),    this,   SLOT(onReadOnly()));
    connect(mActHidden,         SIGNAL(triggered()),    this,   SLOT(onHidden()));

    mToolBar = new QToolBar();
    mToolBar->setIconSize(QSize(16, 16));
    mToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    mToolBar->addAction(mActRefresh);
    mToolBar->addSeparator();
    mToolBar->addAction(mActAddFolder);
    mToolBar->addSeparator();
    mToolBar->addAction(mActExtract);
    mToolBar->addAction(mActDefrag);

    mAddFileFolderPath = "/";
}
//----------------------------------------------------------------------------------------
ProjectFilesViewWidget::~ProjectFilesViewWidget()
{
    delete mMenu;
    mMenu = 0;

    delete mToolBar;
    mToolBar = 0;
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::prepareView()
{
    mOfsTreeWidget = new OfsTreeWidget(this, OfsTreeWidget::CAP_FULL_FUNCTIONS);
    
    mActRefresh->setEnabled(true);
    mActAddFolder->setEnabled(true);
    mActExtract->setEnabled(true);
    mActDefrag->setEnabled(true);

    mVBoxLayout->addWidget(mToolBar, 0);
    mVBoxLayout->addWidget(mOfsTreeWidget, 1);

    connect(mOfsTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));
    connect(mOfsTreeWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onOfsWidgetCustomContextMenuRequested(const QPoint &)));
    connect(mOfsTreeWidget, SIGNAL(busyState(bool)), this, SLOT(onOfsWidgetBusyState(bool)));
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::clearView()
{
    if(mOfsTreeWidget != 0)
    {
        disconnect(mOfsTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));
        disconnect(mOfsTreeWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onOfsWidgetCustomContextMenuRequested(const QPoint &)));
        disconnect(mOfsTreeWidget, SIGNAL(busyState(bool)), this, SLOT(onOfsWidgetBusyState(bool)));
        
        delete mOfsTreeWidget;
        mOfsTreeWidget = 0;

        mVBoxLayout->removeWidget(mToolBar);
    }

    mActRefresh->setEnabled(false);
    mActAddFolder->setEnabled(false);
    mActExtract->setEnabled(false);
    mActDefrag->setEnabled(false);
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::itemDoubleClicked(QTreeWidgetItem* item, int column)
{
    if(item == 0)
        return;

    QString path = item->whatsThis(0);
    if(path.endsWith("/"))
    {
        onRename();
        return;
    }

    path = QString(Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile()->getFileSystemName().c_str()) + QString("::") + path;
    if(mOgitorMainWindow->getGenericTextEditor()->findMatchingCodecFactory(path) != 0)
    {
        mOgitorMainWindow->getGenericTextEditor()->displayTextFromFile(path);
        return;
    }
    
    if(mOgitorMainWindow->getGenericImageEditor()->findMatchingCodecFactory(path) != 0)
    {
        mOgitorMainWindow->getGenericImageEditor()->displayImageFromFile(path);
        return;
    }
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onOfsWidgetCustomContextMenuRequested(const QPoint & pos)
{
    QList<QTreeWidgetItem*> selItems = mOfsTreeWidget->selectedItems();
 
    mAddFileFolderPath = "/";

    if(selItems.size() > 0)
    {
        QString path = selItems[0]->whatsThis(0);
 
        if(path == "/")
        {
            mActMakeAsset->setEnabled(false);
            mActMakeAsset->setChecked(false);
            mActReadOnly->setChecked(false);
            mActHidden->setChecked(false);
            mActReadOnly->setEnabled(false);
            mActHidden->setEnabled(false);
            mActRename->setEnabled(false);
            mActDelete->setEnabled(false);
        }
        else
        {
            if(path.endsWith("/"))
                mAddFileFolderPath = path.toStdString();

            mActMakeAsset->setEnabled(true);
            mActMakeAsset->setChecked(false);
            mActReadOnly->setChecked(true);
            mActHidden->setChecked(true);
            mActReadOnly->setEnabled(true);
            mActHidden->setEnabled(true);
            mActRename->setEnabled(true);
            mActDelete->setEnabled(true);
        }

        OFS::OfsPtr& file = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();
        unsigned int flags = 0;

        if(path.endsWith("/"))
            file->getDirFlags(path.toStdString().c_str(), flags);
        else
            file->getFileFlags(path.toStdString().c_str(), flags);

        mActReadOnly->setChecked(flags & OFS::OFS_READONLY);
        if(flags & OFS::OFS_READONLY)
            mActReadOnly->setIcon(QIcon(":icons/unlock.svg"));
        mActHidden->setChecked(flags & OFS::OFS_HIDDEN);
       
        Ogre::StringVector dirs = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ResourceDirectories;
        Ogre::StringVector::iterator it;

        for(it = dirs.begin(); it != dirs.end(); it++)
        {
            if((*it) == path.toStdString())
                mActMakeAsset->setChecked(true);
        }
    }
    else
    {
        mActMakeAsset->setEnabled(false);
        mActMakeAsset->setChecked(false);
        mActReadOnly->setChecked(false);
        mActHidden->setChecked(false);
        mActReadOnly->setEnabled(false);
        mActHidden->setEnabled(false);
        mActRename->setEnabled(false);
        mActDelete->setEnabled(false);
    }

    mActImportFile->setEnabled(true);
    mActImportFolder->setEnabled(true);

    QPoint globalPos = mOfsTreeWidget->mapToGlobal(pos);
    mMenu->exec(globalPos);
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onOfsWidgetBusyState(bool state)
{
    mActAddFolder->setEnabled(!state);
    mActImportFile->setEnabled(!state);
    mActImportFolder->setEnabled(!state);
    mActRefresh->setEnabled(!state);
    mActExtract->setEnabled(!state);
    mActDefrag->setEnabled(!state);
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onRefresh()
{
    mOfsTreeWidget->refreshWidget();
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onExtract()
{
    mOfsTreeWidget->extractFiles();
}
//----------------------------------------------------------------------------------------
void ofsCallback(std::string msg)
{
    mOgitorMainWindow->updateLog(new QListWidgetItem(msg.c_str(), 0, 2));
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onDefrag()
{
    OFS::OfsPtr& ofsFile = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();

    if(ofsFile.valid())
    {
        QString fileName = ofsFile->getFileSystemName().c_str();
        QString tmpFile = fileName + ".tmp";
        
        // Step-1 defragment the file to temp file
        OFS::LogCallBackFunction callback = ofsCallback;
        ofsFile->defragFileSystemTo(tmpFile.toStdString().c_str(), &callback);
        // Step-2 make the temp file active file, so that we can overwrite the original file
        ofsFile->switchFileSystemTo(tmpFile.toStdString().c_str());
        // Step-3 copy temp file over original file and make it active
        ofsFile->moveFileSystemTo(fileName.toStdString().c_str());
        // Step-4 remove temp file
        QFile::remove(tmpFile);

        mOfsTreeWidget->refreshWidget();
        mOgitorMainWindow->scrollLogToBottom();
    }
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onDelete()
{
    QStringList selItems = mOfsTreeWidget->getSelectedItems();
    OFS::OfsPtr& ofsFile = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();

    if(selItems.size() > 0 && ofsFile.valid())
    {
        if(QMessageBox::information(QApplication::activeWindow(), "qtOgitor", tr("Are you sure you want to delete selected files/folders?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            for(int i = 0;i < selItems.size();i++)
            {
                QString name = selItems.at(i);

                if(name.endsWith("/"))
                    ofsFile->deleteDirectory(name.toStdString().c_str(), true);
                else
                    ofsFile->deleteFile(name.toStdString().c_str());
            }
            
            mOfsTreeWidget->refreshWidget();            
        }
    }
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onRename()
{
    QList<QTreeWidgetItem*> selItems = mOfsTreeWidget->selectedItems();
    OFS::OfsPtr& ofsFile = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();

    if(selItems.size() == 1 && ofsFile.valid())
    {
        QString name = selItems[0]->whatsThis(0);

        bool ok;
        QString text = QInputDialog::getText(this, tr("Rename File/Folder"), "Enter new name:", QLineEdit::Normal, selItems[0]->text(0), &ok);
        if ( ok && !text.isEmpty() ) 
        {
            // user entered something and pressed OK
            if(name.endsWith("/"))
                ofsFile->renameDirectory(name.toStdString().c_str(), text.toStdString().c_str());
            else
                ofsFile->renameFile(name.toStdString().c_str(), text.toStdString().c_str());
            
            mOfsTreeWidget->refreshWidget();            
        } 
    }
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onReadOnly()
{
    QList<QTreeWidgetItem*> selItems = mOfsTreeWidget->selectedItems();
    OFS::OfsPtr& ofsFile = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();

    if(selItems.size() > 0 && ofsFile.valid())
    {
        for(int i = 0;i < selItems.size();i++)
        {
            QString name = selItems[i]->whatsThis(0);

            unsigned int new_flag = 0;
            
            if(name.endsWith("/"))
            {
                ofsFile->getDirFlags(name.toStdString().c_str(), new_flag);
                
                if(mActReadOnly->isChecked())
                    new_flag |= OFS::OFS_READONLY;
                else
                    new_flag &= ~OFS::OFS_READONLY;

                ofsFile->setDirFlags(name.toStdString().c_str(), new_flag);
            }
            else
            {
                ofsFile->getFileFlags(name.toStdString().c_str(), new_flag);
                
                if(mActReadOnly->isChecked())
                    new_flag |= OFS::OFS_READONLY;
                else
                    new_flag &= ~OFS::OFS_READONLY;

                ofsFile->setFileFlags(name.toStdString().c_str(), new_flag);
            }
        }
            
        mOfsTreeWidget->refreshWidget();            
    }
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onHidden()
{
    QList<QTreeWidgetItem*> selItems = mOfsTreeWidget->selectedItems();
    OFS::OfsPtr& ofsFile = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();

    if(selItems.size() > 0 && ofsFile.valid())
    {
        for(int i = 0;i < selItems.size();i++)
        {
            QString name = selItems[i]->whatsThis(0);

            unsigned int new_flag = 0;
            
            if(name.endsWith("/"))
            {
                ofsFile->getDirFlags(name.toStdString().c_str(), new_flag);
                
                if(mActHidden->isChecked())
                    new_flag |= OFS::OFS_HIDDEN;
                else
                    new_flag &= ~OFS::OFS_HIDDEN;

                ofsFile->setDirFlags(name.toStdString().c_str(), new_flag);
            }
            else
            {
                ofsFile->getFileFlags(name.toStdString().c_str(), new_flag);
                
                if(mActHidden->isChecked())
                    new_flag |= OFS::OFS_HIDDEN;
                else
                    new_flag &= ~OFS::OFS_HIDDEN;

                ofsFile->setFileFlags(name.toStdString().c_str(), new_flag);
            }
        }
            
        mOfsTreeWidget->refreshWidget();            
    }
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onImportFolder()
{
    Ogitors::OgitorsSystem *mSystem = Ogitors::OgitorsSystem::getSingletonPtr();

    Ogre::UTFString defaultPath = mSystem->GetSetting("system", "importOpenPath", "");
    Ogre::String folderName = mSystem->DisplayDirectorySelector(OTR("Choose folder to import"), defaultPath);

    if (folderName == "")
        return;

    mSystem->SetSetting("system", "importOpenPath", Ogitors::OgitorsUtils::ExtractFilePath(folderName));

    QStringList folders(folderName.c_str());
    if(!folders.empty())
        mOfsTreeWidget->addFiles(mAddFileFolderPath.c_str(), folders);
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onImportFile()
{
    Ogitors::OgitorsSystem *mSystem = Ogitors::OgitorsSystem::getSingletonPtr();
    
    Ogre::UTFString defaultPath = mSystem->GetSetting("system", "importOpenPath", "");
    Ogre::String fileName = mSystem->DisplayOpenDialog(OTR("Choose file to import"), Ogitors::UTFStringVector(), defaultPath);

    if (fileName == "")
        return;

    mSystem->SetSetting("system", "importOpenPath", Ogitors::OgitorsUtils::ExtractFilePath(fileName));

    QStringList files(fileName.c_str());
    if(!files.empty())
        mOfsTreeWidget->addFiles(mAddFileFolderPath.c_str(), files);
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onMakeAsset()
{
    QList<QTreeWidgetItem*> selItems = mOfsTreeWidget->selectedItems();
    QString name = selItems[0]->whatsThis(0);
    bool found = false;

    Ogre::StringVector::iterator it;
    it = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ResourceDirectories.begin();

    while(it != Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ResourceDirectories.end())
    {
        if((*it) == name.toStdString())
        {
            found = true;
            Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ResourceDirectories.erase(it);
            break;
        }
        else
            it++;
    }

    if(!found)
        Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ResourceDirectories.push_back(name.toStdString());
    
    Ogitors::OgitorsRoot::getSingletonPtr()->ReloadUserResources();
    mOgitorMainWindow->getEntityViewWidget()->prepareView();
    mOgitorMainWindow->getTemplatesViewWidget()->prepareView();

    // Send event so that all listening plugins get notified as well and can update their views
    Ogitors::GlobalPrepareViewEvent evt;
    Ogitors::EventManager::getSingletonPtr()->sendEvent(this, 0, &evt);
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onAddFolder()
{
    QString selectedItemName;
    QString newFolderName;
    
    QStringList selectItemList = mOfsTreeWidget->getSelectedItems();
    if(selectItemList.size() > 1)
    {
        QMessageBox::information(QApplication::activeWindow(), "Ogitor", tr("Only select one parent folder!"));
        return;
    }
    else
        selectedItemName = selectItemList.at(0);

    // Search until a yet unused name has been found.
    bool nameFound = false;
    int i = 0;
    while(nameFound == false)
    {
        newFolderName = QString("### NewFolder ").append(QString::number(i));
        if(mOfsTreeWidget->findItems(newFolderName, Qt::MatchRecursive).size() == 0)
            nameFound = true;
        else
            i++;
    }    
    
    OFS::OfsPtr& ofsFile = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();
    // If current selected item is a folder, create a sub folder. Otherwise create
    // the new folder as a sibling of the selected item.
    if(selectedItemName.right(1) == "/")
    {
        QString sName = selectedItemName + newFolderName + "/";
        ofsFile->createDirectory(sName.toStdString().c_str());
        mOfsTreeWidget->refreshWidget();   

        // setCurrentItem() needs to be called twice, because the first call might trigger
        // the onExpand() slot of ofsTreeWidget, which will overwrite our desired selection.
        // The second call will then not need any expansion work anymore and therefore 
        //successfully set the desired selection.
        mOfsTreeWidget->setCurrentItem(mOfsTreeWidget->findItems(newFolderName, Qt::MatchRecursive).at(0));
        mOfsTreeWidget->setCurrentItem(mOfsTreeWidget->findItems(newFolderName, Qt::MatchRecursive).at(0));
    }
    else
    {
        selectedItemName = selectedItemName.right(selectedItemName.size() - (selectedItemName.lastIndexOf("/") + 1));
        QString parentName = mOfsTreeWidget->findItems(selectedItemName, Qt::MatchRecursive).at(0)->parent()->whatsThis(0);
        
        QString sName = parentName + newFolderName + "/";
        ofsFile->createDirectory(sName.toStdString().c_str());
        mOfsTreeWidget->refreshWidget();
        
        // setCurrentItem() needs to be called twice, because the first call might trigger
        // the onExpand() slot of ofsTreeWidget, which will overwrite our desired selection.
        // The second call will then not need any expansion work anymore and therefore 
        //successfully set the desired selection.
        mOfsTreeWidget->setCurrentItem(mOfsTreeWidget->findItems(newFolderName, Qt::MatchRecursive).at(0));
        mOfsTreeWidget->setCurrentItem(mOfsTreeWidget->findItems(newFolderName, Qt::MatchRecursive).at(0));
    }
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::triggerRefresh()
{
    mActRefresh->trigger();
}
//----------------------------------------------------------------------------------------
