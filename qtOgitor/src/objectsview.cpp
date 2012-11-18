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

#include "mainwindow.hxx"
#include "objectsview.hxx"
#include "entityview.hxx"
#include "terraintoolswidget.hxx"

#include "qtogitorsystem.hxx"
#include "BaseEditor.h"
#include "NodeEditor.h"
#include "EntityEditor.h"
#include "ViewportEditor.h"
#include "SceneManagerEditor.h"

extern bool ViewKeyboard[1024];

using namespace Ogitors;
//----------------------------------------------------------------------------------------
ObjectsViewWidget::ObjectsViewWidget(QWidget *parent) :
    QWidget(parent), listWidget(0), mTimer(0)
{
    mDragData.Object = 0;
    mDragData.ObjectType = "";
    mDragData.Parameters.clear();

    listWidget = new QListWidget(this);
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setGridSize(QSize(64,70));
    listWidget->setFlow(QListView::LeftToRight);
    listWidget->setIconSize(QSize(32,32));
    listWidget->setDragDropMode(QAbstractItemView::DragOnly);
    listWidget->setWordWrap(true);

    QVBoxLayout *boxlayout = new QVBoxLayout(this);
    boxlayout->setMargin(0);
    boxlayout->addWidget(listWidget);

    OgitorsRoot::getSingletonPtr()->RegisterDragDropHandler((void*)listWidget, this);
}
//----------------------------------------------------------------------------------------
ObjectsViewWidget::~ObjectsViewWidget()
{
    delete mTimer;

    OgitorsRoot::getSingletonPtr()->UnRegisterDragDropHandler((void*)listWidget, this);
}
//----------------------------------------------------------------------------------------
void ObjectsViewWidget::resizeEvent(QResizeEvent* evt)
{
    if(listWidget)
        listWidget->setGridSize(QSize(64,70));
}
//----------------------------------------------------------------------------------------
void ObjectsViewWidget::updateView()
{
    if(!Ogitors::OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
        return;

    bool bModified = false;
    for(int i = 0;i < listWidget->count();i++)
    {
        Ogre::String type = listWidget->item(i)->whatsThis().toStdString();
        bool instantiate = Ogitors::OgitorsRoot::getSingletonPtr()->CanInstantiateObject(type);
        if(listWidget->item(i)->isHidden() == instantiate)
        {
            listWidget->item(i)->setHidden(!instantiate);
            bModified = true;
        }
    }
    if(bModified)
        listWidget->setGridSize(QSize(64,70));
}
//----------------------------------------------------------------------------------------
void ObjectsViewWidget::prepareView()
{
    listWidget->clear();

    Ogre::String filename;
    Ogre::String itemname;

    Ogitors::EditorObjectFactoryMap objects = Ogitors::OgitorsRoot::getSingletonPtr()->GetEditorObjectFactories();
    Ogitors::EditorObjectFactoryMap::iterator it = objects.begin();

    while(it != objects.end())
    {

        if(it->second && it->second->mAddToObjectList)
        {
            if(it->second->mIcon != "")
                filename = Ogitors::Globals::OGITOR_PLUGIN_ICON_PATH + "/" + it->second->mIcon;
            else
                filename = ":/icons/objects.svg";


            itemname = it->second->mTypeName;

            itemname.erase(itemname.length() - 7,7);

            filename = OgitorsUtils::QualifyPath(filename);

            QListWidgetItem *item = new QListWidgetItem(QIcon(QString(filename.c_str())), QString(itemname.c_str()), listWidget);
            item->setWhatsThis(it->second->mTypeName.c_str());
            listWidget->addItem(item);
        }

        it++;
      }

    listWidget->setGridSize(QSize(64,70));

    mTimer = new QTimer(this);
    mTimer->setInterval(1000);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(updateView()));
    mTimer->start();
}
//----------------------------------------------------------------------------------------
void ObjectsViewWidget::clearView()
{
    listWidget->clear();
    delete mTimer;
    mTimer = 0;
}
//----------------------------------------------------------------------------------------
bool ObjectsViewWidget::OnDragEnter()
{
    mDragData.Object = 0;
    mDragData.Parameters.clear();

    QList<QListWidgetItem*> selected = listWidget->selectedItems();
    if(selected.size())
    {
        QString stxt = selected[0]->text();

        CBaseEditorFactory *factory = OgitorsRoot::getSingletonPtr()->GetEditorObjectFactory(stxt.toStdString() + " Object");

        if(!factory)
            return false;

        mDragData.ObjectType = factory->mTypeName;

        Ogre::String placeholder = "";

        if(factory->mRequirePlacement)
            placeholder = factory->GetPlaceHolderName();

        if(placeholder == "")
            return true;

        OgitorsPropertyValue pvalue;
        mDragData.Parameters["init"] = EMPTY_PROPERTY_VALUE;
        pvalue.propType = PROP_STRING;
        pvalue.val = Ogre::Any(placeholder);
        mDragData.Parameters["meshfile"] = pvalue;
        pvalue.propType = PROP_VECTOR3;
        pvalue.val = Ogre::Any(Ogre::Vector3(999999,999999,999999));
        mDragData.Parameters["position"] = pvalue;

        CBaseEditor *parent = OgitorsRoot::getSingletonPtr()->GetSceneManagerEditor();
        CBaseEditorFactory *entityfactory = OgitorsRoot::getSingletonPtr()->GetEditorObjectFactory("Entity Object");
        mDragData.Object = entityfactory->CreateObject(&parent, mDragData.Parameters);
        mDragData.Object->load();

        static_cast<Ogre::Entity*>(mDragData.Object->getHandle())->setMaterialName("scbMATWIREFRAME");
        static_cast<Ogre::Entity*>(mDragData.Object->getHandle())->setQueryFlags(0);

        return true;
    }
    return false;
}
//----------------------------------------------------------------------------------------
void ObjectsViewWidget::OnDragLeave()
{
    if(mDragData.Object)
        mDragData.Object->destroy();

    mDragData.Object = 0;
}
//----------------------------------------------------------------------------------------
bool ObjectsViewWidget::OnDragMove(Ogre::Viewport *vp, unsigned int modifier, Ogre::Vector2& position)
{
    if(mDragData.ObjectType == "")
        return false;

    if(!mDragData.Object)
        return true;


    Ogre::Ray mouseRay;
    mouseRay = vp->getCamera()->getCameraToViewportRay(position.x, position.y);

    Ogre::Vector3 vPos;
    mDragData.Object->getProperties()->getValue("position", vPos);

    bool hitfound = false;

    if((modifier & Ogitors::DragDropControlModifier) || OgitorsRoot::getSingletonPtr()->GetSnapGroundState())
    {
        hitfound = OgitorsRoot::getSingletonPtr()->GetViewport()->GetHitPosition(mouseRay, vPos, mDragData.Object->getName());
    }

    if(!hitfound)
    {
        if(vPos.x == 999999 && vPos.y == 999999 && vPos.z == 999999)
            vPos = mouseRay.getOrigin() + (mouseRay.getDirection() * 40.0f);
        else
            vPos = OgitorsRoot::getSingletonPtr()->GetGizmoIntersectCameraPlane(mDragData.Object, mouseRay);
    }

    mDragData.Object->getProperties()->setValue("position", vPos);

    return true;
}
//----------------------------------------------------------------------------------------
void ObjectsViewWidget::OnDragWheel(Ogre::Viewport *vp, float delta)
{
    if(!mDragData.Object)
        return;

    Ogre::Vector3 vPos;
    mDragData.Object->getProperties()->getValue("position", vPos);
    float distance = (vPos - vp->getCamera()->getDerivedPosition()).length() + (delta / 120.0f);

    if(vPos.x == 999999 && vPos.y == 999999 && vPos.z == 999999)
        return;
    else
        vPos = vp->getCamera()->getDerivedPosition() + ((vPos - vp->getCamera()->getDerivedPosition()).normalisedCopy() * distance);

    mDragData.Object->getProperties()->setValue("position", vPos);
}
//----------------------------------------------------------------------------------------
void ObjectsViewWidget::OnDragDropped(Ogre::Viewport *vp, Ogre::Vector2& position)
{
    mDragData.Parameters.clear();

    mDragData.Parameters["init"] = EMPTY_PROPERTY_VALUE;

    if(mDragData.Object)
    {
        OgitorsPropertyValueMap params;
        mDragData.Object->getPropertyMap(params);
        mDragData.Object->destroy();
        mDragData.Object = 0;

        mDragData.Parameters["position"] = params["position"];
    }

    CBaseEditor *object = OgitorsRoot::getSingletonPtr()->CreateEditorObject(0,mDragData.ObjectType, mDragData.Parameters, true, true);

    if(object && object->isTerrainType())
        mOgitorMainWindow->getTerrainToolsWidget()->updateTerrainOptions(object->getTerrainEditor());


    mDragData.Parameters.clear();
    mDragData.ObjectType = "";
    mDragData.Object = 0;
}
//----------------------------------------------------------------------------------------
