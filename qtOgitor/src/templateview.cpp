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

#include <QtCore/QSignalMapper>

#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QContextMenuEvent>

#include <QtWidgets/QMenu>
#include <QtWidgets/QVBoxLayout>

#include "templateview.hxx"
#include "BaseEditor.h"
#include "ViewportEditor.h"
#include "OgitorsSystem.h"
#include "OgitorsUndoManager.h"
#include "OgitorsClipboardManager.h"
#include "NodeEditor.h"
#include "EntityEditor.h"
#include "MultiSelEditor.h"
#include "entityview.hxx"

extern QString ConvertToQString(Ogre::UTFString& value);
extern bool ViewKeyboard[1024];

using namespace Ogitors;

//----------------------------------------------------------------------------------------
TemplateTreeWidget::TemplateTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
    setColumnCount(1);
    setHeaderHidden(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setDragDropMode(QAbstractItemView::DragOnly);
}
//----------------------------------------------------------------------------------------
void TemplateTreeWidget::contextMenuEvent(QContextMenuEvent *evt)
{
    QList<QTreeWidgetItem*> list = selectedItems();
    if(list.size() == 0 || list[0]->parent() == 0)
        return;

    QMenu* contextMenu = new QMenu(this);
    QAction *action = contextMenu->addAction(tr("Copy"), this, SLOT(copyTemplate()));
    action->setIcon(QIcon(":/icons/editcopy.svg"));
    contextMenu->exec(QCursor::pos());
    delete contextMenu;

    evt->accept();
}
//----------------------------------------------------------------------------------------
void TemplateTreeWidget::copyTemplate()
{
    /*QList<QTreeWidgetItem*> list = selectedItems();
    if(list.size() == 0 || list[0]->parent() == 0)
    return;

    OGITORUNDOSTRUCT *templdata = OgitorsUndoManager::getSingletonPtr()->GetTemplateData(list[0]->text(0).toStdString());
    if(!templdata)
    return;

    OgitorsUndoManager::getSingletonPtr()->Copy(templdata);*/
}
//----------------------------------------------------------------------------------------
void TemplateTreeWidget::keyPressEvent(QKeyEvent *evt)
{
    QTreeWidget::keyPressEvent(evt);
}
//----------------------------------------------------------------------------------------
TemplateViewWidget::TemplateViewWidget(QWidget *parent) : QWidget(parent),
mCurrentTemplate(""), mDragObject(0)
{
    treeWidget = new TemplateTreeWidget(this);

    QVBoxLayout *boxlayout = new QVBoxLayout(this);
    boxlayout->setMargin(0);
    boxlayout->addWidget(treeWidget);

    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

    OgitorsRoot::getSingletonPtr()->RegisterDragDropHandler((void*)treeWidget, this);
}
//----------------------------------------------------------------------------------------
TemplateViewWidget::~TemplateViewWidget()
{
    OgitorsRoot::getSingletonPtr()->UnRegisterDragDropHandler((void*)treeWidget, this);
    destroyScene();
}
//----------------------------------------------------------------------------------------
void TemplateViewWidget::destroyScene()
{
    treeWidget->clear();
}
//----------------------------------------------------------------------------------------
void TemplateViewWidget::selectionChanged()
{
    QList<QTreeWidgetItem*> list = treeWidget->selectedItems();
    if(list.size() == 0 || list[0]->parent() == 0)
    {
        mCurrentTemplate = "";
        return;
    }

    mCurrentTemplate = list[0]->text(0).toStdString();
}
//----------------------------------------------------------------------------------------
void TemplateViewWidget::prepareView()
{
    treeWidget->clear();

    Ogre::String dumtitle = "";
    QTreeWidgetItem* rootitem = 0;
    QTreeWidgetItem* tmplitem = 0;

    ObjectTemplateMap::const_iterator it;
    const ObjectTemplateMap& generalTemplates = OgitorsClipboardManager::getSingletonPtr()->getGeneralTemplates();

    rootitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("General Templates")));
    QString iconpath(std::string(Ogitors::Globals::OGITOR_PLUGIN_ICON_PATH + "/project.svg").c_str());
    rootitem->setIcon(0, QIcon(iconpath));
    QFont fnt = rootitem->font(0);
    fnt.setBold(true);
    rootitem->setFont(0, fnt);

    it = generalTemplates.begin();
    while(it != generalTemplates.end())
    {
        tmplitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(it->first.c_str()));
        tmplitem->setIcon(0, QIcon(":/icons/template.svg"));
        rootitem->addChild(tmplitem);
        it++;
    }

    treeWidget->insertTopLevelItem(0, rootitem);

    rootitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("Project Templates")));
    rootitem->setIcon(0, QIcon(iconpath));
    rootitem->setFont(0, fnt);

    const ObjectTemplateMap& projectTemplates = OgitorsClipboardManager::getSingletonPtr()->getProjectTemplates();

    it = projectTemplates.begin();
    while(it != projectTemplates.end())
    {
        tmplitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(it->first.c_str()));
        tmplitem->setIcon(0, QIcon(":/icons/template.svg"));
        rootitem->addChild(tmplitem);
        it++;
    }

    treeWidget->insertTopLevelItem(1, rootitem);
}
//----------------------------------------------------------------------------------------
bool TemplateViewWidget::OnDragEnter()
{
    if(!OgitorsClipboardManager::getSingletonPtr()->isTemplateInstantiable(mCurrentTemplate))
        return false;

    if(OgitorsClipboardManager::getSingletonPtr()->doesTemplateRequirePlacement(mCurrentTemplate))
    {
        mDragObject = OgitorsClipboardManager::getSingletonPtr()->instantiateTemplate(mCurrentTemplate);

        if(!mDragObject)
            return false;
    }

    return true;
}
//----------------------------------------------------------------------------------------
void TemplateViewWidget::OnDragLeave()
{
    if(mDragObject)
        OgitorsRoot::getSingletonPtr()->DestroyEditorObject(mDragObject, true, true);

    mDragObject = 0;
}
//----------------------------------------------------------------------------------------
bool TemplateViewWidget::OnDragMove(Ogre::Viewport *vp, unsigned int modifier, Ogre::Vector2& position)
{
    if(!mDragObject)
        return true;

    Ogre::Ray mouseRay;
    mouseRay = vp->getCamera()->getCameraToViewportRay(position.x, position.y);

    Ogre::StringVector dragnamelist;
    dragnamelist.clear();
    mDragObject->getNameList(dragnamelist);

    Ogre::Vector3 vPos;
    mDragObject->getProperties()->getValue("position", vPos);

    if(vPos.x == 999999 && vPos.y == 999999 && vPos.z == 999999)
        vPos = mouseRay.getOrigin() + (mouseRay.getDirection() * 40.0f);
    else
        vPos = OgitorsRoot::getSingletonPtr()->GetGizmoIntersectCameraPlane(mDragObject, mouseRay);

    if((modifier & Ogitors::DragDropControlModifier) || OgitorsRoot::getSingletonPtr()->GetSnapGroundState())
    {
        bool hitfound = OgitorsRoot::getSingletonPtr()->GetViewport()->GetHitPosition(mouseRay, vPos, dragnamelist);

        if(!hitfound)
        {
            mDragObject->getProperties()->getValue("position", vPos);
            float distance = (vPos - mouseRay.getOrigin()).length();
        }
    }
    mDragObject->getProperties()->setValue("position", vPos);

    return true;
}
//----------------------------------------------------------------------------------------
void TemplateViewWidget::OnDragWheel(Ogre::Viewport *vp, float delta)
{
    if(!mDragObject)
        return;

    Ogre::Vector3 vPos;
    mDragObject->getProperties()->getValue("position", vPos);
    float distance = (vPos - vp->getCamera()->getDerivedPosition()).length() + (delta / 120.0f);

    if(vPos.x == 999999 && vPos.y == 999999 && vPos.z == 999999)
        return;
    else
        vPos = vp->getCamera()->getDerivedPosition() + ((vPos - vp->getCamera()->getDerivedPosition()).normalisedCopy() * distance);

    mDragObject->getProperties()->setValue("position", vPos);
}
//----------------------------------------------------------------------------------------
void TemplateViewWidget::OnDragDropped(Ogre::Viewport *vp, Ogre::Vector2& position)
{
    if(mDragObject)
    {
        OgitorsRoot::getSingletonPtr()->GetSelection()->setSelection(mDragObject);
    }
    else
    {
        CBaseEditor *object = OgitorsClipboardManager::getSingletonPtr()->instantiateTemplate(mCurrentTemplate);
    }

    mDragObject = 0;
}
//----------------------------------------------------------------------------------------
