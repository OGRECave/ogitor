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
#include "entityview.hxx"
#include "templateview.hxx"
#include "genericimageeditor.hxx"

#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "DefaultEvents.h"
#include "EventManager.h"

//----------------------------------------------------------------------------------------
ProjectFilesViewWidget::ProjectFilesViewWidget(QWidget *parent) :
    QWidget(parent), ofsWidget(0)
{
    vboxLayout = new QVBoxLayout(this);
    vboxLayout->setMargin(0);
    vboxLayout->setSpacing(0);
    menuCommands = new QMenu();

    actCommandRefresh = new QAction(tr("Refresh"), this);
    actCommandRefresh->setStatusTip(tr("Refresh file contents"));
    actCommandRefresh->setIcon( QIcon( ":/icons/refresh.svg" ));
     
    actCommandExtract = new QAction(tr("Extract To..."), this);
    actCommandExtract->setStatusTip(tr("Extract file contents to.."));
    actCommandExtract->setIcon( QIcon( ":/icons/extract.svg" ));

    actCommandDefrag = new QAction(tr("Defrag"), this);
    actCommandDefrag->setStatusTip(tr("Defragment file"));
    actCommandDefrag->setIcon( QIcon( ":/icons/defrag.svg" ));

    actCommandDelete = new QAction(tr("Delete"), this);
    actCommandDelete->setStatusTip(tr("Delete Selected File/Folder"));
    actCommandDelete->setIcon( QIcon( ":/icons/trash.svg"));

    actCommandAddFolder = new QAction(tr("Add Folder"), this);
    actCommandAddFolder->setStatusTip(tr("Add Folder"));
    actCommandAddFolder->setIcon( QIcon( ":/icons/import.svg"));

    actCommandAddFile = new QAction(tr("Add File"), this);
    actCommandAddFile->setStatusTip(tr("Add a single File"));
    actCommandAddFile->setIcon( QIcon( ":/icons/additional.svg"));

    actCommandMakeAsset = new QAction(tr("Make scene asset"), this);
    actCommandMakeAsset->setStatusTip(tr("Make the resource usable as a scene asset"));
    actCommandMakeAsset->setIcon( QIcon( ":/icons/objects.svg"));
    actCommandMakeAsset->setCheckable(true);
    actCommandMakeAsset->setChecked(false);

    actCommandRename = new QAction(tr("Rename"), this);
    actCommandRename->setStatusTip(tr("Rename Selected File/Folder"));
    actCommandRename->setIcon( QIcon( ":/icons/editrename.svg"));

    actCommandReadOnly = new QAction(tr("Read Only"), this);
    actCommandReadOnly->setStatusTip(tr("Set/UnSet File/Folder as Read Only"));
    actCommandReadOnly->setCheckable(true);
    actCommandReadOnly->setChecked(false);

    actCommandHidden = new QAction(tr("Hidden"), this);
    actCommandHidden->setStatusTip(tr("Set/UnSet File/Folder as Hidden"));
    actCommandHidden->setCheckable(true);
    actCommandHidden->setChecked(false);

    menuCommands->addAction(actCommandAddFile);
    menuCommands->addAction(actCommandAddFolder);
    menuCommands->addSeparator();
    menuCommands->addAction(actCommandMakeAsset);
    menuCommands->addSeparator();
    menuCommands->addAction(actCommandRename);
    menuCommands->addAction(actCommandReadOnly);
    menuCommands->addAction(actCommandHidden);
    menuCommands->addSeparator();
    menuCommands->addAction(actCommandDelete);

    actCommandAddFile->setEnabled(false);
    actCommandAddFolder->setEnabled(false);
    actCommandMakeAsset->setEnabled(false);
    actCommandRefresh->setEnabled(false);
    actCommandExtract->setEnabled(false);
    actCommandDefrag->setEnabled(false);
    actCommandDelete->setEnabled(false);
    actCommandRename->setEnabled(false);
    actCommandReadOnly->setEnabled(false);
    actCommandHidden->setEnabled(false);

    connect(actCommandAddFile,      SIGNAL(triggered()), this, SLOT(onCommandAddFile()));
    connect(actCommandAddFolder,    SIGNAL(triggered()), this, SLOT(onCommandAddFolder()));
    connect(actCommandMakeAsset,    SIGNAL(triggered()), this, SLOT(onCommandMakeAsset()));
    connect(actCommandRefresh,      SIGNAL(triggered()), this, SLOT(onCommandRefresh()));
    connect(actCommandExtract,      SIGNAL(triggered()), this, SLOT(onCommandExtract()));
    connect(actCommandDefrag,       SIGNAL(triggered()), this, SLOT(onCommandDefrag()));
    connect(actCommandDelete,       SIGNAL(triggered()), this, SLOT(onCommandDelete()));
    connect(actCommandRename,       SIGNAL(triggered()), this, SLOT(onCommandRename()));
    connect(actCommandReadOnly,     SIGNAL(triggered()), this, SLOT(onCommandReadOnly()));
    connect(actCommandHidden,       SIGNAL(triggered()), this, SLOT(onCommandHidden()));

    toolBar = new QToolBar();
    toolBar->setIconSize(QSize(16, 16));
    toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    toolBar->addAction(actCommandRefresh);
    toolBar->addSeparator();
    toolBar->addAction(actCommandExtract);
    toolBar->addAction(actCommandDefrag);

    addFileFolderPath = "/";
}
//----------------------------------------------------------------------------------------
ProjectFilesViewWidget::~ProjectFilesViewWidget()
{
    delete menuCommands;
    menuCommands = 0;

    delete toolBar;
    toolBar = 0;
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::prepareView()
{
    ofsWidget = new OfsTreeWidget(this, OfsTreeWidget::CAP_FULL_FUNCTIONS);
    
    actCommandRefresh->setEnabled(true);
    actCommandExtract->setEnabled(true);
    actCommandDefrag->setEnabled(true);

    vboxLayout->addWidget(toolBar, 0);
    vboxLayout->addWidget(ofsWidget, 1);

    connect(ofsWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));
    connect(ofsWidget, SIGNAL(customContextMenuRequested ( const QPoint &)), this, SLOT(ofsWidgetCustomContextMenuRequested ( const QPoint &)));
    connect(ofsWidget, SIGNAL(busyState(bool)), this, SLOT(ofsWidgetBusyState(bool)));
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::clearView()
{
    if(ofsWidget != 0)
    {
        disconnect(ofsWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));
        disconnect(ofsWidget, SIGNAL(customContextMenuRequested ( const QPoint &)), this, SLOT(ofsWidgetCustomContextMenuRequested ( const QPoint &)));
        disconnect(ofsWidget, SIGNAL(busyState(bool)), this, SLOT(ofsWidgetBusyState(bool)));
        
        delete ofsWidget;
        ofsWidget = 0;

        vboxLayout->removeWidget(toolBar);
    }

    actCommandRefresh->setEnabled(false);
    actCommandExtract->setEnabled(false);
    actCommandDefrag->setEnabled(false);
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::itemDoubleClicked(QTreeWidgetItem * item, int column)
{
    if(item != NULL)
    {
        QString path = item->whatsThis(0);

        if(!path.endsWith("/"))
        {
            path = QString(Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile()->getFileSystemName().c_str()) + QString("::") + path;
                
            if(!mOgitorMainWindow->getGenericTextEditor()->displayTextFromFile(path))
                mOgitorMainWindow->getGenericImageEditor()->displayImageFromFile(path);
        }
    }
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::ofsWidgetCustomContextMenuRequested( const QPoint & pos )
{
    QList<QTreeWidgetItem*> selItems = ofsWidget->selectedItems();
 
    addFileFolderPath = "/";

    if(selItems.size() > 0)
    {
        QString path = selItems[0]->whatsThis(0);
 
        if(path == "/")
        {
            actCommandMakeAsset->setEnabled(false);
            actCommandMakeAsset->setChecked(false);
            actCommandReadOnly->setChecked(false);
            actCommandHidden->setChecked(false);
            actCommandReadOnly->setEnabled(false);
            actCommandHidden->setEnabled(false);
            actCommandRename->setEnabled(false);
            actCommandDelete->setEnabled(false);
        }
        else
        {
            if(path.endsWith("/"))
                addFileFolderPath = path.toStdString();

            actCommandMakeAsset->setEnabled(true);
            actCommandMakeAsset->setChecked(true);
            actCommandReadOnly->setChecked(true);
            actCommandHidden->setChecked(true);
            actCommandReadOnly->setEnabled(true);
            actCommandHidden->setEnabled(true);
            actCommandRename->setEnabled(true);
            actCommandDelete->setEnabled(true);
        }

        OFS::OfsPtr& file = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();
        unsigned int flags = 0;

        if(path.endsWith("/"))
            file->getDirFlags(path.toStdString().c_str(), flags);
        else
            file->getFileFlags(path.toStdString().c_str(), flags);

        actCommandReadOnly->setChecked(flags & OFS::OFS_READONLY);
        actCommandHidden->setChecked(flags & OFS::OFS_HIDDEN);
       
        Ogre::StringVector dirs = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ResourceDirectories;
        Ogre::StringVector::iterator it;

        for(it = dirs.begin(); it != dirs.end(); it++)
        {
            if((*it) == path.toStdString())
                actCommandMakeAsset->setChecked(true);
            else
                actCommandMakeAsset->setChecked(false);
        }
    }
    else
    {
        actCommandMakeAsset->setEnabled(false);
        actCommandMakeAsset->setChecked(false);
        actCommandReadOnly->setChecked(false);
        actCommandHidden->setChecked(false);
        actCommandReadOnly->setEnabled(false);
        actCommandHidden->setEnabled(false);
        actCommandRename->setEnabled(false);
        actCommandDelete->setEnabled(false);
    }

    actCommandAddFile->setEnabled(true);
    actCommandAddFolder->setEnabled(true);

    QPoint globalPos = ofsWidget->mapToGlobal(pos);
    menuCommands->exec( globalPos );
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::ofsWidgetBusyState(bool state)
{
    actCommandAddFile->setEnabled(!state);
    actCommandAddFolder->setEnabled(!state);
    actCommandRefresh->setEnabled(!state);
    actCommandExtract->setEnabled(!state);
    actCommandDefrag->setEnabled(!state);
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onCommandRefresh()
{
    ofsWidget->refreshWidget();
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onCommandExtract()
{
    ofsWidget->extractFiles();
}
//----------------------------------------------------------------------------------------
void ofsCallback(std::string msg)
{
    mOgitorMainWindow->updateLog(new QListWidgetItem(msg.c_str(), 0, 2));
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onCommandDefrag()
{
    OFS::OfsPtr& ofsFile = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();

    if(ofsFile.valid())
    {
        QString fileName = ofsFile->getFileSystemName().c_str();
        QString tmpFile = fileName + ".tmp";
        
        // Step-1 defrag the file to temp file
        OFS::LogCallBackFunction callback = ofsCallback;
        ofsFile->defragFileSystemTo(tmpFile.toStdString().c_str(), &callback);
        // Step-2 make the temp file active file, so that we can overwrite the original file
        ofsFile->switchFileSystemTo(tmpFile.toStdString().c_str());
        // Step-3 copy temp file over original file and make it active
        ofsFile->moveFileSystemTo(fileName.toStdString().c_str());
        // Step-4 remove temp file
        QFile::remove(tmpFile);

        ofsWidget->refreshWidget();
        mOgitorMainWindow->scrollLogToBottom();
    }
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onCommandDelete()
{
    QList<QTreeWidgetItem*> selItems = ofsWidget->selectedItems();
    OFS::OfsPtr& ofsFile = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile();

    if(selItems.size() > 0 && ofsFile.valid())
    {
        if(QMessageBox::information(QApplication::activeWindow(),"qtOgitor", tr("Are you sure you want to delete selected files?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            for(int i = 0;i < selItems.size();i++)
            {
                QString name = selItems[i]->whatsThis(0);

                if(name.endsWith("/"))
                    ofsFile->deleteDirectory(name.toStdString().c_str(), true);
                else
                    ofsFile->deleteFile(name.toStdString().c_str());
            }
            
            ofsWidget->refreshWidget();            
        }
    }
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onCommandRename()
{
    QList<QTreeWidgetItem*> selItems = ofsWidget->selectedItems();
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
            
            ofsWidget->refreshWidget();            
        } 
    }
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onCommandReadOnly()
{
    QList<QTreeWidgetItem*> selItems = ofsWidget->selectedItems();
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
                
                if(actCommandReadOnly->isChecked())
                    new_flag |= OFS::OFS_READONLY;
                else
                    new_flag &= ~OFS::OFS_READONLY;

                ofsFile->setDirFlags(name.toStdString().c_str(), new_flag);
            }
            else
            {
                ofsFile->getFileFlags(name.toStdString().c_str(), new_flag);
                
                if(actCommandReadOnly->isChecked())
                    new_flag |= OFS::OFS_READONLY;
                else
                    new_flag &= ~OFS::OFS_READONLY;

                ofsFile->setFileFlags(name.toStdString().c_str(), new_flag);
            }
        }
            
        ofsWidget->refreshWidget();            
    }
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onCommandHidden()
{
    QList<QTreeWidgetItem*> selItems = ofsWidget->selectedItems();
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
                
                if(actCommandHidden->isChecked())
                    new_flag |= OFS::OFS_HIDDEN;
                else
                    new_flag &= ~OFS::OFS_HIDDEN;

                ofsFile->setDirFlags(name.toStdString().c_str(), new_flag);
            }
            else
            {
                ofsFile->getFileFlags(name.toStdString().c_str(), new_flag);
                
                if(actCommandHidden->isChecked())
                    new_flag |= OFS::OFS_HIDDEN;
                else
                    new_flag &= ~OFS::OFS_HIDDEN;

                ofsFile->setFileFlags(name.toStdString().c_str(), new_flag);
            }
        }
            
        ofsWidget->refreshWidget();            
    }
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onCommandAddFolder()
{
    Ogitors::OgitorsSystem *mSystem = Ogitors::OgitorsSystem::getSingletonPtr();
    Ogre::String folderName = mSystem->DisplayDirectorySelector(OTR("Choose folder to import"));

    QStringList folders(folderName.c_str());
    if(!folders.empty())
        ofsWidget->addFiles(addFileFolderPath.c_str(), folders);
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onCommandAddFile()
{
    Ogitors::OgitorsSystem *mSystem = Ogitors::OgitorsSystem::getSingletonPtr();
    Ogre::String fileName = mSystem->DisplayOpenDialog(OTR("Choose folder to import"), Ogitors::UTFStringVector());

    QStringList files(fileName.c_str());
    if(!files.empty())
        ofsWidget->addFiles(addFileFolderPath.c_str(), files);
}
//----------------------------------------------------------------------------------------
void ProjectFilesViewWidget::onCommandMakeAsset()
{
    QList<QTreeWidgetItem*> selItems = ofsWidget->selectedItems();
    QString name = selItems[0]->whatsThis(0);
    Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ResourceDirectories.push_back(name.toStdString());
    Ogitors::OgitorsRoot::getSingletonPtr()->ReloadUserResources();
    mOgitorMainWindow->getEntityViewWidget()->prepareView();
    mOgitorMainWindow->getTemplatesViewWidget()->prepareView();

    // Send event so that all listening plugins get notified as well and can update their views
    Ogitors::GlobalPrepareViewEvent evt;
    Ogitors::EventManager::getSingletonPtr()->sendEvent(this, 0, &evt);
}
//----------------------------------------------------------------------------------------