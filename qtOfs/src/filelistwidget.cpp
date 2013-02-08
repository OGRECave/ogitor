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

#include "mainwindow.hxx"
#include "filelistwidget.hxx"

//----------------------------------------------------------------------------------------
FileListWidget::FileListWidget(int rows, int columns, QWidget *parent) :
    QTableWidget(rows, columns, parent)
{
    QStringList headers;
    headers << tr("Name");
    headers << tr("Size");
    headers << tr("Type");
    headers << tr("Date");
    headers << tr("UUID");

    setEditTriggers(QTableWidget::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setHorizontalHeaderLabels(headers);
    horizontalHeader()->setSelectionBehavior(QAbstractItemView::SelectItems);
    verticalHeader()->setVisible(false);
    setShowGrid(false);
    setColumnWidth(0, 450);
    setColumnWidth(1, 80);
    setColumnWidth(2, 120);
    setColumnWidth(3, 120);
    setColumnWidth(4, 200);

    setAcceptDrops(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
}
//----------------------------------------------------------------------------------------
FileListWidget::~FileListWidget()
{
}
//----------------------------------------------------------------------------------------
QStringList FileListWidget::getFilenames(const QMimeData * data)
{
   QStringList result;

   QList<QUrl> urls = data->urls();
   for(int i = 0; i < urls.size(); ++i)
      result.push_back(urls.at(i).toLocalFile());

   return result;
}
//----------------------------------------------------------------------------------
void FileListWidget::mouseReleaseEvent(QMouseEvent *evt)
{
    if(evt->button() == Qt::RightButton)
    {
        //showMenu();
    }
}
//------------------------------------------------------------------------------------
void FileListWidget::dragEnterEvent(QDragEnterEvent *evt)
{
    QWidget *source = evt->source();

    // Get the filenames
    QStringList filenames = getFilenames(evt->mimeData());

    // Don't accept empty drags
    if(filenames.empty() || mOfsMainWindow->isBusy())
    {
        evt->ignore();
        return;
    }

    if(!mOfsMainWindow->actClose->isEnabled())
    {
        QFileInfo file(filenames.at(0));
        QString extension = file.suffix().toLower();

        if(!file.isDir() && extension == "ofs")
            evt->accept();
    }
    else
        evt->accept();
}
//-------------------------------------------------------------------------------------------
void FileListWidget::dragLeaveEvent(QDragLeaveEvent *evt)
{
}
//-------------------------------------------------------------------------------------------
void FileListWidget::dragMoveEvent(QDragMoveEvent *evt)
{
    QWidget *source = evt->source();

    evt->setAccepted(true);
}
//-------------------------------------------------------------------------------------------
void FileListWidget::dropEvent(QDropEvent *evt)
{
    QWidget *source = evt->source();

    // Get the dropped filenames
    QStringList filenames = getFilenames(evt->mimeData());

    if(filenames.empty())
        evt->ignore();

    if(!mOfsMainWindow->actClose->isEnabled())
    {
        QFileInfo file(filenames.at(0));
        QString extension = file.suffix().toLower();

        if(!file.isDir() && extension == "ofs")
        {
            mOfsMainWindow->openOfsFile(file.absoluteFilePath());
            return;
        }
        else
        {
            evt->ignore();
            return;
        }
    }

#if !defined( __WIN32__ ) && !defined( _WIN32 )
    QString msg = "Following files/folders dropped : ";
    for(int i = 0;i < filenames.size();i++)
    {
        msg += filenames.at(i) + QString(";");
    }

    QMessageBox::information(QApplication::activeWindow(),"LINUX DEBUG OUTPUT :", msg, QMessageBox::Ok);
#endif


    mOfsMainWindow->addFiles(filenames);
}
//-------------------------------------------------------------------------------------------
