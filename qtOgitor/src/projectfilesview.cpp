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

    mActEmptyRecycleBin = new QAction(tr("Empty Recycle Bin"), this);
    mActEmptyRecycleBin->setStatusTip(tr("Delete contents of Recycle Bin"));
    mActEmptyRecycleBin->setIcon(QIcon(":/icons/recyclebin_empty.svg"));

    mActRestoreFromRecycleBin = new QAction(tr("Restore from Recycle Bin"), this);
    mActRestoreFromRecycleBin->setStatusTip(tr("Restore file/folder to old location"));
    mActRestoreFromRecycleBin->setIcon(QIcon(":/icons/refresh.svg"));

    mActMakeAsset = new QAction(tr("Make scene asset"), this);
    mActMakeAsset->setStatusTip(tr("Make the resource usable as a scene asset"));
    mActMakeAsset->setIcon(QIcon(":/icons/objects.svg"));
    mActMakeAsset->setCheckable(true);
    mActMakeAsset->setChecked(false);

    mActReadOnly = new QAction(tr("Read Only"), this);
    mActReadOnly->setStatusTip(tr("Set/UnSet File/Folder as Read Only"));
    mActReadOnly->setCheckable(true);
    mActReadOnly->setChecked(false);

    mActHidden = new QAction(tr("Hidden"), this);
    mActHidden->setStatusTip(tr("Set/UnSet File/Folder as Hidden"));
    mActHidden->setCheckable(true);
    mActHidden->setChecked(false);

    mActLinkFileSystem = new QAction(tr("Link File System"), this);
    mActLinkFileSystem->setStatusTip(tr("Link a File System to current directory"));

    mUnlinkFileSystem = new QMenu(tr("Un-Link File System"));
    mUnlinkFileSystem->setStatusTip(tr("Remove Link to the selected File System"));

    mMenu->addAction(mActEmptyRecycleBin);
    mMenu->addAction(mActRestoreFromRecycleBin);
    mMenu->addAction(mActAddFolder);
    mMenu->addAction(mActLinkFileSystem);
    mMenu->addMenu(mUnlinkFileSystem);
    mMenu->addSeparator();
    mMenu->addAction(mActImportFile);
    mMenu->addAction(mActImportFolder);
    mMenu->addSeparator();
    mMenu->addAction(mActMakeAsset);
    mMenu->addSeparator();
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
    mActReadOnly->setEnabled(false);
    mActHidden->setEnabled(false);
    mActLinkFileSystem->setEnabled(false);
    mUnlinkFileSystem->setEnabled(false);


    connect(mActAddFolder,      SIGNAL(triggered()),    this,   SLOT(onAddFolder()));
    connect(mActImportFile,     SIGNAL(triggered()),    this,   SLOT(onImportFile()));
    connect(mActImportFolder,   SIGNAL(triggered()),    this,   SLOT(onImportFolder()));
    connect(mActMakeAsset,      SIGNAL(triggered()),    this,   SLOT(onMakeAsset()));
    connect(mActRefresh,        SIGNAL(triggered()),    this,   SLOT(onRefresh()));
    connect(mActExtract,        SIGNAL(triggered()),    this,   SLOT(onExtract()));
    connect(mActDefrag,         SIGNAL(triggered()),    this,   SLOT(onDefrag()));
    connect(mActDelete,         SIGNAL(triggered()),    this,   SLOT(onDelete()));
    connect(mActReadOnly,       SIGNAL(triggered()),    this,   SLOT(onReadOnly()));
    connect(mActHidden,         SIGNAL(triggered()),    this,   SLOT(onHidden()));
    connect(mActLinkFileSystem, SIGNAL(triggered()),    this,   SLOT(onLinkFileSystem()));
    connect(mActEmptyRecycleBin, SIGNAL(triggered()),    this,   SLOT(onEmptyRecycleBin()));
    connect(mActRestoreFromRecycleBin, SIGNAL(triggered()),    this,   SLOT(onRestoreFromRecycleBin()));

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
bool ProjectFilesViewWidget::isFocusTarget()
{
	if( mOfsTreeWidget != NULL )
		return mOfsTreeWidget->hasFocus();

	return false;
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
	connect(mOfsTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::clearView()
{
    if(mOfsTreeWidget != 0)
    {
        disconnect(mOfsTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));
        disconnect(mOfsTreeWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onOfsWidgetCustomContextMenuRequested(const QPoint &)));
        disconnect(mOfsTreeWidget, SIGNAL(busyState(bool)), this, SLOT(onOfsWidgetBusyState(bool)));
	    disconnect(mOfsTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));

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
void ProjectFilesViewWidget::onSelectionChanged()
{
	if( mOfsTreeWidget == NULL )
		return;

	bool rename = false;

	QList<QTreeWidgetItem*> selItems = mOfsTreeWidget->selectedItems();

    if(selItems.size() == 1 && selItems[0]->parent() != mOfsTreeWidget->topLevelItem(1))
    {
        QString path = selItems[0]->whatsThis(0);
        OFS::OfsPtr& file = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();
        unsigned int flags = 0;

        if(path.endsWith("/"))
            file->getDirFlags(path.toStdString().c_str(), flags);
        else
            file->getFileFlags(path.toStdString().c_str(), flags);

        if(path != "/")
            rename = !(flags & OFS::OFS_LINK);
    }

	mOgitorMainWindow->actEditRename->setEnabled( rename );
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
void ProjectFilesViewWidget::modifyStats( selectStats& stats, QTreeWidgetItem* item)
{
    if( item == mOfsTreeWidget->topLevelItem(1) )
    {
        memset( &stats, 0, sizeof(selectStats) );

        stats.mActEmptyRecycleBinEnabled = true;

        return;
    }
    else if( item->parent() == mOfsTreeWidget->topLevelItem(1) )
    {
        memset( &stats, 0, sizeof(selectStats) );

        stats.mActRestoreFromRecycleBinEnabled = true;

        return;
    }

    stats.mActEmptyRecycleBinEnabled = false;
    stats.mActRestoreFromRecycleBinEnabled = false;

    OFS::_Ofs::NameOfsPtrMap fsLinks;

    QString path = item->whatsThis(0);

    OFS::OfsPtr& file = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();
    unsigned int flags = 0;

    if(path.endsWith("/"))
    {
        file->getDirFlags(path.toStdString().c_str(), flags);
        file->getDirectoryLinks(path.toStdString().c_str(), fsLinks);
    }
    else
        file->getFileFlags(path.toStdString().c_str(), flags);

    if(path == "/")
    {
        stats.mActMakeAssetEnabled = false;
        stats.mActMakeAssetChecked = false;
        stats.mActReadOnlyEnabled = false;
        stats.mActReadOnlyChecked = false;
        stats.mActHiddenEnabled = false;
        stats.mActHiddenChecked = false;
        stats.mActDeleteEnabled = false;
    }
    else
    {
        if(path.endsWith("/"))
        {
            if( flags & OFS::OFS_LINK )
            {
                stats.mActLinkFileSystemEnabled = false;
                stats.mActImportFileEnabled = false;
                stats.mActImportFolderEnabled = false;
            }

            Ogre::StringVector dirs = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ResourceDirectories;
            Ogre::StringVector::iterator it;

            bool asset_state = false;

            for(it = dirs.begin(); it != dirs.end(); it++)
            {
                if((*it) == path.toStdString())
                    asset_state = true;
            }

            if( asset_state == false )
                stats.mActMakeAssetChecked = false;
        }
        else
        {
            stats.mActLinkFileSystemEnabled = false;
            stats.mActMakeAssetEnabled = false;
            stats.mActMakeAssetChecked = false;
            stats.mActImportFileEnabled = false;
            stats.mActImportFolderEnabled = false;
            stats.mActAddEmptyFolderEnabled = false;
        }

        if( flags & OFS::OFS_LINK )
        {
            stats.mActReadOnlyEnabled = false;
            stats.mActHiddenEnabled = false;
            stats.mActDeleteEnabled = false;
            stats.mActAddEmptyFolderEnabled = false;
            stats.mActImportFileEnabled = false;
            stats.mActImportFolderEnabled = false;
        }

        if( !(flags & OFS::OFS_READONLY) )
            stats.mActReadOnlyChecked = false;

        if( !(flags & OFS::OFS_HIDDEN) )
            stats.mActHiddenChecked = false;

    }
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onOfsWidgetCustomContextMenuRequested(const QPoint & pos)
{
    QList<QTreeWidgetItem*> selItems = mOfsTreeWidget->selectedItems();

    mAddFileFolderPath = "/";

    QSignalMapper * mapper = new QSignalMapper(this);

    selectStats stats;

    stats.mActMakeAssetEnabled = true;
    stats.mActMakeAssetChecked = true;
    stats.mActReadOnlyEnabled = true;
    stats.mActReadOnlyChecked = true;
    stats.mActHiddenEnabled = true;
    stats.mActHiddenChecked = true;
    stats.mActDeleteEnabled = true;
    stats.mActLinkFileSystemEnabled = true;
    stats.mActImportFileEnabled = true;
    stats.mActImportFolderEnabled = true;
    stats.mActAddEmptyFolderEnabled = true;
    stats.mActEmptyRecycleBinEnabled = true;
    stats.mActRestoreFromRecycleBinEnabled = true;

    for( int s = 0; s < selItems.size() ; s++ )
    {
        modifyStats(stats, selItems[s]);
    }


    OFS::OfsPtr& file = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();

    mUnlinkFileSystem->clear();
    mUnlinkFileSystem->setEnabled(false);

    if(selItems.size() == 0)
    {
        memset( &stats, 0, sizeof(selectStats));
    }
    else if(selItems.size() == 1)
    {
        if( stats.mActLinkFileSystemEnabled )
        {
            OFS::_Ofs::NameOfsPtrMap fsLinks;
            file->getDirectoryLinks(selItems[0]->whatsThis(0).toStdString().c_str(), fsLinks);
   
            OFS::_Ofs::NameOfsPtrMap::iterator fit = fsLinks.begin();

            while( fit != fsLinks.end() )
            {
                QAction *act = mUnlinkFileSystem->addAction(QString(fit->first.c_str()));

                connect(act, SIGNAL(triggered()), mapper, SLOT(map()));
                mapper->setMapping(act, QString(fit->first.c_str()) );

                fit++;
            }

            connect(mapper, SIGNAL(mapped(const QString &)), this, SLOT(onUnlinkFileSystem(const QString &)));

            mUnlinkFileSystem->setEnabled(fsLinks.size() > 0);
        }

        mAddFileFolderPath = selItems[0]->whatsThis(0).toStdString();
    }
    else
    {
        stats.mActAddEmptyFolderEnabled = false;
        stats.mActImportFileEnabled = false;
        stats.mActImportFolderEnabled = false;
        stats.mActMakeAssetEnabled = false;
        stats.mActLinkFileSystemEnabled = false;
        stats.mActEmptyRecycleBinEnabled = false;
    }

	if( mMenu->actions().at(0) !=  mOgitorMainWindow->actEditRename)
	{
		mMenu->insertAction(mActEmptyRecycleBin, mOgitorMainWindow->actEditRename);
		mMenu->insertSeparator(mActEmptyRecycleBin);
	}


    mActMakeAsset->setEnabled( stats.mActMakeAssetEnabled );
    mActMakeAsset->setChecked( stats.mActMakeAssetChecked & stats.mActMakeAssetEnabled );
    mActReadOnly->setEnabled( stats.mActReadOnlyEnabled );
    mActReadOnly->setChecked( stats.mActReadOnlyChecked & stats.mActReadOnlyEnabled );
    mActHidden->setEnabled( stats.mActHiddenEnabled );
    mActHidden->setChecked( stats.mActHiddenChecked & stats.mActHiddenEnabled );
    mActDelete->setEnabled( stats.mActDeleteEnabled );
    mActLinkFileSystem->setEnabled( stats.mActLinkFileSystemEnabled );
    mActImportFile->setEnabled( stats.mActImportFileEnabled );
    mActImportFolder->setEnabled( stats.mActImportFolderEnabled );
    mActAddFolder->setEnabled( stats.mActAddEmptyFolderEnabled );
    mActEmptyRecycleBin->setVisible( stats.mActEmptyRecycleBinEnabled );
    mActRestoreFromRecycleBin->setVisible( stats.mActRestoreFromRecycleBinEnabled );

    QPoint globalPos = mOfsTreeWidget->mapToGlobal(pos);
    mMenu->exec(globalPos);

    delete mapper;
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
void ProjectFilesViewWidget::onLinkFileSystem()
{
    QStringList selItems = mOfsTreeWidget->getSelectedItems();
    Ogitors::OgitorsSystem *mSystem = Ogitors::OgitorsSystem::getSingletonPtr();
    OFS::OfsPtr& ofsFile = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();

    if(selItems.size() > 0 && ofsFile.valid())
    {
        Ogitors::UTFStringVector extlist;
        extlist.push_back(OTR("Ogitor File System File"));
        extlist.push_back("*.ofs");

        Ogre::UTFString importfile = mSystem->GetSetting("system", "oldOpenPath", "");
        importfile = mSystem->DisplayOpenDialog(OTR("Link File System"), extlist, importfile);
        if(importfile == "") 
            return;

        mSystem->SetSetting("system", "oldOpenPath", importfile);

        QString name = selItems.at(0);

        Ogitors::LINKDATA data;

        data.FileSystem = importfile;
        data.Directory = name.toStdString();

        if( ofsFile->linkFileSystem( data.FileSystem.c_str(), data.Directory.c_str()) == OFS::OFS_OK )
        {
            Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectOptions()->FileSystemLinks.push_back( data );

            ofsFile->rebuildUUIDMap();
        }

        mOfsTreeWidget->refreshWidget();
    }
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onUnlinkFileSystem( const QString & text )
{
    QStringList selItems = mOfsTreeWidget->getSelectedItems();
    OFS::OfsPtr& ofsFile = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();

    if(selItems.size() > 0 && ofsFile.valid())
    {
        QString name = selItems.at(0);

        if( ofsFile->unlinkFileSystem( text.toStdString().c_str(), name.toStdString().c_str()) == OFS::OFS_OK )
        {
            std::vector<Ogitors::LINKDATA> & links = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectOptions()->FileSystemLinks;

            for( unsigned int i = 0; i < links.size(); i++ )
            {
                if( links[i].FileSystem == text.toStdString() && links[i].Directory == name.toStdString() )
                {
                    links.erase( links.begin() + i );
                    break;
                }
            }

            ofsFile->rebuildUUIDMap();
        }

        mOfsTreeWidget->refreshWidget();
    }
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onEmptyRecycleBin()
{
    OFS::OfsPtr& ofsFile = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();

    ofsFile->emptyRecycleBin();

    mOfsTreeWidget->refreshWidget();   
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onRestoreFromRecycleBin()
{
    QStringList selItems = mOfsTreeWidget->getSelectedItems();
    OFS::OfsPtr& ofsFile = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();

    if(selItems.size() > 0 && ofsFile.valid())
    {
        if(QMessageBox::information(QApplication::activeWindow(), "qtOgitor", tr("Are you sure you want to restore selected files/folders?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            for(int i = 0;i < selItems.size();i++)
            {
                int id = selItems.at(i).toInt();

                OFS::OfsResult ret = ofsFile->restoreFromRecycleBin( id );

                if( ret == OFS::OFS_ACCESS_DENIED )
                {
                    QMessageBox::information(QApplication::activeWindow(), "qtOgitor", tr("Restore failed for selected item(s) due to name collision."), QMessageBox::Ok);
                }
            }

            mOfsTreeWidget->refreshWidget();            
        }
    }
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

                ofsFile->moveToRecycleBin( name.toStdString().c_str() );
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
