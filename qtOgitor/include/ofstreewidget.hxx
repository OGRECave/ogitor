/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2013 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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

#ifndef OFSTREEWIDGET_HXX
#define OFSTREEWIDGET_HXX

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>
#include <QtCore/QThread>
#include <QtCore/QMutex>

#include "ofs.h"

class ExtractorThread : public QThread
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

    char *tmp_buffer;

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

    /* Flag indicating that this file/folder does exist on the real files system. Therefore
    /* the file system meta data can be sued to determine whether the passed fileName is a
    /* directory or not.
    /* This flag is only to be set to false when a new folder is added, so when no 
    /* import process is executed. In this case, the isDir flag has to be set manually.
    */
    bool    onFS;        
};

typedef std::vector<AddFilesData> AddFilesList;

class AddFilesThread : public QThread
{
    Q_OBJECT
public:

    void addFiles(const OFS::OfsPtr& _ofsFile, const std::string& _currentDir, const QStringList& _list);

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

    char *tmp_buffer;

    void run();
    unsigned int generateList(AddFilesList& list);
    void addFiles(const AddFilesList& list);
};
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
class OfsTreeWidget : public QTreeWidget
{
    Q_OBJECT;
public:

    enum Capabilities
    {
        CAP_SHOW_DIRS        = 0,
        CAP_SHOW_FILES       = 1,
        CAP_ALLOW_DROPS      = 2,
        CAP_SHOW_COLORS      = 4,
        CAP_SHOW_RECYCLEBIN  = 8,
        CAP_FULL_FUNCTIONS   = 0xFFFF
    };

    OfsTreeWidget(QWidget *parent = 0, unsigned int capabilities = CAP_SHOW_DIRS, QStringList initialSelection = QStringList("/"));
    virtual ~OfsTreeWidget();

    const QStringList& getSelectedItems() { return mSelectedItems; }
    void refreshWidget();
    void extractFiles();
    void addFiles(QString rootDir, QStringList list);

    static void triggerCallback(void* userData, OFS::_OfsBase::OfsEntryDesc* arg1, const char* arg2);

public Q_SLOTS:
    void onSelectionChanged();
    void onItemCollapsed(QTreeWidgetItem * item);
    void onItemExpanded(QTreeWidgetItem * item);
    void threadFinished();

Q_SIGNALS:
    void busyState(bool state);

protected:
    typedef std::map<std::string, QTreeWidgetItem*> NameTreeWidgetMap;

    OFS::OfsPtr       mFile;
    QStringList       mSelectedItems;
    NameTreeWidgetMap mItemMap;
    unsigned int      mCapabilities;
    QIcon             mUnknownFileIcon;
    AddFilesThread   *mAddFilesThread;
    ExtractorThread  *mExtractorThread;
    QTreeWidgetItem  *mRecycleBinParent;

    void dragEnterEvent(QDragEnterEvent *evt);
    void dragMoveEvent(QDragMoveEvent *evt);
    void dropEvent(QDropEvent *evt);
    
    void fillTree(QTreeWidgetItem *pItem, std::string path);
    void fillTreeFiles(QTreeWidgetItem *pItem, std::string path);
    void fillRecycleBin(QTreeWidgetItem *pItem);
    QStringList getFilenames(const QMimeData * data);
};

#endif // OFSTREEWIDGET_HXX
