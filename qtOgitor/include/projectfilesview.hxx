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

#ifndef PROJECTFILESVIEW_HXX
#define PROJECTFILESVIEW_HXX

//------------------------------------------------------------------------------

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QAction>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QToolBar>

class OfsTreeWidget;

//------------------------------------------------------------------------------
struct selectStats
{
        bool mActMakeAssetEnabled;
        bool mActMakeAssetChecked;
        bool mActReadOnlyEnabled;
        bool mActReadOnlyChecked;
        bool mActHiddenEnabled;
        bool mActHiddenChecked;
        bool mActDeleteEnabled;
        bool mActLinkFileSystemEnabled;
        bool mActImportFileEnabled;
        bool mActImportFolderEnabled;
        bool mActAddEmptyFolderEnabled;
        bool mActEmptyRecycleBinEnabled;
        bool mActRestoreFromRecycleBinEnabled;
};

class ProjectFilesViewWidget : public QWidget
{
    Q_OBJECT;
public:
    explicit ProjectFilesViewWidget(QWidget *parent = 0);
    virtual ~ProjectFilesViewWidget();
    void prepareView();
    void clearView();
    void triggerRefresh();
	bool isFocusTarget();

public Q_SLOTS:
    void itemDoubleClicked(QTreeWidgetItem* item, int column);
    void onOfsWidgetCustomContextMenuRequested(const QPoint &pos);
    void onOfsWidgetBusyState(bool state);

    void onRefresh();
    void onExtract();
    void onDefrag();
    void onDelete();
    void onRename();
    void onReadOnly();
    void onHidden();
    void onImportFolder();
    void onImportFile();
    void onMakeAsset();
    void onAddFolder();
    void onLinkFileSystem();
	void onUnlinkFileSystem( const QString & text );
	void onSelectionChanged();
    void onEmptyRecycleBin();
    void onRestoreFromRecycleBin();

Q_SIGNALS:
    void needUpdate();

protected:
    std::string     mAddFileFolderPath;
    OfsTreeWidget*  mOfsTreeWidget;
    QVBoxLayout*    mVBoxLayout;
    QToolBar*       mToolBar;

    QMenu*          mMenu;
    QMenu*          mUnlinkFileSystem;
    
    QAction*        mActRefresh;
    QAction*        mActImportFolder;
    QAction*        mActImportFile;
    QAction*        mActAddFolder;
    QAction*        mActMakeAsset;
    QAction*        mActExtract;
    QAction*        mActDefrag;
    QAction*        mActDelete;
    QAction*        mActReadOnly;
    QAction*        mActHidden;
    QAction*        mActLinkFileSystem;
    QAction*        mActEmptyRecycleBin;
    QAction*        mActRestoreFromRecycleBin;

    void modifyStats( selectStats& stats, QTreeWidgetItem* item);
};

//------------------------------------------------------------------------------

#endif // PROJECTFILESVIEW_HXX

//------------------------------------------------------------------------------
