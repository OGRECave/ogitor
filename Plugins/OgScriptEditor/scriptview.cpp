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

#include <QtCore/QSignalMapper>
#include <QtGui/QtGui>
#include <QtGui/QVBoxLayout>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtCore/QTextCodec>

#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "DefaultEvents.h"
#include "EventManager.h"
#include "ofs.h"

#include "scriptview.hxx"
#include "scripttexteditorcodec.hxx"

using namespace Ogitors;

ScriptViewWidget *mScriptViewWidget = 0;

//----------------------------------------------------------------------------------------
QString ConvertToQString(Ogre::UTFString& value)
{
    QByteArray encodedString((const char*)value.data(), value.size() * 2);
    QTextCodec *codec = QTextCodec::codecForName("UTF-16");
    return codec->toUnicode(encodedString);
}
//----------------------------------------------------------------------------------------
ScriptTreeWidget::ScriptTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
    setColumnCount(1);
    setHeaderHidden(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setDragDropMode(QAbstractItemView::NoDragDrop);
}
//----------------------------------------------------------------------------------------
ScriptTreeWidget::~ScriptTreeWidget()
{
}
//----------------------------------------------------------------------------------------
void ScriptTreeWidget::contextMenuEvent(QContextMenuEvent *event)
{
    event->ignore();
}
//----------------------------------------------------------------------------------------
void ScriptTreeWidget::contextMenu(int id)
{
}
//----------------------------------------------------------------------------------------
void ScriptTreeWidget::keyPressEvent(QKeyEvent *event)
{
    QTreeWidget::keyPressEvent(event);
}
//----------------------------------------------------------------------------------------
void ScriptTreeWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QList<QTreeWidgetItem*> list = selectedItems();

    if(list.size() == 0)
        return;

    QTreeWidgetItem *item = list[0];

    if(item->parent() == 0)
        return;

    while(item->parent() != 0 && item->parent()->parent() != 0)
        item = item->parent();

    Ogre::String scriptname = item->text(0).toStdString();
    Ogre::String scriptfilename = item->whatsThis(0).toStdString();

    GenericTextEditor::getSingletonPtr()->displayTextFromFile(scriptfilename.c_str());
}
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
ScriptViewWidget::ScriptViewWidget(QWidget *parent) : QWidget(parent)
{
    treeWidget = new ScriptTreeWidget(this);

    QVBoxLayout *boxlayout = new QVBoxLayout(this);
    boxlayout->setMargin(0);
    boxlayout->addWidget(treeWidget);

    generalCategory = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("General Scripts")));
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    QString iconpath(OgitorsUtils::QualifyPath("/usr/share/qtOgitor/Plugins/Icons/project.svg").c_str());
#else
    QString iconpath(OgitorsUtils::QualifyPath("../Plugins/Icons/project.svg").c_str());
#endif
    generalCategory->setIcon(0, QIcon(iconpath));
    QFont fnt = generalCategory->font(0);
    fnt.setBold(true);
    generalCategory->setFont(0, fnt);

    treeWidget->addTopLevelItem(generalCategory);

    projectCategory = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("Project Scripts")));
    projectCategory->setIcon(0, QIcon(iconpath));
    projectCategory->setFont(0, fnt);

    treeWidget->addTopLevelItem(projectCategory);

#if defined(Q_WS_X11)
    Ogre::String filefilter = OgitorsUtils::QualifyPath("/usr/share/qtOgitor/Scripts/*.as");
#else
    Ogre::String filefilter = OgitorsUtils::QualifyPath("../Scripts/*.as");
#endif

    QTreeWidgetItem* scriptitem = 0;
    Ogre::StringVector list;
    OgitorsSystem::getSingletonPtr()->GetFileList(filefilter, list);

    for(unsigned int i = 0;i < list.size();i++)
    {
        Ogre::String filename = list[i];
        Ogre::String scriptname = OgitorsUtils::ExtractFileName(list[i]);
        scriptitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(scriptname.c_str())));
        scriptitem->setWhatsThis(0, QString(filename.c_str()));
        scriptitem->setIcon(0, QIcon(":/icons/script2.svg"));
        generalCategory->addChild(scriptitem);
    }

    Ogitors::EventManager::getSingletonPtr()->connectEvent(EventManager::LOAD_STATE_CHANGE, this, true, 0, true, 0, EVENT_CALLBACK(ScriptViewWidget, onSceneLoadStateChange));
}
//----------------------------------------------------------------------------------------
ScriptViewWidget::~ScriptViewWidget()
{
    treeWidget->clear();
    Ogitors::EventManager::getSingletonPtr()->disconnectEvent(EventManager::LOAD_STATE_CHANGE, this);
}
//----------------------------------------------------------------------------------------
void ScriptViewWidget::destroyScene()
{
    while(projectCategory->childCount() > 0)
        delete projectCategory->child(0);
}
//----------------------------------------------------------------------------------------
void ScriptViewWidget::prepareView()
{
    PropertyOptionsVector *scripts = OgitorsRoot::getSingletonPtr()->GetScriptNames();

    QTreeWidgetItem* scriptitem = 0;

    for(unsigned int i = 1;i < (*scripts).size();i++)
    {
        Ogre::String shortname = (*scripts)[i].mKey;
        Ogre::String longname = Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectFile()->getFileSystemName();
        longname += "::/Scripts/";
        longname += (*scripts)[i].mKey;
        scriptitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(shortname.c_str())));
        scriptitem->setWhatsThis(0, QString(longname.c_str()));
        scriptitem->setIcon(0, QIcon(":/icons/script.svg"));
        projectCategory->addChild(scriptitem);
    }
}
//----------------------------------------------------------------------------------------
void ScriptViewWidget::onSceneLoadStateChange(Ogitors::IEvent* evt)
{
    Ogitors::LoadStateChangeEvent *change_event = Ogitors::event_cast<Ogitors::LoadStateChangeEvent*>(evt);

    if(change_event)
    {
        //reload the zone selection widget when a scene is loaded
        LoadState state = change_event->getType();

        if(state == LS_LOADED)
            prepareView();
        else if(state == LS_UNLOADED)
        {
            destroyScene();
        }
    }
}
//----------------------------------------------------------------------------------------