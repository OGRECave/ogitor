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
#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <QtGui/QtGui>
#include <QtCore/QtCore>

#include "ofs.h"

//-----------------------------------------------------------------------------------------

class QProcess;
class FileListWidget;
//-----------------------------------------------------------------------------------------
typedef std::map<std::string, QIcon> FileIconMap;

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
class ExtractThread : public QThread
{
    Q_OBJECT
public:

    void extract(const OFS::OfsPtr& _ofsFile, const std::string& _currentDir, const QString& _path, const OFS::FileList& _list);

    float getCurrentPos()
    {
        mutex.lock();
        float ret = currentPos; 
        mutex.unlock();

        return ret;
    }

    const QString& getProgressMessage()
    {
        QMutexLocker mut(&mutex);
        
        return msgProgress;
    }

private:
    QString path;
    OFS::FileList mlist;
    OFS::OfsPtr ofsFile;
    std::string ofsFileName;
    std::string currentDir;
    float currentPos;
    QMutex mutex;
    unsigned int mTotalFileSize;
    QString msgProgress;

    void run();
    unsigned int generateList(OFS::FileList& list);
    void extractFiles(QString path, const OFS::FileList& list);
};
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
struct AddFilesData
{
    QString fileName;
    QString ofsName;
    bool    isDir;
};

typedef std::vector<AddFilesData> AddFilesList;

class AddFilesThread : public QThread
{
    Q_OBJECT
public:

    void add(const OFS::OfsPtr& _ofsFile, const std::string& _currentDir, const QStringList& _list);

    float getCurrentPos()
    {
        mutex.lock();
        float ret = currentPos; 
        mutex.unlock();

        return ret;
    }

    const QString& getProgressMessage()
    {
        QMutexLocker mut(&mutex);
        
        return msgProgress;
    }

private:
    AddFilesList mlist;
    OFS::OfsPtr ofsFile;
    std::string ofsFileName;
    std::string currentDir;
    float currentPos;
    QMutex mutex;
    unsigned int mTotalFileSize;
    QString msgProgress;

    void run();
    unsigned int generateList(AddFilesList& list);
    void addFiles(const AddFilesList& list);
};
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QMenuBar*       mMenuBar;
    QStatusBar*     mStatusBar;
    QToolBar*       mMainToolBar;
    FileListWidget* mFileListWidget;
    QLabel*         mDirectoryLabel;
    QLineEdit*      mDirectoryDisplay;
    QPushButton*    mDirUpButton;
    QIcon           mUnknownFileIcon;
    QProgressBar*   mProgressBar;
    QLabel*         mProgressLabel;
    QTimer*         mProgressTimer;
    QLabel*         mTotalEntriesLabel;

    QAction*  actNew;
    QAction*  actOpen;
    QAction*  actClose;
    QAction*  actExtract;
    QAction*  actDefrag;
    QAction*  actExit;

    QAction*  actAbout;
    QAction*  actHelp;
    QAction*  actOpenPreferences;

    QAction*  actCommandDelete;
    QAction*  actCommandRename;
    QAction*  actCommandReadOnly;
    QAction*  actCommandHidden;

    QAction*  actViewShowHidden;

    QMenu*    menuFile;
    QMenu*    menuCommands;
    QMenu*    menuView;

    MainWindow(QString args = "", QWidget *parent = 0);
    ~MainWindow();

    void retranslateUi();

    void updateLoadTerminateActions(bool loaded);

    void setApplicationObject(QObject *obj);
    bool getAppActive() { return mAppActive; };
    void addFiles(QStringList list);
    void openOfsFile(QString filename);
    void extractFiles(QString path, OFS::FileList list);
    bool isBusy() 
    {
        return (mExtractorThread->isRunning() || mAddFilesThread->isRunning());
    };

public Q_SLOTS:
    void dblClick(int row, int column);
    void fileListItemSelectionChanged();
    void fileListCustomContextMenuRequested ( const QPoint & pos );
    void showAbout();
    void showHelp();
    void exitApp();
    void newFile();
    void openFile();
    void closeFile();
    void extractTo();
    void defragFile();
    void dirUpOneLevel();
    void openPreferences();
    void commandDelete();
    void commandRename();
    void commandReadOnly();
    void commandHidden();
    void viewShowHidden();
    void extractFinished();
    void addFilesFinished();
    void updateProgress();

private:
    ExtractThread                  *mExtractorThread;
    AddFilesThread                 *mAddFilesThread;
    QFileIconProvider               mIconProvider;

    OFS::OfsPtr                     mOfsFile;
    std::string                     mCurrentDir;
    OFS::FileList                   mCurrentFiles;
    OFS::FileList                   mSelectedFiles;
    QString                         mArgsFile;
    QObject                        *mApplicationObject;
    bool                            mAppActive;
    int                             mRowHeight;

    FileIconMap                     mFileIconMap;

    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj,  QEvent *event);
    bool mHasFileArgs;

    void readSettings(QString filename = "");
    void writeSettings(QString filename = "");
    void createListWidget();
    void createToolbar();
    void addActions();
    void addMenus();
    void setupStatusBar();
    void showFiles();
};

//-----------------------------------------------------------------------------------------

extern MainWindow *mOfsMainWindow;

#endif // MAINWINDOW_HXX

//-----------------------------------------------------------------------------------------
