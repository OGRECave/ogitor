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
#include "BaseEditor.h"
#include "MaterialEditor.h"
#include "TechniqueEditor.h"
#include "MultiSelEditor.h"
#include "DefaultEvents.h"
#include "EventManager.h"

#include "materialview.hxx"
#include "materialtexteditorcodec.hxx"

using namespace Ogitors;

MaterialViewWidget *mMaterialViewWidget = 0;

//----------------------------------------------------------------------------------------
QString ConvertToQString(Ogre::UTFString& value)
{
    QByteArray encodedString((const char*)value.data(), value.size() * 2);
    QTextCodec *codec = QTextCodec::codecForName("UTF-16");
    return codec->toUnicode(encodedString);
}
//----------------------------------------------------------------------------------------
MaterialTreeWidget::MaterialTreeWidget(QWidget *parent) : QTreeWidget(parent),
mMaterialEditor(0)
{
    setColumnCount(1);
    setHeaderHidden(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setDragDropMode(QAbstractItemView::DragOnly);
    OgitorsRoot::getSingletonPtr()->RegisterDragDropHandler(this, this);
}
//----------------------------------------------------------------------------------------
MaterialTreeWidget::~MaterialTreeWidget()
{
    OgitorsRoot::getSingletonPtr()->UnRegisterDragDropHandler(this, this);
}
//----------------------------------------------------------------------------------------
void MaterialTreeWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QList<QTreeWidgetItem*> list = selectedItems();
    if(list.size() == 0)
        return;

    if(list[0]->parent() == 0)
        return;

    if(mMaterialEditor != 0)
    {
        UTFStringVector menuList;
        if(mMaterialEditor->getObjectContextMenu(menuList))
        {
            QMenu* contextMenu = new QMenu(this);
            QSignalMapper *signalMapper = new QSignalMapper(this);

            UTFStringVector vList;
            int counter = 0;

            for(unsigned int i = 0;i < menuList.size();i++)
            {
                if(i == 0)
                    contextMenu->addSeparator();

                OgitorsUtils::ParseUTFStringVector(menuList[i], vList);
                if(vList.size() > 0 && vList[0] != "")
                {
                    QAction *item = contextMenu->addAction(ConvertToQString(vList[0]), signalMapper, SLOT(map()), 0);
                    if(vList.size() > 1)
                        item->setIcon(QIcon(ConvertToQString(vList[1])));
                    signalMapper->setMapping(item, i);
                    counter++;
                }
            }
            if(counter)
            {
                connect(signalMapper, SIGNAL(mapped( int )), this, SLOT(contextMenu( int )));
                contextMenu->exec(QCursor::pos());
            }
            delete contextMenu;
            delete signalMapper;
        }
    }

    event->accept();
}
//----------------------------------------------------------------------------------------
void MaterialTreeWidget::contextMenu(int id)
{
    if(mMaterialEditor)
    {
        mMaterialEditor->onObjectContextMenu(id);
        QTreeWidgetItem *item = static_cast<QTreeWidgetItem*>(mMaterialEditor->getSceneTreeItemHandle());
        while(item->childCount() > 0)
            item->removeChild(item->child(0));

        Ogre::Material* material = static_cast<Ogre::Material*>(mMaterialEditor->getHandle());
        Ogre::String dumtitle;
        QTreeWidgetItem *techitem = 0, *passitem = 0;

        for(unsigned int t = 0;t < material->getNumTechniques();t++)
        {
            Ogre::Technique *tech = material->getTechnique(t);
            dumtitle = "Technique " + Ogre::StringConverter::toString(t) + ": (" + tech->getName() + ")";
            techitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(dumtitle.c_str())));
            techitem->setIcon(0, QIcon(":/icons/technique.svg"));
            techitem->setWhatsThis(0, QString("%1").arg(t << 8));
            item->addChild(techitem);

            for(unsigned int p = 0;p < tech->getNumPasses();p++)
            {
                Ogre::Pass *pass = tech->getPass(p);
                dumtitle = "Pass " + Ogre::StringConverter::toString(p) + ": (" + pass->getName() + ")";
                passitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(dumtitle.c_str())));
                passitem->setIcon(0, QIcon(":/icons/pass.svg"));
                passitem->setWhatsThis(0, QString("%1").arg((t << 8) + p));
                techitem->addChild(passitem);
            }
        }
    }
}
//----------------------------------------------------------------------------------------
void MaterialTreeWidget::keyPressEvent(QKeyEvent *event)
{
    QTreeWidget::keyPressEvent(event);
}
//----------------------------------------------------------------------------------------
void MaterialTreeWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
        return;

    Ogre::Material* material = static_cast<Ogre::Material*>(mMaterialEditor->getHandle());
    try
    {
        std::string resourceGroup(Ogre::ResourceGroupManager::getSingletonPtr()->findGroupContainingResource(material->getOrigin()));
        Ogre::FileInfoListPtr fileInfoList(Ogre::ResourceGroupManager::getSingletonPtr()->findResourceFileInfo(resourceGroup, material->getOrigin()));
        Ogre::String materialPath = fileInfoList.getPointer()->at(0).filename;

        if(fileInfoList.getPointer()->at(0).archive->getType() == "FileSystem")
            materialPath = fileInfoList.getPointer()->at(0).archive->getName() + "/" + materialPath;
        else if(fileInfoList.getPointer()->at(0).archive->getType() == "Ofs")
        {
            Ogre::String fsName = fileInfoList.getPointer()->at(0).archive->getName();
            int pos = fsName.find("::");
            fsName.erase(0, pos + 2);
            materialPath = fsName + materialPath;
        }

        mMaterialEditor->setMaterialPath(materialPath);

        Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(fileInfoList.getPointer()->at(0).filename, resourceGroup);
        Ogre::String fullPath = fileInfoList.getPointer()->at(0).archive->getName() +  fileInfoList.getPointer()->at(0).filename;
        GenericTextEditor::getSingletonPtr()->displayTextFromFile(fullPath.c_str(), material->getName().c_str());
    }
    catch(Ogre::Exception e)
    {
        QMessageBox::information(QApplication::activeWindow(), "qtOgitor", "This is only an internal, temporary material and therefore there is no displayable code available.");
    }
}
//----------------------------------------------------------------------------------------
bool MaterialTreeWidget::OnDragEnter()
{
    return true;
}
//----------------------------------------------------------------------------------------
void MaterialTreeWidget::OnDragLeave()
{
}
//----------------------------------------------------------------------------------------
bool MaterialTreeWidget::OnDragMove(Ogre::Viewport *vp, unsigned int modifier, Ogre::Vector2& position)
{
    Ogre::Ray mouseRay;
    mouseRay = vp->getCamera()->getCameraToViewportRay(position.x, position.y);

    Ogre::Entity *result;
    Ogre::Vector3 hitlocation;
    CBaseEditor *object = 0;
    if(OgitorsUtils::PickEntity(mouseRay,&result,hitlocation)&&result->getName() != "HydraxMeshEnt")
    {
        Ogre::String sName = result->getName();
        object = OgitorsRoot::getSingletonPtr()->FindObject(sName);
    }

    if(object && object->supports(CAN_USEMATERIAL))
        return true;
    else
        return false;
    return false;
}
//----------------------------------------------------------------------------------------
void MaterialTreeWidget::OnDragDropped(Ogre::Viewport *vp, Ogre::Vector2& position)
{
    Ogre::Ray mouseRay;
    mouseRay = vp->getCamera()->getCameraToViewportRay(position.x, position.y);

    Ogre::Entity *result;
    Ogre::Vector3 hitlocation;
    CBaseEditor *object = 0;
    if(OgitorsUtils::PickEntity(mouseRay,&result,hitlocation)&&result->getName() != "HydraxMeshEnt")
    {
        Ogre::String sName = result->getName();
        object = OgitorsRoot::getSingletonPtr()->FindObject(sName);
    }

    if(object && object->supports(CAN_USEMATERIAL))
        object->onDropMaterial(mouseRay, hitlocation, mMaterialEditor->getName());
}
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
MaterialViewWidget::MaterialViewWidget(QWidget *parent) : QWidget(parent), mMaterialEditor(0),
mTechniqueEditor(0)
{
    treeWidget = new MaterialTreeWidget(this);

    QVBoxLayout *boxlayout = new QVBoxLayout(this);
    boxlayout->setMargin(0);
    boxlayout->addWidget(treeWidget);

    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

    Ogitors::EventManager::getSingletonPtr()->connectEvent(EventManager::LOAD_STATE_CHANGE, this, true, 0, true, 0, EVENT_CALLBACK(MaterialViewWidget, onSceneLoadStateChange));
    Ogitors::EventManager::getSingletonPtr()->connectEvent(EventManager::GLOBAL_PREPARE_VIEW, this, true, 0, true, 0, EVENT_CALLBACK(MaterialViewWidget, onGlobalPrepareView));
}
//----------------------------------------------------------------------------------------
MaterialViewWidget::~MaterialViewWidget()
{
    destroyScene();
    Ogitors::EventManager::getSingletonPtr()->disconnectEvent(EventManager::LOAD_STATE_CHANGE, this);
    Ogitors::EventManager::getSingletonPtr()->disconnectEvent(EventManager::GLOBAL_PREPARE_VIEW, this);
}
//----------------------------------------------------------------------------------------
void MaterialViewWidget::destroyScene()
{
    if(mMaterialEditor)
        mMaterialEditor->destroy();
    mMaterialEditor = 0;

    if(mTechniqueEditor)
        mTechniqueEditor->destroy();
    mTechniqueEditor = 0;

    treeWidget->clear();
}
//----------------------------------------------------------------------------------------
void MaterialViewWidget::selectionChanged()
{
    QList<QTreeWidgetItem*> list = treeWidget->selectedItems();
    if(list.size() == 0)
    {
        if(OgitorsRoot::getSingletonPtr()->GetSelection())
            OgitorsRoot::getSingletonPtr()->GetSelection()->setSelection(0);
        return;
    }

    if(list[0]->parent() == 0)
    {
        if(OgitorsRoot::getSingletonPtr()->GetSelection())
            OgitorsRoot::getSingletonPtr()->GetSelection()->setSelection(0);
        return;
    }

    QTreeWidgetItem *selitem = list[0];
    int level = 0;
    int ID = selitem->whatsThis(0).toInt();

    while(selitem->parent()->parent() != 0)
    {
        selitem = selitem->parent();
        ++level;
    }

    QString name = selitem->text(0);

    OgitorsRoot *ogRoot = OgitorsRoot::getSingletonPtr();

    if(!mMaterialEditor || name.toStdString() != mMaterialEditor->getName() || level == mMaterialEditor->getPropertyLevel() || ID == mMaterialEditor->getPropertyID())
    {
        ogRoot->GetSelection()->setSelection(0);

        if(mMaterialEditor)
            mMaterialEditor->destroy();

        CBaseEditor *parent = OgitorsRoot::getSingletonPtr()->GetRootEditor();
        OgitorsPropertyValueMap params;
        OgitorsPropertyValue value;
        value.propType = PROP_STRING;
        value.val = name.toStdString();
        params["name"] = value;

        CBaseEditorFactory *factory = OgitorsRoot::getSingletonPtr()->GetEditorObjectFactory("Material Object");
        mMaterialEditor = (CMaterialEditor*)factory->CreateObject(&parent, params);
        if(!mMaterialEditor)
            return;

        if(mMaterialEditor->load())
        {
            treeWidget->mMaterialEditor = mMaterialEditor;
            mMaterialEditor->setPropertiesLevel(level, ID);
            mMaterialEditor->setSelected(true);

            Ogre::Material* material = static_cast<Ogre::Material*>(mMaterialEditor->getHandle());
            try
            {
                std::string resourceGroup(Ogre::ResourceGroupManager::getSingletonPtr()->findGroupContainingResource(material->getOrigin()));
                Ogre::FileInfoListPtr fileInfoList(Ogre::ResourceGroupManager::getSingletonPtr()->findResourceFileInfo(resourceGroup, material->getOrigin()));
                Ogre::String materialPath = fileInfoList.getPointer()->at(0).archive->getName() + "/" + fileInfoList.getPointer()->at(0).filename;
                mMaterialEditor->setMaterialPath(materialPath);
            }
            catch(Ogre::Exception e)
            {
                QMessageBox::information(QApplication::activeWindow(),"qtOgitor", tr("This is only an internal, temporary material and therefore there is no displayable code available."));
            }
        }
    }
}
//----------------------------------------------------------------------------------------
void MaterialViewWidget::prepareView()
{
    treeWidget->clear();

    PropertyOptionsVector *materials = OgitorsRoot::getSingletonPtr()->GetMaterialNames();

    Ogre::String dumtitle = "";
    QTreeWidgetItem* rootitem = 0;
    QTreeWidgetItem* matitem = 0;
    QTreeWidgetItem* techitem = 0;
    QTreeWidgetItem* passitem = 0;

    rootitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("Project Materials")));
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    QString iconpath("/usr/share/qtOgitor/Plugins/Icons/project.svg");
#else
    QString iconpath(OgitorsUtils::QualifyPath("../Plugins/Icons/project.svg").c_str());
#endif
    rootitem->setIcon(0, QIcon(iconpath));
    QFont fnt = rootitem->font(0);
    fnt.setBold(true);
    rootitem->setFont(0, fnt);

    for(unsigned int i = 0;i < (*materials).size();i++)
    {
        Ogre::MaterialPtr MatPtr = Ogre::MaterialManager::getSingletonPtr()->getByName((*materials)[i].mKey);
        if(MatPtr.isNull())
            continue;

        matitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString((*materials)[i].mKey.c_str())));
        matitem->setIcon(0, QIcon(":/icons/material.svg"));
        rootitem->addChild(matitem);

        for(unsigned int t = 0;t < MatPtr->getNumTechniques();t++)
        {
            Ogre::Technique *tech = MatPtr->getTechnique(t);
            dumtitle = "Technique " + Ogre::StringConverter::toString(t) + ": (" + tech->getName() + ")";
            techitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(dumtitle.c_str())));
            techitem->setIcon(0, QIcon(":/icons/technique.svg"));
            techitem->setWhatsThis(0, QString("%1").arg(t << 8));
            matitem->addChild(techitem);

            for(unsigned int p = 0;p < tech->getNumPasses();p++)
            {
                Ogre::Pass *pass = tech->getPass(p);
                dumtitle = "Pass " + Ogre::StringConverter::toString(p) + ": (" + pass->getName() + ")";
                passitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(dumtitle.c_str())));
                passitem->setIcon(0, QIcon(":/icons/pass.svg"));
                passitem->setWhatsThis(0, QString("%1").arg((t << 8) + p));
                techitem->addChild(passitem);
            }
        }
    }

    treeWidget->insertTopLevelItem(0, rootitem);
}
//----------------------------------------------------------------------------------------
void MaterialViewWidget::onSceneLoadStateChange(Ogitors::IEvent* evt)
{
    Ogitors::LoadStateChangeEvent *change_event = Ogitors::event_cast<Ogitors::LoadStateChangeEvent*>(evt);

    if(change_event)
    {
        // Reload the zone selection widget when a scene is loaded
        LoadState state = change_event->getType();

        if(state == LS_LOADED)
            prepareView();
        else if(state == LS_UNLOADED)
            destroyScene();
    }
}
//----------------------------------------------------------------------------------------
void MaterialViewWidget::onGlobalPrepareView(Ogitors::IEvent* evt)
{
    prepareView();
}
//----------------------------------------------------------------------------------------