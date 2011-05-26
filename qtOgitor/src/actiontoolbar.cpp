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

#include "mainwindow.hxx"
#include "actiontoolbar.hxx"

//-------------------------------------------------------------------------------------------
ActionToolbar::ActionToolbar(const QString& name) : QToolBar(name), mCurrentAction(0)
{
    // Enable dropping on the widget
    setAcceptDrops(true);
}
//-------------------------------------------------------------------------------------------
ActionToolbar::~ActionToolbar()
{
}
//-------------------------------------------------------------------------------------------
void ActionToolbar::dragEnterEvent(QDragEnterEvent *evt)
{
   // Get the filenames
   QStringList filenames = getFilenames(evt->mimeData());

   // Don't accept empty drags
   if(filenames.empty())
   {
      evt->ignore();
      return;
   }

   bool ignore_event = true;
   // Accept when only directories and zip-files are being dragged
   for(int i = 0; i < filenames.size(); ++i)
   {
      QFileInfo file(filenames.at(i));
      QString extension = file.suffix().toLower();

      if(extension == "as")
      {
          ignore_event = false;
      }
   }
   if(ignore_event)
       evt->ignore();
   else
       evt->accept();
}
//-------------------------------------------------------------------------------------------
void ActionToolbar::dropEvent(QDropEvent *evt)
{
   // Get the filenames
   QStringList filenames = getFilenames(evt->mimeData());

   // Don't accept empty drags
   if(filenames.empty())
   {
      evt->ignore();
      return;
   }

   bool ignore_event = true;
   // Accept when only directories and zip-files are being dragged
   for(int i = 0; i < filenames.size(); ++i)
   {
      QFileInfo file(filenames.at(i));
      QString extension = file.suffix().toLower();

      if(extension == "as")
      {
          ignore_event = false;
          mOgitorMainWindow->_addScriptAction(":/icons/toolbar.svg", filenames.at(i));
      }
   }
   
   if(ignore_event)
       evt->ignore();
   else
   {
       mOgitorMainWindow->_saveScriptActions();
       evt->accept();
   }
}
//-------------------------------------------------------------------------------------------
QStringList ActionToolbar::getFilenames(const QMimeData * data)
{
   QStringList result;

   QList<QUrl> urls = data->urls();
   for(int i = 0; i < urls.size(); ++i)
      result.push_back(urls.at(i).toLocalFile());

   return result;
}
//----------------------------------------------------------------------------------
void ActionToolbar::contextMenuEvent(QContextMenuEvent *evt)
{
    QMenu* contextMenu = new QMenu(this);

    QAction *act;
    mCurrentAction = actionAt(evt->pos());

    contextMenu->addAction(QIcon(":/icons/additional.svg"), tr("Add Script Action"), mOgitorMainWindow, SLOT(onAddScriptAction()));
    
    act = contextMenu->addAction(QIcon(":/icons/trash.svg"), tr("Remove Script Action"), this, SLOT(onRemoveScriptAction()));
    act->setEnabled(mCurrentAction != 0);

    act = contextMenu->addAction(QIcon(":/icons/editrename.svg"), tr("Edit Script Action"), this, SLOT(onEditScriptAction()));
    act->setEnabled(mCurrentAction != 0);

    contextMenu->exec(QCursor::pos());
    delete contextMenu;

    evt->accept();
}
//----------------------------------------------------------------------------------
void ActionToolbar::onEditScriptAction()
{
    if(mCurrentAction)
    {
        mOgitorMainWindow->_editScriptAction(mCurrentAction);
        mCurrentAction = 0;
    }
}
//----------------------------------------------------------------------------------
void ActionToolbar::onRemoveScriptAction()
{
    if(mCurrentAction)
    {
        mOgitorMainWindow->_removeScriptAction(mCurrentAction);
        mCurrentAction = 0;
    }
}
//----------------------------------------------------------------------------------
