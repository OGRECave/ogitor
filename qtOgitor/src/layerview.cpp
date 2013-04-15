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
#include "layerview.hxx"

#include "BaseEditor.h"
#include "ViewportEditor.h"
#include "OgitorsSystem.h"
#include "OgitorsRoot.h"
#include "OgitorsUndoManager.h"
#include "OgitorsClipboardManager.h"
#include "MultiSelEditor.h"

using namespace Ogitors;
//----------------------------------------------------------------------------------------
void adjustOgitorLayerParameters()
{
    Ogitors::PROJECTOPTIONS *pOpt = OgitorsRoot::getSingletonPtr()->GetProjectOptions();

    unsigned int vismask = 0x80000000;
    for(unsigned v = 0;v < 31;v++)
    {
        unsigned int curmask = 1 << v;
        if(pOpt->LayerVisible[v])
            vismask |= curmask;
    }

    OgitorsRoot::getSingletonPtr()->GetSceneManager()->setVisibilityMask(vismask);

    PropertyOptionsVector *LayerNames = OgitorsRoot::GetLayerNames();
    
    LayerNames->clear();

    for(unsigned int li = 0;li < (unsigned int)pOpt->LayerCount;li++)
    {
        LayerNames->push_back(PropertyOption(pOpt->LayerNames[li], Ogre::Any(li)));
    }
}
//----------------------------------------------------------------------------------------
LayerTreeWidget::LayerTreeWidget(QWidget *parent) : QTreeWidget(parent) 
{
    mEditedItem = 0;
    mContextLayer = 0;
    setColumnCount(1);
    setHeaderHidden(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setDragDropOverwriteMode(false);
    setDragDropMode(QAbstractItemView::DragDrop);
    setAutoScroll(true);

    connect(itemDelegate(), SIGNAL(closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)), this, SLOT(itemEditDone()));
}
//----------------------------------------------------------------------------------------
void LayerTreeWidget::itemEditDone()
{
    if(mEditedItem)
    {
        int index = indexOfTopLevelItem(mEditedItem);

        OgitorsRoot::getSingletonPtr()->GetProjectOptions()->LayerNames[index] = mEditedItem->text(0).toStdString();
        PropertyOptionsVector *list = OgitorsRoot::GetLayerNames();
        (*list)[index].mKey = mEditedItem->text(0).toStdString();

        mEditedItem = 0;
    }
}
//----------------------------------------------------------------------------------------
void LayerTreeWidget::dragEnterEvent(QDragEnterEvent *evt)
{
    if(evt->source() == this && !OgitorsRoot::getSingletonPtr()->GetSelection()->isEmpty())
    {
        evt->acceptProposedAction();
    }
}
//----------------------------------------------------------------------------------------
void LayerTreeWidget::dragMoveEvent(QDragMoveEvent *evt)
{
    bool allow = false;
    QTreeWidgetItem *item = itemAt(evt->pos());
    
    if(item)
    {
        if(item->parent() == 0)
             allow = true;
    }

    if(allow)
        evt->accept();
    else
        evt->ignore();

    QTreeWidget::dragMoveEvent(evt);
}
//----------------------------------------------------------------------------------------
void LayerTreeWidget::dropEvent(QDropEvent *evt)
{
    bool allow = false;
    CBaseEditor *source = 0;
    QTreeWidgetItem *item = itemAt(evt->pos());
    
    if(!item || !(item->parent() == 0) || !(evt->source() == this))
    {
        evt->ignore();
        return;
    }

    int index = indexOfTopLevelItem(item);
    
    OgitorsUndoManager *undoMgr = OgitorsUndoManager::getSingletonPtr();
    undoMgr->BeginCollection("Drag&Drop layer change");

    QList<QTreeWidgetItem*> list = selectedItems();

    for(int i = 0;i < list.size();i++)
    {
        if(list[i]->parent() == 0)
            continue;

        source = OgitorsRoot::getSingletonPtr()->FindObject(list[i]->text(0).toStdString());
        if(source)
           source->setLayer(index);
    }

    undoMgr->EndCollection(true);
}
//----------------------------------------------------------------------------------------
void LayerTreeWidget::contextMenuEvent(QContextMenuEvent *evt)
{
    if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
        return;

    QTreeWidgetItem *item = itemAt(evt->x(), evt->y());
        
    if(item && item->parent() == 0)
    {
        QMenu* contextMenu = new QMenu(this);

        mContextLayer = item;

        int index = indexOfTopLevelItem(item);
        
        contextMenu->addAction(tr("Toggle Layer Visibility"), this, SLOT(toggleLayer()));
        contextMenu->addSeparator();
        bool addseparator = false;
        if(OgitorsRoot::getSingletonPtr()->GetProjectOptions()->LayerCount < 31)
        {
            contextMenu->addAction(tr("Insert Layer Before"), parent(), SLOT(insertLayerBefore()));
            contextMenu->addAction(tr("Insert Layer After"), parent(), SLOT(insertLayerAfter()));
            addseparator = true;
        }
        if(index)
        {
            contextMenu->addAction(tr("Remove Layer"), parent(), SLOT(removeLayer()));
            addseparator = true;
        }
        if(addseparator)
            contextMenu->addSeparator();

        contextMenu->addAction(tr("Select All Items"), this, SLOT(selectAll()));
        contextMenu->addAction(tr("Lock All Items"), this, SLOT(lockAll()));
        contextMenu->addAction(tr("Unlock All Items"), this, SLOT(unlockAll()));
        contextMenu->exec(QCursor::pos());
        delete contextMenu;

        mContextLayer = 0;

        return;
    }


    CBaseEditor *e = OgitorsRoot::getSingletonPtr()->GetSelection()->getAsSingle();

    if(e != 0 && !OgitorsRoot::getSingletonPtr()->GetSelection()->isEmpty())
    {
        QMenu* contextMenu = new QMenu(this);
        QSignalMapper *signalMapper = 0;
        QSignalMapper *pasteSignalMapper = 0;

        contextMenu->addAction(mOgitorMainWindow->actEditCopy);
        contextMenu->addAction(mOgitorMainWindow->actEditCut);
        contextMenu->addAction(mOgitorMainWindow->actEditPaste);

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
        contextMenu->addAction(mOgitorMainWindow->actEditRename);
        contextMenu->addSeparator();
        contextMenu->addAction(mOgitorMainWindow->actEditCopyToTemplate);
        contextMenu->addAction(mOgitorMainWindow->actEditCopyToTemplateWithChildren);
        
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
void LayerTreeWidget::contextMenu(int id)
{
    if(!OgitorsRoot::getSingletonPtr()->GetSelection()->isEmpty())
        OgitorsRoot::getSingletonPtr()->GetSelection()->getAsSingle()->onObjectContextMenu(id);
}
//----------------------------------------------------------------------------------------
void LayerTreeWidget::toggleLayer()
{
    if(mContextLayer)
    {
        int index = indexOfTopLevelItem(mContextLayer);
        OgitorsRoot::getSingletonPtr()->ToggleLayerVisibility(index);
        bool enbld = OgitorsRoot::getSingletonPtr()->GetProjectOptions()->LayerVisible[index];
        mContextLayer->setTextColor(0, enbld?QColor(0,0,0):QColor(192,192,192));
        mContextLayer->setIcon(0, enbld?QIcon(":/icons/visibility.svg"):QIcon(":/icons/visibility_disabled.svg"));
    }
}
//----------------------------------------------------------------------------------------
void LayerTreeWidget::selectAll()
{
    if(mContextLayer)
    {
        Ogre::StringVector newlist;
        
        for(int i = 0;i < mContextLayer->childCount();i++)
        {
            newlist.push_back(mContextLayer->child(i)->text(0).toStdString());
        }

        OgitorsRoot::getSingletonPtr()->GetSelection()->setSelection(0);
        OgitorsRoot::getSingletonPtr()->GetSelection()->add(newlist);
    }
}
//----------------------------------------------------------------------------------------
void LayerTreeWidget::lockAll()
{
    if(mContextLayer)
    {
        for(int i = 0;i < mContextLayer->childCount();i++)
        {
            Ogre::String name = mContextLayer->child(i)->text(0).toStdString();
            CBaseEditor *object = OgitorsRoot::getSingletonPtr()->FindObject(name);
            if(object)
                object->setLocked(true);
        }
    }
}
//----------------------------------------------------------------------------------------
void LayerTreeWidget::unlockAll()
{
    if(mContextLayer)
    {
        for(int i = 0;i < mContextLayer->childCount();i++)
        {
            Ogre::String name = mContextLayer->child(i)->text(0).toStdString();
            CBaseEditor *object = OgitorsRoot::getSingletonPtr()->FindObject(name);
            if(object)
                object->setLocked(false);
        }
    }
}
//----------------------------------------------------------------------------------------
void LayerTreeWidget::keyPressEvent(QKeyEvent *evt)
{
    CViewportEditor *ovp = OgitorsRoot::getSingletonPtr()->GetViewport();

    if(ovp && (evt->key() == Qt::Key_F))
    {
       ovp->FocusSelectedObject();
    }
    else if(ovp && (evt->key() == Qt::Key_Delete))
    {
        QList<QTreeWidgetItem*> list = selectedItems();

        if(list.size() == 1 && list[0]->parent() == 0)
        {
            static_cast<LayerViewWidget*>(parent())->removeLayer();
            return;
        }

        ovp->DeleteSelectedObject();
    }
    else
        QTreeWidget::keyPressEvent(evt);
}
//----------------------------------------------------------------------------------------
void LayerTreeWidget::mousePressEvent(QMouseEvent *evt)
{
    if(evt->button() == Qt::LeftButton)
    {
        int x = evt->x();
        if(x > 22 && x < 41)
        {
            QTreeWidgetItem *item = itemAt(evt->pos());
            if(item && item->parent() == 0)
            {
                mContextLayer = item;
                toggleLayer();
                mContextLayer = 0;
                return;
            }
        }
    }

    QTreeWidget::mousePressEvent(evt);
}
//----------------------------------------------------------------------------------------
void LayerTreeWidget::mouseDoubleClickEvent(QMouseEvent *evt)
{
    CBaseEditor *target = 0;
    QTreeWidgetItem *item = itemAt(evt->pos());
    
    if(item)
        target = OgitorsRoot::getSingletonPtr()->FindObject(item->text(0).toStdString());
    else
        return;

    if(item->parent() == 0)
    {
        mEditedItem = item;
        editItem(item, 0);
        return;
    }
    
    CViewportEditor *ovp = OgitorsRoot::getSingletonPtr()->GetViewport();

    if(target && ovp && target->supports(CAN_FOCUS))
    {
        OgitorsRoot::getSingletonPtr()->GetSelection()->setSelection(target);
        ovp->FocusSelectedObject();
    }
}
//----------------------------------------------------------------------------------------
LayerViewWidget::LayerViewWidget(QWidget *parent) :
    QWidget(parent)
{
    selectionUpdateInProgress = false;

    mCurrentItemWidget = 0;

    actAddLayer = new QAction(tr("Add New Layer"), this);
    actAddLayer->setStatusTip(tr("Add a New Layer"));
    actAddLayer->setIcon( QIcon( ":/icons/add_layer.svg" ));
    actAddLayer->setEnabled(false);

    actRemoveLayer = new QAction(tr("Remove Layer"), this);
    actRemoveLayer->setStatusTip(tr("Remove Selected Layer"));
    actRemoveLayer->setIcon( QIcon( ":/icons/trash.svg" ));
    actRemoveLayer->setEnabled(false);

    connect(actAddLayer, SIGNAL(triggered()), this, SLOT(addLayer()));
    connect(actRemoveLayer, SIGNAL(triggered()), this, SLOT(removeLayer()));

    treeWidget = new LayerTreeWidget(this);
    toolBar = new QToolBar("", this);
    toolBar->setObjectName(QString::fromUtf8("layerDockWidgettoolBar"));
    toolBar->setIconSize(QSize(20,20));
    toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolBar->addAction(actAddLayer);
    toolBar->addAction(actRemoveLayer);

    QVBoxLayout *boxlayout = new QVBoxLayout(this);
    boxlayout->setMargin(0);
    boxlayout->addWidget(toolBar);
    boxlayout->addWidget(treeWidget);
    boxlayout->setStretch(0,0);
    boxlayout->setStretch(1,1);

    for(int i = 0;i < 31;i++)
        mLayerRootItems[i] = 0;
    
    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
}
//----------------------------------------------------------------------------------------
LayerViewWidget::~LayerViewWidget()
{
}
//----------------------------------------------------------------------------------------
void LayerViewWidget::selectionChanged()
{
    bool enableCopy = false;
    bool enableCopyToTemplate = false;
    bool enablePaste = false;
    bool enableDelete = false;
    bool enableCopyWC = false;

    mCurrentItemWidget = 0;

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
        }
    }
    else
    {
        QList<QTreeWidgetItem*> list = treeWidget->selectedItems();

        CMultiSelEditor *multiSel = OgitorsRoot::getSingletonPtr()->GetSelection();
        
        if(list.size() == 1 && list[0]->parent() == 0)
        {
            if(multiSel && !multiSel->isEmpty())
                multiSel->setSelection(0);

            list[0]->setSelected(true);

            mCurrentItemWidget = list[0];

            int index = treeWidget->indexOfTopLevelItem(mCurrentItemWidget);
            if(index != 0)
                actRemoveLayer->setEnabled(true);
        }
        else if(list.size() > 0 && multiSel)
        {
            NameObjectPairList multiList;

            for(int i = 0;i < list.size();i++)
            {
                if(list[i]->parent() == 0)
                    continue;

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
}
//----------------------------------------------------------------------------------------
void LayerViewWidget::updateLayerNames()
{
    adjustOgitorLayerParameters();

    Ogitors::PROJECTOPTIONS *pOpt = OgitorsRoot::getSingletonPtr()->GetProjectOptions();
    
    for(unsigned int i = 0;i < 31;i++)
    {
        QString name(pOpt->LayerNames[i].c_str());

        QTreeWidgetItem *witem = getRootItem(i);

        witem->setText(0, name);
        if(pOpt->LayerVisible[i])
        {
            witem->setTextColor(0, QColor(0,0,0));
        }
        else
        {
            witem->setTextColor(0, QColor(192,192,192));
            witem->setIcon(0, QIcon(":/icons/visibility_disabled.svg"));
        }

        if(i < (unsigned int)pOpt->LayerCount)
            treeWidget->addTopLevelItem(witem);
    }

    actAddLayer->setEnabled(pOpt->LayerCount < 31);
}
//----------------------------------------------------------------------------------------
void LayerViewWidget::resetLayerNames()
{
    disconnect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

    for(int i = 0;i < 31;i++)
    {
        if(mLayerRootItems[i])
        {
            delete mLayerRootItems[i];
            mLayerRootItems[i] = 0;
        }
    }

    mCurrentItemWidget = 0;

    actAddLayer->setEnabled(false);

    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
}
//----------------------------------------------------------------------------------------
void LayerViewWidget::enableLayer(int layerid, bool enable)
{
    if(mLayerRootItems[layerid])
    {
        mLayerRootItems[layerid]->setTextColor(0, enable?QColor(0,0,0):QColor(128,128,128));
        mLayerRootItems[layerid]->setIcon(0, enable?QIcon(":/icons/visibility.svg"):QIcon(":/icons/visibility_disabled.svg"));
    }
}
//----------------------------------------------------------------------------------------
void LayerViewWidget::addLayer()
{
    Ogitors::PROJECTOPTIONS *pOpt = OgitorsRoot::getSingletonPtr()->GetProjectOptions();

    if(pOpt->LayerCount < 31)
    {
        addLayer(pOpt->LayerCount);
    }
}
//----------------------------------------------------------------------------------------
void LayerViewWidget::removeLayer()
{
    if(mCurrentItemWidget)
    {
        if(removeLayer(treeWidget->indexOfTopLevelItem(mCurrentItemWidget)))
        {
            mCurrentItemWidget = 0;
            actRemoveLayer->setEnabled(false);
            actAddLayer->setEnabled(OgitorsRoot::getSingletonPtr()->GetProjectOptions()->LayerCount < 31);
        }
    }
}
//----------------------------------------------------------------------------------------
bool LayerViewWidget::addLayer(int index)
{
    Ogitors::PROJECTOPTIONS *pOpt = OgitorsRoot::getSingletonPtr()->GetProjectOptions();

    disconnect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

    delete mLayerRootItems[30];
    
    for(int i = 30;i > index;i--)
    {
        mLayerRootItems[i] = mLayerRootItems[i - 1];
        pOpt->LayerNames[i] = pOpt->LayerNames[i - 1];
        pOpt->LayerVisible[i] = pOpt->LayerVisible[i - 1];
    }

    QString name = QString("Layer%1").arg(index);
    mLayerRootItems[index] = new QTreeWidgetItem((QTreeWidget*)0, QStringList(name));
    mLayerRootItems[index]->setIcon(0, QIcon(":/icons/visibility.svg"));
    QFont fnt = mLayerRootItems[index]->font(0);
    fnt.setBold(true);
    mLayerRootItems[index]->setFont(0, fnt);
    mLayerRootItems[index]->setFlags(mLayerRootItems[index]->flags() | Qt::ItemIsEditable);

    treeWidget->insertTopLevelItem(index, mLayerRootItems[index]);

    pOpt->LayerNames[index] = name.toStdString();
    pOpt->LayerVisible[index] = true;
    pOpt->LayerCount++;

    adjustOgitorLayerParameters();

    OgitorsUndoManager::getSingletonPtr()->BeginCollection("NULL");    

    for(int i = pOpt->LayerCount - 1;i > index;i--)
    {
        for(int cn = 0;cn < mLayerRootItems[i]->childCount();cn++)
        {
            Ogre::String objname = mLayerRootItems[i]->child(cn)->text(0).toStdString();
            CBaseEditor *ed = OgitorsRoot::getSingletonPtr()->FindObject(objname);
            if(ed)
            {
                ed->setLayer(i);
            }
        }
    }
    
    OgitorsUndoManager::getSingletonPtr()->EndCollection(false, true);    

    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

    actAddLayer->setEnabled(pOpt->LayerCount < 31);

    return true;
}
//----------------------------------------------------------------------------------------
bool LayerViewWidget::removeLayer(int index)
{
    if(mLayerRootItems[index]->childCount())
    {
        QMessageBox::information(QApplication::activeWindow(),"qtOgitor", tr("Cannot delete layer : Layer is not empty!!"));
        return false;
    }

    Ogitors::PROJECTOPTIONS *pOpt = OgitorsRoot::getSingletonPtr()->GetProjectOptions();

    disconnect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

    delete mLayerRootItems[index];
    
    for(int i = index;i < 30;i++)
    {
        mLayerRootItems[i] = mLayerRootItems[i + 1];
        pOpt->LayerNames[i] = pOpt->LayerNames[i + 1];
        pOpt->LayerVisible[i] = pOpt->LayerVisible[i + 1];
    }

    mLayerRootItems[30] = 0;
    
    pOpt->LayerNames[30] = "Layer30";
    pOpt->LayerVisible[30] = true;
    pOpt->LayerCount--;

    adjustOgitorLayerParameters();

    OgitorsUndoManager::getSingletonPtr()->BeginCollection("NULL");    

    for(int i = pOpt->LayerCount - 1;i >= index;i--)
    {
        for(int cn = 0;cn < mLayerRootItems[i]->childCount();cn++)
        {
            Ogre::String objname = mLayerRootItems[i]->child(cn)->text(0).toStdString();
            CBaseEditor *ed = OgitorsRoot::getSingletonPtr()->FindObject(objname);
            if(ed)
            {
                ed->setLayer(i);
            }
        }
    }
    
    OgitorsUndoManager::getSingletonPtr()->EndCollection(false, true);    

    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

    return true;
}
//----------------------------------------------------------------------------------------
void LayerViewWidget::insertLayerBefore()
{
    if(mCurrentItemWidget)
    {
        addLayer(treeWidget->indexOfTopLevelItem(mCurrentItemWidget));
    }
}
//----------------------------------------------------------------------------------------
void LayerViewWidget::insertLayerAfter()
{
    if(mCurrentItemWidget)
    {
        addLayer(treeWidget->indexOfTopLevelItem(mCurrentItemWidget) + 1);
    }
}
//----------------------------------------------------------------------------------------
QTreeWidgetItem *LayerViewWidget::getRootItem(int index) 
{
    if(index < 31)
    {
        if(mLayerRootItems[index] == 0)
        {
            QString name = QString("Layer%1").arg(index);
            mLayerRootItems[index] = new QTreeWidgetItem((QTreeWidget*)0, QStringList(name));
            mLayerRootItems[index]->setIcon(0, QIcon(":/icons/visibility.svg"));
            QFont fnt = mLayerRootItems[index]->font(0);
            fnt.setBold(true);
            mLayerRootItems[index]->setFont(0, fnt);
            mLayerRootItems[index]->setFlags(mLayerRootItems[index]->flags() | Qt::ItemIsEditable);
        }

        return mLayerRootItems[index]; 
    }
    else
        return 0;
}
