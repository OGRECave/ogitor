/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2015 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
///
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

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QGridLayout>
#include <QtGui/QCloseEvent>

MainWindow *mOfsMainWindow = 0;

const unsigned int MAX_BUFFER_SIZE = 1024 * 1024 * 16; // 16 MB
char tmp_buffer[MAX_BUFFER_SIZE];

//------------------------------------------------------------------------------
bool AddFilesListCompare ( AddFilesData elem1, AddFilesData elem2 )
{
    return (strcmp(elem1.ofsName.toStdString().c_str(), elem2.ofsName.toStdString().c_str()) < 0);
}
//------------------------------------------------------------------------------
MainWindow::MainWindow(QString args, QWidget *parent)
    : QMainWindow(parent), mArgsFile(args), mHasFileArgs(false)
{
    mOfsMainWindow = this;

    mUnknownFileIcon = QIcon(":/icons/filenew.svg");

    setMinimumSize(400,300);

    mRowHeight = 20;

    QIcon icon;
    icon.addPixmap(QPixmap(":/icons/qtOfs.png"), QIcon::Normal, QIcon::Off);
    setWindowIcon(icon);

    if(objectName().isEmpty())
        setObjectName(QString::fromUtf8("this"));
    resize(400, 300);

    addActions();

    addMenus();

    setupStatusBar();

    createToolbar();

    retranslateUi();

    createListWidget();

    updateLoadTerminateActions(false);

    mApplicationObject = 0;

    readSettings();

    mExtractorThread = new ExtractThread();
    mAddFilesThread = new AddFilesThread();
    mProgressTimer = new QTimer(this);
    mProgressTimer->setInterval(100);
    mProgressTimer->stop();
    connect(mExtractorThread, SIGNAL(finished()), this, SLOT(extractFinished()));
    connect(mAddFilesThread, SIGNAL(finished()), this, SLOT(addFilesFinished()));
    connect(mProgressTimer, SIGNAL(timeout()), this, SLOT(updateProgress()));

    if(!mArgsFile.isEmpty())
    {
        mHasFileArgs = true;
        openOfsFile(mArgsFile);
    }
}
//------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete mExtractorThread;
}
//------------------------------------------------------------------------------
void MainWindow::setApplicationObject(QObject *obj) 
{ 
    mApplicationObject = obj;
    mAppActive = true;
    obj->installEventFilter(this);
}
//------------------------------------------------------------------------------
void MainWindow::readSettings(QString filename)
{
    QSettings *settings;

    if(filename.isEmpty())
        settings = new QSettings();
    else
        settings = new QSettings(filename, QSettings::IniFormat);

    settings->beginGroup("session");
    restoreState(settings->value("Layout").toByteArray());
    bool maximized = settings->value("MainWindowMaximized", false).toBool();
    bool showHidden = settings->value("ShowHiddenFiles", false).toBool();
    QRect rect = settings->value("MainWindowGeometry", QRect(0, 0, 800, 600)).toRect();

    int col_width = settings->value("Column0", "0").toInt();
    if(col_width != 0)
        mFileListWidget->setColumnWidth(0, col_width);
    col_width = settings->value("Column1", "0").toInt();
    if(col_width != 0)
        mFileListWidget->setColumnWidth(1, col_width);
    col_width = settings->value("Column2", "0").toInt();
    if(col_width != 0)
        mFileListWidget->setColumnWidth(2, col_width);
    col_width = settings->value("Column3", "0").toInt();
    if(col_width != 0)
        mFileListWidget->setColumnWidth(3, col_width);

    settings->endGroup();

    if(maximized)
        setWindowState(Qt::WindowMaximized);
    else
    {
        move(rect.topLeft());
        resize(rect.size());
    }

    actViewShowHidden->setChecked(showHidden);

    delete settings;
}
//------------------------------------------------------------------------------
void MainWindow::writeSettings(QString filename)
{
    QSettings *settings;

    if(filename.isEmpty())
        settings = new QSettings();
    else
        settings = new QSettings(filename, QSettings::IniFormat);

    settings->beginGroup("session");
    settings->setValue("MainWindowGeometry", geometry());
    settings->setValue("ShowHiddenFiles", actViewShowHidden->isChecked());

    bool maximized = isMaximized();
    settings->remove("Layout");
    settings->setValue("Layout", saveState());
    settings->setValue("MainWindowMaximized", maximized);
    settings->setValue("Column0", mFileListWidget->columnWidth(0));
    settings->setValue("Column1", mFileListWidget->columnWidth(1));
    settings->setValue("Column2", mFileListWidget->columnWidth(2));
    settings->setValue("Column3", mFileListWidget->columnWidth(3));
    settings->endGroup();

    delete settings;
}
//------------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(mExtractorThread->isRunning())
    {
        event->setAccepted(false);
    }

    writeSettings();
    
    mOfsFile.unmount();

    event->setAccepted(true);    
}
//------------------------------------------------------------------------------
void MainWindow::retranslateUi()
{
    QString appTitle = "qtOfs ";
    setWindowTitle(appTitle);
    menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
}
//------------------------------------------------------------------------------
void MainWindow::createListWidget()
{
    QWidget* widget = new QWidget(this);

    setCentralWidget(widget);

    mDirectoryLabel = new QLabel(tr("Path :"));
    mDirectoryLabel->setMinimumWidth(50);
    mDirectoryLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    mDirectoryDisplay = new QLineEdit(widget);
    mDirectoryDisplay->setReadOnly(true);
    mDirUpButton = new QPushButton(QIcon(":/icons/uparrow.svg"), QString(""));
    mDirUpButton->setEnabled(false);
    mDirUpButton->setFixedSize(QSize(20, 20));
    mFileListWidget = new FileListWidget(0,5, widget);

    QGridLayout *lay = new QGridLayout(widget);
    lay->setSpacing(0);
    lay->setMargin(0);
    lay->addWidget(mDirUpButton, 0, 0);
    lay->addWidget(mDirectoryLabel, 0, 1);
    lay->addWidget(mDirectoryDisplay, 0, 2);
    lay->addWidget(mFileListWidget, 1, 0, 1, 3);

    lay->setColumnStretch(0, 0);
    lay->setColumnStretch(1, 0);
    lay->setColumnStretch(2, 1);
    lay->setRowStretch(0, 0);
    lay->setRowStretch(1, 1);

    widget->setLayout(lay);

    connect(mFileListWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(dblClick(int, int)));
    connect(mFileListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(fileListItemSelectionChanged()));
    connect(mFileListWidget, SIGNAL(customContextMenuRequested ( const QPoint &)), this, SLOT(fileListCustomContextMenuRequested ( const QPoint &)));
    
    connect(mDirUpButton, SIGNAL(clicked()), this, SLOT(dirUpOneLevel()));
}
//------------------------------------------------------------------------------
void MainWindow::createToolbar()
{
    mMainToolBar = new QToolBar(tr("File"));
    mMainToolBar->setObjectName("MainToolBar");
    mMainToolBar->setIconSize(QSize(24,24));
    mMainToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    mMainToolBar->addAction(actNew);
    mMainToolBar->addSeparator();
    mMainToolBar->addAction(actOpen);
    mMainToolBar->addAction(actExtract);
    mMainToolBar->addAction(actClose);
    mMainToolBar->addSeparator();
    mMainToolBar->addAction(actDefrag);

    addToolBar(Qt::TopToolBarArea, mMainToolBar);
}
//------------------------------------------------------------------------------
void MainWindow::addMenus()
{
    mMenuBar = new QMenuBar(this);
    mMenuBar->setObjectName(QString::fromUtf8("mMenuBar"));
    mMenuBar->setGeometry(QRect(0, 0, 616, 22));

    menuFile = new QMenu(tr("File"), mMenuBar);
    menuFile->setObjectName(QString::fromUtf8("menuFile"));
    mMenuBar->addAction(menuFile->menuAction());
    menuFile->addAction(actNew);
    menuFile->addSeparator();
    menuFile->addAction(actOpen);
    menuFile->addAction(actExtract);
    menuFile->addAction(actDefrag);
    menuFile->addAction(actClose);
    menuFile->addSeparator();
    menuFile->addAction(actExit);

    menuCommands = new QMenu(tr("Commands"), mMenuBar);
    menuCommands->setObjectName(QString::fromUtf8("menuCommands"));
    mMenuBar->addAction(menuCommands->menuAction());
    menuCommands->addAction(actCommandRename);
    menuCommands->addAction(actCommandDelete);
    menuCommands->addSeparator();
    menuCommands->addAction(actCommandReadOnly);
    menuCommands->addAction(actCommandHidden);

    menuView = new QMenu(tr("View"), mMenuBar);
    menuView->setObjectName(QString::fromUtf8("menuView"));
    mMenuBar->addAction(menuView->menuAction());
    menuView->addAction(actViewShowHidden);

    setMenuBar(mMenuBar);
}
//------------------------------------------------------------------------------
void MainWindow::setupStatusBar()
{
    mStatusBar = new QStatusBar(this);
    mStatusBar->setObjectName(QString::fromUtf8("mStatusBar"));

    mProgressBar = new QProgressBar(this);
    mStatusBar->addWidget(mProgressBar);

    mProgressLabel = new QLabel(this);
    mStatusBar->addWidget(mProgressLabel, 1);

    mProgressLabel->setText("");
    mProgressLabel->setVisible(false);

    mProgressBar->setFixedWidth(200);
    mProgressBar->setMinimum(0);
    mProgressBar->setMaximum(100);
    mProgressBar->setVisible(false);

    mTotalEntriesLabel = new QLabel(this);
    mTotalEntriesLabel->setText(tr("Total Entries : "));
    mStatusBar->addWidget(mTotalEntriesLabel, 1);

    setStatusBar(mStatusBar);
}
//------------------------------------------------------------------------------
bool inActivationEvent = false;

bool MainWindow::eventFilter(QObject *obj,  QEvent *event)
{
    if (obj == mApplicationObject  &&  !inActivationEvent)
    {
        if (event->type() == QEvent::ApplicationActivate)
        {
            // This gets called when the application starts, and when you switch back. 
            inActivationEvent = true;
            mAppActive = true;
            inActivationEvent = false;
        }

        else if (event->type() == QEvent::ApplicationDeactivate)
        {
            // This gets called when the application closes, and when you switch away. 
            inActivationEvent = true;
            mAppActive = false;
            inActivationEvent = false;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}
//------------------------------------------------------------------------------------
void MainWindow::showFiles()
{
    while(mFileListWidget->rowCount() > 0)
        mFileListWidget->removeRow(0);
    
    mCurrentFiles.clear();

    QIcon folderIcon = mIconProvider.icon(QFileIconProvider::Folder);
    
    if(mOfsFile.valid())
    {
        OFS::FileList dirs = mOfsFile->listFiles(mCurrentDir.c_str(), OFS::OFS_DIR);
        OFS::FileList files = mOfsFile->listFiles(mCurrentDir.c_str(), OFS::OFS_FILE);

        std::sort(dirs.begin(), dirs.end(), OFS::FileEntry::Compare);
        std::sort(files.begin(), files.end(), OFS::FileEntry::Compare);

        int current_row = 0;
        if(mCurrentDir.size() > 1)
        {
            mFileListWidget->insertRow(current_row);
            mFileListWidget->setRowHeight(current_row, mRowHeight);
            QTableWidgetItem *witem = new QTableWidgetItem(folderIcon, QString(".."));
            mFileListWidget->setItem(0, 0, witem);

            witem = new QTableWidgetItem("");
            mFileListWidget->setItem(current_row, 1, witem);

            witem = new QTableWidgetItem(tr("Directory"));
            mFileListWidget->setItem(current_row, 2, witem);

            witem = new QTableWidgetItem("");
            mFileListWidget->setItem(current_row, 3, witem);

            witem = new QTableWidgetItem("");
            mFileListWidget->setItem(current_row, 4, witem);

            current_row++;

            OFS::FileEntry updata;
            updata.flags = OFS::OFS_DIR;
            updata.name = "..";
            updata.file_size = 0;

            mCurrentFiles.push_back(updata);
        }

        mDirUpButton->setEnabled(mCurrentDir.size() > 1);

        for(unsigned int i = 0;i < dirs.size();i++)
        {
            mFileListWidget->insertRow(current_row);
            mFileListWidget->setRowHeight(current_row, mRowHeight);

            bool isReadOnly = (dirs[i].flags & OFS::OFS_READONLY) > 0;
            bool isHidden = (dirs[i].flags & OFS::OFS_HIDDEN) > 0;

            if(isHidden && (actViewShowHidden->isChecked() == false))
                continue;

            QColor textColor = Qt::black;

            if(isReadOnly && isHidden)
                textColor = QColor(255, 210, 210);
            else if(isReadOnly)
                textColor = QColor(255, 0, 0);
            else if(isHidden)
                textColor = QColor(210, 210, 210);

            QTableWidgetItem *witem = new QTableWidgetItem(folderIcon, QString(dirs[i].name.c_str()));
            witem->setTextColor(textColor);
            mFileListWidget->setItem(current_row, 0, witem);

            witem = new QTableWidgetItem("");
            mFileListWidget->setItem(current_row, 1, witem);

            witem = new QTableWidgetItem(tr("Directory"));
            witem->setTextColor(textColor);
            mFileListWidget->setItem(current_row, 2, witem);

            witem = new QTableWidgetItem(QDateTime::fromTime_t(dirs[i].create_time).toString());
            witem->setTextColor(textColor);
            mFileListWidget->setItem(current_row, 3, witem);

            witem = new QTableWidgetItem(QString("{ ") + QString(dirs[i].uuid.toString().c_str()) + QString(" }"));
            witem->setTextColor(textColor);
            mFileListWidget->setItem(current_row, 4, witem);

            current_row++;

            mCurrentFiles.push_back(dirs[i]);
        }

        for(unsigned int i = 0;i < files.size();i++)
        {
            mFileListWidget->insertRow(current_row);
            mFileListWidget->setRowHeight(current_row, mRowHeight);

            std::string ext_name = files[i].name;

            bool isReadOnly = (files[i].flags & OFS::OFS_READONLY) > 0;
            bool isHidden = (files[i].flags & OFS::OFS_HIDDEN) > 0;

            if(isHidden && (actViewShowHidden->isChecked() == false))
                continue;

            QColor textColor = Qt::black;

            if(isReadOnly && isHidden)
                textColor = QColor(255, 210, 210);
            else if(isReadOnly)
                textColor = QColor(255, 0, 0);
            else if(isHidden)
                textColor = QColor(210, 210, 210);

            QIcon icon = mUnknownFileIcon;
            int ext_pos = ext_name.find_last_of(".");

            if(ext_pos > 0)
            {
                ext_name.erase(0, ext_pos);
                FileIconMap::iterator it = mFileIconMap.find(ext_name);
                if(it == mFileIconMap.end())
                {
                    std::string filename = "./qtOfs_icontest";
                    filename += ext_name;

                    std::fstream stream;
                    stream.open(filename.c_str(), std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
                    stream.close();

                    QFileInfo info(QString(filename.c_str()));
                    icon = mIconProvider.icon(info);
                    if(icon.isNull())
                        icon = mUnknownFileIcon;
                    
                    mFileIconMap.insert(FileIconMap::value_type(ext_name, icon));

                    QFile::remove(QString(filename.c_str()));
                }
                else
                    icon = it->second;
            }

            QTableWidgetItem *witem = new QTableWidgetItem(icon, QString(files[i].name.c_str()));
            witem->setTextColor(textColor);
            mFileListWidget->setItem(current_row, 0, witem);

            unsigned int file_size = files[i].file_size;
            
            QString size_str = QString("%1").arg(file_size);

            int len = size_str.length();
            int runs = (len - 1) / 3;

            for(int j = 0;j < runs;j++)
            {
                size_str.insert(len - (3 * j) - 3, '.');
            }

            witem = new QTableWidgetItem(size_str + QString(" "));
            witem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            witem->setTextColor(textColor);
            mFileListWidget->setItem(current_row, 1, witem);

            witem = new QTableWidgetItem(tr("File"));
            witem->setTextColor(textColor);
            mFileListWidget->setItem(current_row, 2, witem);

            witem = new QTableWidgetItem(QDateTime::fromTime_t(files[i].create_time).toString());
            witem->setTextColor(textColor);
            mFileListWidget->setItem(current_row, 3, witem);

            witem = new QTableWidgetItem(QString("{ ") + QString(files[i].uuid.toString().c_str()) + QString(" }"));
            witem->setTextColor(textColor);
            mFileListWidget->setItem(current_row, 4, witem);

            ++current_row;

            mCurrentFiles.push_back(files[i]);
        }

        if(mCurrentDir.size() > 1)
            --current_row;

        mTotalEntriesLabel->setText(tr("Total Entries : ") + QString("%1").arg(current_row));
        mDirectoryDisplay->setText(QString(mCurrentDir.c_str()));
    }
}
//------------------------------------------------------------------------------------
void MainWindow::dblClick(int row, int column)
{
    if(mOfsFile.valid())
    {
        if(mCurrentFiles[row].flags & OFS::OFS_DIR)
        {
            if(mCurrentFiles[row].name == "..")
            {
                mCurrentDir.erase(mCurrentDir.length() - 1, 1);
                int pos = mCurrentDir.find_last_of("/");
                mCurrentDir.erase(pos + 1, mCurrentDir.length() - pos - 1);
            }
            else
            {
                mCurrentDir = mCurrentDir + mCurrentFiles[row].name + "/";
            }

            showFiles();
        }
    }
}
//------------------------------------------------------------------------------------
void MainWindow::addFiles(QStringList list)
{
    if(!mExtractorThread->isRunning())
    {
        actDefrag->setEnabled(false);
        actClose->setEnabled(false);
        actExtract->setEnabled(false);
        mProgressBar->setValue(0);
        mProgressBar->setVisible(true);
        mProgressLabel->setVisible(true);
        mAddFilesThread->addFiles(mOfsFile, mCurrentDir, list);
        mProgressTimer->start();
    }
}
//------------------------------------------------------------------------------------
void MainWindow::extractFiles(QString path, OFS::FileList list)
{
    if(!mExtractorThread->isRunning())
    {
        actDefrag->setEnabled(false);
        actClose->setEnabled(false);
        actExtract->setEnabled(false);
        mProgressBar->setValue(0);
        mProgressBar->setVisible(true);
        mProgressLabel->setVisible(true);
        mExtractorThread->extract(mOfsFile, mCurrentDir, path, list);
        mProgressTimer->start();
    }
}
//------------------------------------------------------------------------------------
void MainWindow::updateProgress()
{
    if(mExtractorThread->isRunning())
    {
        int pos = 100.0f * mExtractorThread->getCurrentPos();
        mProgressBar->setValue(pos);
        QString msg = QString("   ") + mExtractorThread->getProgressMessage();
        mProgressLabel->setText(msg);
    }
    else if(mAddFilesThread->isRunning())
    {
        int pos = 100.0f * mAddFilesThread->getCurrentPos();
        mProgressBar->setValue(pos);
        QString msg = QString("   ") + mAddFilesThread->getProgressMessage();
        mProgressLabel->setText(msg);
    }
}
//------------------------------------------------------------------------------------
void MainWindow::extractFinished()
{
    actExtract->setEnabled(mOfsFile.valid());
    actClose->setEnabled(mOfsFile.valid());
    actDefrag->setEnabled(mOfsFile.valid());
    mProgressBar->setVisible(false);
    mProgressLabel->setVisible(false);
    mProgressTimer->stop();
}
//------------------------------------------------------------------------------------
void MainWindow::addFilesFinished()
{
    actExtract->setEnabled(mOfsFile.valid());
    actClose->setEnabled(mOfsFile.valid());
    actDefrag->setEnabled(mOfsFile.valid());
    mProgressBar->setVisible(false);
    mProgressLabel->setVisible(false);
    mProgressTimer->stop();

    showFiles();
}
//------------------------------------------------------------------------------------
void MainWindow::fileListItemSelectionChanged()
{
    QList<QTableWidgetItem*> selected = mFileListWidget->selectedItems();

    mSelectedFiles.clear();

    if(selected.size() > 0)
    {
        unsigned char rows[1024];
        memset(rows, 0, 1024);

        for(int i = 0;i < selected.size();i++)
        {
            int val = selected[i]->row();
            if(val < 1024)
                rows[val] = 1;
        }
   
        for(unsigned int i = 0;i < 1024;i++)
        {
            if(rows[i] > 0)
            {
                if(mCurrentFiles[i].name != "..")
                    mSelectedFiles.push_back(mCurrentFiles[i]);
            }
        }
    }

    if(mSelectedFiles.size() == 0)
    {
        actCommandDelete->setEnabled(false);
        actCommandRename->setEnabled(false);
        actCommandReadOnly->setEnabled(false);
        actCommandHidden->setEnabled(false);

        actCommandReadOnly->setChecked(false);
        actCommandHidden->setChecked(false);
    }
    else
    {
        actCommandDelete->setEnabled(true);

        if(mSelectedFiles.size() == 1)
        {
            actCommandRename->setEnabled(true);
            actCommandReadOnly->setEnabled(true);
            actCommandHidden->setEnabled(true);
            actCommandReadOnly->setChecked(mSelectedFiles[0].flags & OFS::OFS_READONLY);
            actCommandHidden->setChecked(mSelectedFiles[0].flags & OFS::OFS_HIDDEN);
        }
        else
        {
            actCommandRename->setEnabled(false);
            actCommandReadOnly->setEnabled(false);
            actCommandHidden->setEnabled(false);
            actCommandReadOnly->setChecked(false);
            actCommandHidden->setChecked(false);
        }
    }
}
//------------------------------------------------------------------------------------
void MainWindow::fileListCustomContextMenuRequested ( const QPoint & pos )
{
    QPoint globalPos = mFileListWidget->mapToGlobal(pos);
    menuCommands->exec( globalPos );
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
void ExtractThread::extract(const OFS::OfsPtr& _ofsFile, const std::string& _currentDir, const QString& _path, const OFS::FileList& _list)
{
    path = _path;
    mlist = _list;
    currentDir = _currentDir;
    ofsFileName = _ofsFile->getFileSystemName();

    start();
}
//------------------------------------------------------------------------------------
void ExtractThread::extractFiles(QString path, const OFS::FileList& list)
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
OFS::ofs64 ExtractThread::generateList(OFS::FileList& list)
{
    unsigned int list_max = list.size();
    OFS::ofs64 file_size = 0;
    
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
void ExtractThread::run()
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
    
    extractFiles(path, mlist);

    ofsFile.unmount();
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
void AddFilesThread::addFiles(const OFS::OfsPtr& _ofsFile, const std::string& _currentDir, const QStringList& _list)
{
    mlist.clear();
    AddFilesData data;
    for(int i = 0;i < _list.size();i++)
    {
        data.fileName = _list.at(i);
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

    // Handle the dropped items
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
                    //its an optimization, thats why we don't have to call Ofs:write after createFile
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
                        QMessageBox::information(QApplication::activeWindow(),"Ofs Exception:", QString(e.getDescription().c_str()), QMessageBox::Ok);
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
                        //createFile accepts initial data to be written during allocation
                        //its an optimization, thats why we don't have to call Ofs::write after createFile
                        if(ofsFile->createFile(fhandle, file_ofs_path.c_str(), stream_size, stream_size, tmp_buffer) != OFS::OFS_OK)
                        {
                            QMessageBox::information(QApplication::activeWindow(), "File Copy Error", tr("File copy failed for: ") + QString(file_ofs_path.c_str()), QMessageBox::Ok);
                            stream.close();
                            continue;
                        }
                    }
                    catch(OFS::Exception& e)
                    {
                        QMessageBox::information(QApplication::activeWindow(), "Ofs Exception:", QString(e.getDescription().c_str()), QMessageBox::Ok);
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
                    QMessageBox::information(QApplication::activeWindow(), "Ofs Exception:", tr("Cannot create directory: ") + QString(dir_ofs_path.c_str()), QMessageBox::Ok);
                    continue;
                }
            }
            catch(OFS::Exception& e)
            {
                QMessageBox::information(QApplication::activeWindow(), "Ofs Exception:", tr("Cannot create directory: ") + QString(dir_ofs_path.c_str()) + QString("\n") + QString(e.getDescription().c_str()), QMessageBox::Ok);
                continue;
            }
        }
    }

    mutex.lock();
    currentPos = 1.0f; 
    mutex.unlock();
}
//------------------------------------------------------------------------------------
OFS::ofs64 AddFilesThread::generateList(AddFilesList& list)
{
    unsigned int list_max = list.size();
    OFS::ofs64 file_size = 0;
    
    AddFilesData data;
    AddFilesList subList;
    std::string tmpSaveCurrentDir;

    for(unsigned int i = 0;i < list_max;i++)
    {
        QFileInfo file(list[i].fileName);

        if(!file.isDir())
        {
            list[i].isDir = false;
            list[i].ofsName = QString(currentDir.c_str()) + file.fileName();
            file_size += file.size();
        }
        else
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

    addFiles(mlist);

    ofsFile.unmount();
}
//------------------------------------------------------------------------------------
