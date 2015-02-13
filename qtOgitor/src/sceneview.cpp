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
#include "entityview.hxx"
#include "sceneview.hxx"

#include "BaseEditor.h"
#include "ViewportEditor.h"
#include "OgitorsSystem.h"
#include "OgitorsRoot.h"
#include "OgitorsUndoManager.h"
#include "OgitorsClipboardManager.h"
#include "MultiSelEditor.h"

#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>

#include <QtWidgets/QVBoxLayout>

using namespace Ogitors;
//----------------------------------------------------------------------------------------
SceneTreeWidget::SceneTreeWidget(QWidget *parent) : ExtendedTreeWidget(parent) 
{
    setColumnCount(1);
    setHeaderHidden(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setDragDropOverwriteMode(false);
    setDragDropMode(QAbstractItemView::DragDrop);
    setAutoScroll(true);
}
//----------------------------------------------------------------------------------------
void SceneTreeWidget::dragEnterEvent(QDragEnterEvent *evt)
{
    if(evt->source() == this || evt->source() == mOgitorMainWindow->getEntityViewWidget()->getListWidget())
    {
        evt->acceptProposedAction();
    }
}
//----------------------------------------------------------------------------------------
void SceneTreeWidget::dragMoveEvent(QDragMoveEvent *evt)
{
    bool allow = false;
    CBaseEditor *target = 0;
    CBaseEditor *source = 0;
    QTreeWidgetItem *item = itemAt(evt->pos());
    
    if(item)
        target = OgitorsRoot::getSingletonPtr()->FindObject(item->text(0).toStdString());

    if(!target || !target->supports(CAN_DROP))
    {
        evt->ignore();
        return;
    }

    if(evt->source() == this)
    {
        if(item)
        {
            allow = true;

            CMultiSelEditor *multiSel = OgitorsRoot::getSingletonPtr()->GetSelection();

            NameObjectPairList selected = multiSel->getModificationList();

            for( NameObjectPairList::iterator it = selected.begin(); it != selected.end(); it++ )
            {
                source = it->second;

                if(source->supports(CAN_DRAG))
                {
                    if(target == source || source->findChild(target->getName(), true))
                        allow = false;
                }
                else
                    allow = false;
            }
        }
    }
    else if(evt->source() == mOgitorMainWindow->getEntityViewWidget()->getListWidget())
        allow = true;

    if(allow)
        evt->accept();
    else
        evt->ignore();

    QTreeWidget::dragMoveEvent(evt);
}
//----------------------------------------------------------------------------------------
void SceneTreeWidget::dropEvent(QDropEvent *evt)
{
    bool allow = false;
    CBaseEditor *target = 0;
    CBaseEditor *source = 0;
    QTreeWidgetItem *item = itemAt(evt->pos());
    
    if(item)
        target = OgitorsRoot::getSingletonPtr()->FindObject(item->text(0).toStdString());

    if(!target || !target->supports(CAN_DROP))
    {
        evt->ignore();
        return;
    }

    if(evt->source() == this)
    {
        OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
        
        CMultiSelEditor *multiSel = OgitorsRoot::getSingletonPtr()->GetSelection();

        if(!multiSel->isSingle())
            undoMgr->BeginCollection("Multiple Drag&Drop");

        NameObjectPairList selected = multiSel->getModificationList();

        for( NameObjectPairList::iterator it = selected.begin(); it != selected.end(); it++ )
        {
            source = it->second;

            if(target != source && source->supports(CAN_DRAG) && !source->findChild(target->getName(), true))
            {
                allow = true;
        
                undoMgr->BeginCollection(source->getName() + "'s parent change");
                Ogre::Vector3 old_pos = source->getDerivedPosition();
                Ogre::Quaternion old_orient = source->getDerivedOrientation();
                Ogre::Vector3 old_scale = source->getDerivedScale();

                source->setParent(target);
                source->setDerivedPosition(old_pos);
                source->setDerivedOrientation(old_orient);
                source->setDerivedScale(old_scale);

                undoMgr->EndCollection(true);
            }
        }

        if(!multiSel->isSingle())
            undoMgr->EndCollection(true);
    }
    else if(evt->source() == mOgitorMainWindow->getEntityViewWidget()->getListWidget())
    {
        QList<QListWidgetItem*> selected = mOgitorMainWindow->getEntityViewWidget()->getListWidget()->selectedItems();
        if(selected.size())
        {
            QString stxt = selected[0]->text();
            OgitorsPropertyValueMap params;
            OgitorsPropertyValue value;

            value.propType = PROP_STRING;
            value.val = Ogre::Any(stxt.toStdString() + ".mesh");

            params["init"] = EMPTY_PROPERTY_VALUE;
            params["meshfile"] = value;
            
            source = OgitorsRoot::getSingletonPtr()->CreateEditorObject(target, "Entity", params, true, true);
            if(source)
            {
                source->load();
                allow = true;
            }
        }
    }

    if(!allow)
        evt->ignore();
}
//----------------------------------------------------------------------------------------
void SceneTreeWidget::contextMenuEvent(QContextMenuEvent *evt)
{
    if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
        return;

    CBaseEditor *e = OgitorsRoot::getSingletonPtr()->GetSelection()->getAsSingle();

    if(e != 0 && !OgitorsRoot::getSingletonPtr()->GetSelection()->isEmpty())
    {
        QMenu* contextMenu = new QMenu(this);
        QSignalMapper *signalMapper = 0;
        QSignalMapper *pasteSignalMapper = 0;

        contextMenu->addAction(mOgitorMainWindow->actEditRename);
        if(e->supports(CAN_FOCUS))
            contextMenu->addAction(mOgitorMainWindow->actFocus);
        contextMenu->addSeparator();
        contextMenu->addAction(mOgitorMainWindow->actEditCopy);
        contextMenu->addAction(mOgitorMainWindow->actEditCut);
        contextMenu->addAction(mOgitorMainWindow->actEditPaste);
        contextMenu->addSeparator();

        QMenu *menuPasteList = 0;
        Ogre::StringVector pastelist;
        pastelist = OgitorsClipboardManager::getSingletonPtr()->getPasteNames(); 
        if(pastelist.size() > 0)
        {
            menuPasteList = contextMenu->addMenu(QIcon(":/icons/editpaste.svg"), tr("Paste From"));
            pasteSignalMapper = new QSignalMapper(this);
            for(unsigned int pl = 0;pl < pastelist.size();pl++)
            {
                QAction *item = menuPasteList->addAction( QString(pastelist[pl].c_str()), pasteSignalMapper, SLOT(map()), 0);
                pasteSignalMapper->setMapping(item, pl);
            }
            connect(pasteSignalMapper, SIGNAL(mapped( int )), this, SLOT(pasteFromClipboardBuffer( int )));
            menuPasteList->setEnabled(mOgitorMainWindow->actEditPaste->isEnabled());
        }

        contextMenu->addAction(mOgitorMainWindow->actEditDelete);
        
        if(mOgitorMainWindow->actEditCopyToTemplate->isEnabled() || 
            mOgitorMainWindow->actEditCopyToTemplateWithChildren->isEnabled())
        {
            contextMenu->addSeparator();
            QMenu* templateMenu = contextMenu->addMenu(tr("Templates"));
            templateMenu->addAction(mOgitorMainWindow->actEditCopyToTemplate);
            templateMenu->addAction(mOgitorMainWindow->actEditCopyToTemplateWithChildren);
        }
        
        UTFStringVector menuList;
        if(e->getObjectContextMenu(menuList))
        {
            mOgitorMainWindow->parseAndAppendContextMenuList(contextMenu, menuList, this);
        }
        contextMenu->exec(QCursor::pos());
        delete contextMenu;
        delete signalMapper;
    }

    evt->accept();
}
//----------------------------------------------------------------------------------------
void SceneTreeWidget::contextMenu(int id)
{
    if(!OgitorsRoot::getSingletonPtr()->GetSelection()->isEmpty())
        OgitorsRoot::getSingletonPtr()->GetSelection()->getAsSingle()->onObjectContextMenu(id);
}
//----------------------------------------------------------------------------------------
void SceneTreeWidget::pasteFromClipboardBuffer(int id)
{
    CBaseEditor *object = OgitorsRoot::getSingletonPtr()->GetSelection()->getFirstObject();
    if(object)
    {
        OgitorsClipboardManager::getSingletonPtr()->paste(object, id);
    }
}
//----------------------------------------------------------------------------------------
void SceneTreeWidget::keyPressEvent(QKeyEvent *evt)
{
    CViewportEditor *ovp = OgitorsRoot::getSingletonPtr()->GetViewport();

    if(ovp && (evt->key() == Qt::Key_F))
    {
       ovp->FocusSelectedObject();
    }
    else if(ovp && (evt->key() == Qt::Key_Delete))
    {
       ovp->DeleteSelectedObject();
    }
    else
        QTreeWidget::keyPressEvent(evt);
}
//----------------------------------------------------------------------------------------
void SceneTreeWidget::itemDoubleClicked(QTreeWidgetItem* item, int column)
{
    CBaseEditor *target = 0;
    
    target = OgitorsRoot::getSingletonPtr()->FindObject(item->text(0).toStdString());

    CViewportEditor *ovp = OgitorsRoot::getSingletonPtr()->GetViewport();

    if(target && ovp && target->supports(CAN_FOCUS))
    {
        OgitorsRoot::getSingletonPtr()->GetSelection()->setSelection(target);
        ovp->FocusSelectedObject();
    }
}
//----------------------------------------------------------------------------------------
SceneViewWidget::SceneViewWidget(QWidget *parent) :
    QWidget(parent)
{
    selectionUpdateInProgress = false;

    treeWidget = new SceneTreeWidget(this);

    QVBoxLayout *boxlayout = new QVBoxLayout(this);
    boxlayout->setMargin(0);
    boxlayout->addWidget(treeWidget);

    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

}
//----------------------------------------------------------------------------------------
SceneViewWidget::~SceneViewWidget()
{
}
//----------------------------------------------------------------------------------------
void SceneViewWidget::setEditRenameState()
{
    bool enableRename = false;
    OgitorsRoot *ogRoot = OgitorsRoot::getSingletonPtr();
    CBaseEditor *curSelection = 0;

    CMultiSelEditor *multiSel = OgitorsRoot::getSingletonPtr()->GetSelection();
        
    if(multiSel)
    {
        enableRename = multiSel->getAsSingle()->getProperty("name")->getDefinition()->canWrite();
    }

    mOgitorMainWindow->actEditRename->setEnabled(enableRename);
}
//----------------------------------------------------------------------------------------
void SceneViewWidget::selectionChanged()
{
    bool enableCopy = false;
    bool enableCopyToTemplate = false;
    bool enablePaste = false;
    bool enableDelete = false;
    bool enableCopyWC = false;
    bool enableRename = false;

    OgitorsRoot *ogRoot = OgitorsRoot::getSingletonPtr();
    CBaseEditor *curSelection = 0;

    if(ogRoot->GetSelection() && !ogRoot->GetSelection()->isEmpty())
        curSelection = ogRoot->GetSelection()->getAsSingle();

    if(selectionUpdateInProgress)
    {
        if(curSelection)
        {
            enableCopy = curSelection->supports(CAN_ACCEPTCOPY);
            enablePaste = curSelection->supports(CAN_ACCEPTPASTE);
            enableDelete = curSelection->supports(CAN_DELETE);
            enableCopyWC = enableCopy && curSelection->isNodeType();

            if(curSelection->getEditorType() == ETYPE_MULTISEL)
                enableCopyToTemplate = true;
            else
                enableCopyToTemplate = enableCopy;

            enableRename = curSelection->getProperty("name")->getDefinition()->canWrite();

            CViewportEditor *vpe = OgitorsRoot::getSingletonPtr()->GetViewport();
            if(!curSelection->isTerrainType() && vpe->GetEditorTool() >= TOOL_DEFORM)
                vpe->SetEditorTool(TOOL_SELECT);
        }
    }
    else
    {
        QList<QTreeWidgetItem*> list = treeWidget->selectedItems();

        CMultiSelEditor *multiSel = OgitorsRoot::getSingletonPtr()->GetSelection();
        
        if(list.size() > 0 && multiSel)
        {
            NameObjectPairList multiList;

            for(int i = 0;i < list.size();i++)
            {
                QString name = list[i]->text(0);
                
                CBaseEditor *obj = ogRoot->FindObject(name.toStdString());

                if(obj)
                    multiList[obj->getName()] = obj;
            }

            multiSel->setSelection(multiList);

            enableCopy = multiSel->getAsSingle()->supports(CAN_ACCEPTCOPY);
            enablePaste = multiSel->getAsSingle()->supports(CAN_ACCEPTPASTE);
            enableDelete = multiSel->getAsSingle()->supports(CAN_DELETE);
            enableCopyWC = enableCopy && multiSel->getAsSingle()->isNodeType();
            if(multiSel->getAsSingle() == multiSel && !multiSel->isEmpty())
                enableCopyToTemplate = true;
            else
                enableCopyToTemplate = enableCopy;

            enableRename = multiSel->getAsSingle()->getProperty("name")->getDefinition()->canWrite();
        }
        else if(multiSel)
            multiSel->setSelection(0);
    }

    mOgitorMainWindow->actEditCopy->setEnabled(enableCopy);
    mOgitorMainWindow->actEditCut->setEnabled(enableCopy && enableDelete);
    mOgitorMainWindow->actEditPaste->setEnabled(enablePaste && OgitorsClipboardManager::getSingletonPtr()->canPaste());
    mOgitorMainWindow->actEditDelete->setEnabled(enableDelete);
    mOgitorMainWindow->actEditCopyToTemplate->setEnabled(enableCopyToTemplate);
    mOgitorMainWindow->actEditCopyToTemplateWithChildren->setEnabled(enableCopyWC);
    mOgitorMainWindow->actEditRename->setEnabled(enableRename);
}
//----------------------------------------------------------------------------------------
