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
#include "filelistwidget.hxx"

//------------------------------------------------------------------------------
void MainWindow::addActions()
{
    actNew = new QAction(tr("Create New"), this);
    actNew->setStatusTip(tr("Create a new Ofs file"));
    actNew->setIcon( QIcon( ":/icons/ogscene.png" ));
    actNew->setShortcut(QKeySequence::New);

    actOpen = new QAction(tr("Open"), this);
    actOpen->setStatusTip(tr("Open an Ofs file"));
    actOpen->setIcon( QIcon( ":/icons/fileopen.svg" ));
    actOpen->setShortcut(QKeySequence::Open);

    actClose = new QAction(tr("Close"), this);
    actClose->setStatusTip(tr("Close Ofs file"));
    actClose->setIcon( QIcon( ":/icons/fileclose.svg" ));

    actExtract = new QAction(tr("Extract To..."), this);
    actExtract->setStatusTip(tr("Extract file contents to.."));
    actExtract->setIcon( QIcon( ":/icons/extract.svg" ));
    actExtract->setShortcut(QKeySequence::Save);

    actDefrag = new QAction(tr("Defrag"), this);
    actDefrag->setStatusTip(tr("Defragment file"));
    actDefrag->setIcon( QIcon( ":/icons/defrag.svg" ));

    actExit = new QAction(tr("Exit"), this);
    actExit->setStatusTip(tr("Exit Application"));
    actExit->setIcon( QIcon( ":/icons/exit.svg" ));
    actExit->setShortcut(QKeySequence::Close);

    actAbout = new QAction(tr("About"), this);
    actAbout->setStatusTip(tr("About qtOfs"));
    actAbout->setIcon( QIcon( ":/icons/about.svg"));

    actHelp = new QAction(tr("Help"), this);
    actHelp->setStatusTip(tr("qtOfs Help"));
    actHelp->setIcon( QIcon( ":/icons/help.svg"));
    actHelp->setShortcut(QKeySequence::HelpContents);

    actOpenPreferences = new QAction(tr("Preferences"), this);
    actOpenPreferences->setIcon(QIcon(":/icons/preferences.svg"));
    actOpenPreferences->setStatusTip(tr("Preferences"));

    actCommandDelete = new QAction(tr("Delete"), this);
    actCommandDelete->setStatusTip(tr("Delete Selected File/Folder"));
    actCommandDelete->setIcon( QIcon( ":/icons/trash.svg"));
    actCommandDelete->setShortcut(QKeySequence::Delete);

    actCommandRename = new QAction(tr("Rename"), this);
    actCommandRename->setStatusTip(tr("Rename Selected File/Folder"));
    actCommandRename->setIcon( QIcon( ":/icons/editrename.svg"));
    actCommandRename->setShortcut(QKeySequence("F2"));

    actCommandReadOnly = new QAction(tr("Read Only"), this);
    actCommandReadOnly->setStatusTip(tr("Set/UnSet File/Folder as Read Only"));
    actCommandReadOnly->setCheckable(true);
    actCommandReadOnly->setChecked(false);

    actCommandHidden = new QAction(tr("Hidden"), this);
    actCommandHidden->setStatusTip(tr("Set/UnSet File/Folder as Hidden"));
    actCommandHidden->setCheckable(true);
    actCommandHidden->setChecked(false);

    actViewShowHidden = new QAction(tr("Show Hidden Files/Folders"), this);
    actViewShowHidden->setStatusTip(tr("Show Hidden Files/Folders"));
    actViewShowHidden->setCheckable(true);
    actViewShowHidden->setChecked(false);

    connect(actAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
    connect(actHelp, SIGNAL(triggered()), this, SLOT(showHelp()));
    connect(actNew, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(actOpen, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(actClose, SIGNAL(triggered()), this, SLOT(closeFile()));
    connect(actExtract, SIGNAL(triggered()), this, SLOT(extractTo()));
    connect(actDefrag, SIGNAL(triggered()), this, SLOT(defragFile()));
    connect(actExit, SIGNAL(triggered()), this, SLOT(exitApp()));
    connect(actOpenPreferences, SIGNAL(triggered()), this, SLOT(openPreferences()));
    connect(actCommandDelete, SIGNAL(triggered()), this, SLOT(commandDelete()));
    connect(actCommandRename, SIGNAL(triggered()), this, SLOT(commandRename()));
    connect(actCommandReadOnly, SIGNAL(triggered()), this, SLOT(commandReadOnly()));
    connect(actCommandHidden, SIGNAL(triggered()), this, SLOT(commandHidden()));
    connect(actViewShowHidden, SIGNAL(triggered()), this, SLOT(viewShowHidden()));

    QMetaObject::connectSlotsByName(this);
}
//------------------------------------------------------------------------------
void MainWindow::updateLoadTerminateActions(bool loaded)
{
    QString appTitle = "qtOfs ";
    
    if(loaded)
    {
        appTitle += "- ";
        appTitle += mOfsFile->getFileSystemName().c_str();
    }

    setWindowTitle(appTitle);


    actExtract->setEnabled(loaded);
    actClose->setEnabled(loaded);
    actDefrag->setEnabled(loaded);
    actNew->setEnabled(!loaded);

    if(loaded)
        mDirectoryDisplay->setText(QString(mCurrentDir.c_str()));
    else
    {
        mDirectoryDisplay->setText(QString(""));
        mDirUpButton->setEnabled(false);
    }

    actCommandDelete->setEnabled(false);
    actCommandRename->setEnabled(false);
    actCommandReadOnly->setEnabled(false);
    actCommandHidden->setEnabled(false);

    actCommandReadOnly->setChecked(false);
    actCommandHidden->setChecked(false);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//-----ACTION FUNCTIONS---------------------------------------------------------
//------------------------------------------------------------------------------
void MainWindow::showAbout()
{
}
//------------------------------------------------------------------------------
void MainWindow::showHelp()
{
}
//------------------------------------------------------------------------------
void MainWindow::newFile()
{
    if(mOfsFile.valid())
    {
        closeFile();
    }

    QString path = QFileDialog::getSaveFileName(QApplication::activeWindow(), tr("Create File"), "", tr("Ofs Files (*.ofs)"), 0
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    );
#else
    , QFileDialog::DontUseNativeDialog );
#endif

    if(path != "")
    {
        try
        {
            mOfsFile.mount(path.toStdString().c_str(), OFS::OFS_MOUNT_CREATE);

            mCurrentDir = "/";

            showFiles();

            updateLoadTerminateActions(true);
        }
        catch(OFS::Exception& e)
        {
            QMessageBox::information(QApplication::activeWindow(),"Ofs Exception:", QString(e.getDescription().c_str()), QMessageBox::Ok);
        }
    }
}
//------------------------------------------------------------------------------
void MainWindow::openOfsFile(QString filename)
{
    if(mOfsFile.valid())
        closeFile();

    try
    {
        OFS::OfsResult ret = mOfsFile.mount(filename.toStdString().c_str(), OFS::OFS_MOUNT_OPEN | OFS::OFS_MOUNT_RECOVER);

        if(ret == OFS::OFS_PREVIOUS_VERSION && QMessageBox::information(QApplication::activeWindow(),"Old File Version", tr("File you want to open is of an old format.") + QString("\n") + tr("Would you like to upgrade it?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            OFS::OfsConverter conv;

            QString filename2 = filename + QString(".tmp");
            if(conv.convert(filename.toStdString(), filename2.toStdString()))
            {
                QFile::remove(filename);
                QFile::rename(filename2, filename);
            }
            else
                QFile::remove(filename2);

           ret = mOfsFile.mount(filename.toStdString().c_str(), OFS::OFS_MOUNT_OPEN | OFS::OFS_MOUNT_RECOVER);
        }

        if(ret != OFS::OFS_OK)
        {
            QMessageBox::information(QApplication::activeWindow(),"qtOfs", tr("Error reading file : ") + filename, QMessageBox::Ok);
            return;
        }

        mCurrentDir = "/";

        showFiles();

        updateLoadTerminateActions(true);
    }
    catch(OFS::Exception& e)
    {
        QMessageBox::information(QApplication::activeWindow(),"Ofs Exception:", QString(e.getDescription().c_str()), QMessageBox::Ok);
    }
}
//------------------------------------------------------------------------------
void MainWindow::openFile()
{
    if(mOfsFile.valid())
    {
        closeFile();
    }

    QString path = QFileDialog::getOpenFileName(QApplication::activeWindow(), tr("Open File"), "", tr("Ofs Files (*.ofs)"), 0
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    );
#else
    , QFileDialog::DontUseNativeDialog );
#endif

    if(path != "")
    {
        openOfsFile(path);
    }
}
//------------------------------------------------------------------------------
void MainWindow::closeFile()
{
    if(mOfsFile.valid())
    {
        mOfsFile.unmount();

        updateLoadTerminateActions(false);

        showFiles();
    }
}
//------------------------------------------------------------------------------
void MainWindow::defragFile()
{
    if(mOfsFile.valid())
    {
        QString fileName = mOfsFile->getFileSystemName().c_str();
        QString tmpFile = fileName + ".tmp";
        mOfsFile->defragFileSystemTo(tmpFile.toStdString().c_str());

        closeFile();

        QFile::remove(fileName);
        QFile::rename(tmpFile, fileName);

        openOfsFile(fileName);
    }
}
//------------------------------------------------------------------------------
void MainWindow::dirUpOneLevel()
{
    if(mOfsFile.valid())
    {
        mCurrentDir.erase(mCurrentDir.length() - 1, 1);
        int pos = mCurrentDir.find_last_of("/");
        mCurrentDir.erase(pos + 1, mCurrentDir.length() - pos - 1);

        showFiles();
    }
}
//------------------------------------------------------------------------------
void MainWindow::exitApp()
{
    close();
}
//------------------------------------------------------------------------------
void MainWindow::extractTo()
{
    if(!mOfsFile.valid())
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
       
        if(mSelectedFiles.size() == 0)
        {
            for(unsigned int i = 0;i < mCurrentFiles.size();i++)
            {
                if(mCurrentFiles[i].name != "..")
                    theList.push_back(mCurrentFiles[i]);
            }

            extractFiles(path, theList);
        }
        else
            extractFiles(path, mSelectedFiles);
    }
}
//------------------------------------------------------------------------------
void MainWindow::openPreferences()
{
}
//------------------------------------------------------------------------------
void MainWindow::commandDelete()
{
    if(mSelectedFiles.size() > 0 && mOfsFile.valid() && !isBusy())
    {
        if(QMessageBox::information(QApplication::activeWindow(),"qtOFS", tr("Are you sure you want to delete selected files?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            for(unsigned int i = 0;i < mSelectedFiles.size();i++)
            {
                std::string name = mCurrentDir + mSelectedFiles[i].name;

                if(mSelectedFiles[i].flags & OFS::OFS_DIR)
                    mOfsFile->deleteDirectory(name.c_str(), true);
                else
                    mOfsFile->deleteFile(name.c_str());
            }
            
            showFiles();
        }
    }
}
//------------------------------------------------------------------------------
void MainWindow::commandRename()
{
    if(mSelectedFiles.size() == 1 && mOfsFile.valid() && !isBusy())
    {
        std::string name = mCurrentDir + mSelectedFiles[0].name;

        bool ok;
        QString text = QInputDialog::getText(this, tr("Rename File/Folder"), "Enter new name:", QLineEdit::Normal, QString(mSelectedFiles[0].name.c_str()), &ok);
        if ( ok && !text.isEmpty() ) 
        {
            // user entered something and pressed OK
            if(mSelectedFiles[0].flags & OFS::OFS_DIR)
                mOfsFile->renameDirectory(name.c_str(), text.toStdString().c_str());
            else
                mOfsFile->renameFile(name.c_str(), text.toStdString().c_str());
            
            showFiles();
        } 
    }
}
//------------------------------------------------------------------------------
void MainWindow::commandReadOnly()
{
    if(mSelectedFiles.size() > 0 && mOfsFile.valid() && !isBusy())
    {
        for(unsigned int i = 0;i < mSelectedFiles.size();i++)
        {
            std::string name = mCurrentDir + mSelectedFiles[i].name;

            unsigned int new_flag = mSelectedFiles[i].flags & ~OFS::OFS_READONLY;

            if(actCommandReadOnly->isChecked())
                new_flag |= OFS::OFS_READONLY;

            if(mSelectedFiles[i].flags & OFS::OFS_DIR)
                mOfsFile->setDirFlags(name.c_str(), new_flag);
            else
                mOfsFile->setFileFlags(name.c_str(), new_flag);
        }
        
        showFiles();
    }
}
//------------------------------------------------------------------------------
void MainWindow::commandHidden()
{
    if(mSelectedFiles.size() > 0 && mOfsFile.valid() && !isBusy())
    {
        for(unsigned int i = 0;i < mSelectedFiles.size();i++)
        {
            std::string name = mCurrentDir + mSelectedFiles[i].name;

            unsigned int new_flag = mSelectedFiles[i].flags & ~OFS::OFS_HIDDEN;

            if(actCommandHidden->isChecked())
                new_flag |= OFS::OFS_HIDDEN;

            if(mSelectedFiles[i].flags & OFS::OFS_DIR)
                mOfsFile->setDirFlags(name.c_str(), new_flag);
            else
                mOfsFile->setFileFlags(name.c_str(), new_flag);
        }
        
        showFiles();
    }
}
//------------------------------------------------------------------------------
void MainWindow::viewShowHidden()
{
    if(mOfsFile.valid())
    {
        showFiles();
    }
}
//------------------------------------------------------------------------------