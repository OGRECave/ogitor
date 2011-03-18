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

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QListWidget>
#include <QtGui/QToolButton>
#include <QtGui/QLineEdit>
#include <QtGui/QPixmap>
#include <QtGui/QImage>
#include "qtogitorsystem.h"
#include "entityview.hxx"
#include "BaseEditor.h"
#include "NodeEditor.h"
#include "EntityEditor.h"
#include "ViewportEditor.h"
#include "SceneManagerEditor.h"

extern bool ViewKeyboard[1024];

using namespace Ogitors;
//----------------------------------------------------------------------------------------
EntityViewWidget::EntityViewWidget(QWidget *parent) :
    QWidget(parent), listWidget(0)
{
    mDragData.Object = 0;
    mDragData.ObjectType = "";
    mDragData.Parameters.clear();

    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    filterBox = new QLineEdit(this);
    filterBox->setObjectName(QString::fromUtf8("filterBox"));
    
    horizontalLayout->addWidget(filterBox);
    
    clearFilterButton = new QToolButton(this);
    clearFilterButton->setObjectName(QString::fromUtf8("clearFilterButton"));
    clearFilterButton->setIcon(QIcon(":/icons/refresh.svg"));
    
    horizontalLayout->addWidget(clearFilterButton);
    horizontalLayout->setMargin(0);
    
    verticalLayout->addLayout(horizontalLayout);

    listWidget = new QListWidget(this);
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setGridSize(QSize(64,64));
    listWidget->setFlow(QListView::LeftToRight);
    listWidget->setIconSize(QSize(48,48));
    listWidget->setDragDropMode(QAbstractItemView::DragOnly);
    listWidget->setWordWrap(true);
    
    verticalLayout->addWidget(listWidget);
    verticalLayout->setMargin(0);
    
    QObject::connect(clearFilterButton, SIGNAL(clicked()), filterBox, SLOT(clear()));
    QObject::connect(filterBox, SIGNAL(textChanged(QString)), this, SLOT(filterBoxTextChanged(QString)));

    OgitorsRoot::getSingletonPtr()->RegisterDragDropHandler((void*)listWidget, this);
}
//----------------------------------------------------------------------------------------
EntityViewWidget::~EntityViewWidget()
{
    OgitorsRoot::getSingletonPtr()->UnRegisterDragDropHandler((void*)listWidget, this);
}
//----------------------------------------------------------------------------------------
void EntityViewWidget::resizeEvent(QResizeEvent* evt)
{
    if(listWidget)
        listWidget->setGridSize(QSize(64,64));
}
//----------------------------------------------------------------------------------------
void EntityViewWidget::prepareView()
{
    _createImages(mIcons);

    listWidget->clear();

    Ogre::String filename;
    Ogre::String itemname;
    ImageMap::iterator it = mIcons.begin();

    while(it != mIcons.end())
    {
        QImage pImg(it->second, 96, 96, QImage::Format_ARGB32);
        QPixmap pmap = QPixmap::fromImage(pImg);
        QListWidgetItem *item = new QListWidgetItem(QIcon(pmap), it->first, listWidget);
        item->setToolTip(it->first);
        listWidget->addItem(item);
        delete [] it->second;
        it++;
    }
    mIcons.clear();
}
//----------------------------------------------------------------------------------------
void EntityViewWidget::clearView()
{
    listWidget->clear();
}
//----------------------------------------------------------------------------------------
typedef std::map<Ogre::String,Ogre::String> EntityMap;

void EntityViewWidget::_createImages(ImageMap& retlist)
{
    retlist.clear();
    
    Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().createManual( "EntityTex", 
                   Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 
                   256, 256, 0, Ogre::PF_A8R8G8B8 , Ogre::TU_RENDERTARGET );

    Ogre::RenderTexture *rttTex = texture->getBuffer()->getRenderTarget();
    Ogre::SceneManager *mSceneMgr = Ogre::Root::getSingletonPtr()->createSceneManager("OctreeSceneManager", "EntityTexMgr");

    Ogre::Light *dirl = mSceneMgr->createLight("DisplayLight");
    dirl->setDirection(-1,-1,-1);
    dirl->setDiffuseColour(1,1,1);
    dirl->setType(Ogre::Light::LT_DIRECTIONAL);

    Ogre::Camera* RTTCam = mSceneMgr->createCamera("EntityCam");
    RTTCam->setNearClipDistance(0.01F);
    RTTCam->setFarClipDistance(0);
    RTTCam->setAspectRatio(1);
    RTTCam->setFOVy(Ogre::Degree(90));
    RTTCam->setPosition(0,0,1);
    RTTCam->lookAt(0,0,0);

    Ogre::Viewport *v = rttTex->addViewport( RTTCam );
    v->setClearEveryFrame( true );
    v->setBackgroundColour(Ogre::ColourValue(0,0,0,0));

    Ogre::StringVectorPtr pList = Ogre::ResourceGroupManager::getSingleton().findResourceNames(PROJECT_RESOURCE_GROUP,"*.mesh",false);
    
    Ogre::Entity *mEntity;

    unsigned char dataptr[300 * 300 * 6]; 
    unsigned char *dataptr2;
    Ogre::PixelBox pb(256,256,1,Ogre::PF_A8R8G8B8, dataptr);

    EntityMap entities;
    for(unsigned int i = 0;i < pList->size();i++)
    {
        Ogre::String addstr = (*pList)[i];
        if(entities.find(addstr) == entities.end())
            entities.insert(EntityMap::value_type(addstr,addstr));
    }

    EntityMap::const_iterator ite = entities.begin();

    while(ite != entities.end())
    {
        Ogre::String addstr = ite->first;

        mEntity = mSceneMgr->createEntity("scbDisplay", addstr);
        mSceneMgr->getRootSceneNode()->attachObject(mEntity);

        Ogre::Vector3 vSize = mEntity->getBoundingBox().getHalfSize();
        Ogre::Vector3 vCenter = mEntity->getBoundingBox().getCenter(); 
    
        vSize += Ogre::Vector3(vSize.z, vSize.z, vSize.z);

        float maxsize = std::max(std::max(vSize.x,vSize.y),vSize.z);
    
        vSize = Ogre::Vector3(0, 0, maxsize * 1.1f) + vCenter;
    
        RTTCam->setPosition(vSize.x,vSize.y,vSize.z);
        RTTCam->lookAt(vCenter.x,vCenter.y,vCenter.z);

        try
        {
            rttTex->update();
            rttTex->copyContentsToMemory(pb, Ogre::RenderTarget::FB_FRONT);
        

            dataptr2 = new unsigned char[96 * 96 * 4];
            Ogre::PixelBox pb2(96,96,1,Ogre::PF_A8R8G8B8, dataptr2);
            Ogre::Image::scale(pb,pb2);

            addstr.erase(addstr.length() - 5, 5);
            retlist.insert(ImageMap::value_type(addstr.c_str(), dataptr2));
        }
        catch(...)
        {
            OgitorsSystem::getSingletonPtr()->DisplayMessageDialog(Ogre::UTFString("Error Preparing Mesh : ") + mEntity->getName(), DLGTYPE_OK);
        }

        mEntity->detachFromParent();
        mSceneMgr->destroyEntity(mEntity);

        ite++;
    }

    rttTex->removeAllViewports();
    Ogre::Root::getSingletonPtr()->destroySceneManager(mSceneMgr);
    Ogre::TextureManager::getSingletonPtr()->unload(texture->getName());
    Ogre::TextureManager::getSingletonPtr()->remove(texture->getName());

    pList.setNull();
}
//----------------------------------------------------------------------------------------
void EntityViewWidget::filterBoxTextChanged(QString text)
{
    if(!Ogitors::OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
        return;

    for(int i = 0;i < listWidget->count();i++)
    {
        QString name = listWidget->item(i)->text();
        if(name.contains(text, Qt::CaseInsensitive) || text.trimmed().isEmpty())
            listWidget->item(i)->setHidden(false);
        else
            listWidget->item(i)->setHidden(true);
    }
    listWidget->setGridSize(QSize(64,64));
}
//----------------------------------------------------------------------------------------
bool EntityViewWidget::OnDragEnter()
{
    QList<QListWidgetItem*> selected = listWidget->selectedItems();
    if(selected.size())
    {
        QString stxt = selected[0]->text();
        mDragData.ObjectType = "Entity Object";
        OgitorsPropertyValue pvalue;
        mDragData.Parameters["init"] = EMPTY_PROPERTY_VALUE;
        pvalue.propType = PROP_STRING;
        pvalue.val = Ogre::Any(stxt.toStdString() + ".mesh");
        mDragData.Parameters["meshfile"] = pvalue;
        pvalue.propType = PROP_VECTOR3;
        pvalue.val = Ogre::Any(Ogre::Vector3(999999,999999,999999));
        mDragData.Parameters["position"] = pvalue;
        
        CBaseEditor *parent = OgitorsRoot::getSingletonPtr()->GetSceneManagerEditor();
        CBaseEditorFactory *factory = OgitorsRoot::getSingletonPtr()->GetEditorObjectFactory("Entity Object");
        mDragData.Object = factory->CreateObject(&parent, mDragData.Parameters);
        mDragData.Object->load();
        
        static_cast<Ogre::Entity*>(mDragData.Object->getHandle())->setMaterialName("scbMATWIREFRAME");
        static_cast<Ogre::Entity*>(mDragData.Object->getHandle())->setQueryFlags(0);

        return true;
    }
    return false;
}
//----------------------------------------------------------------------------------------
void EntityViewWidget::OnDragLeave()
{
    if(mDragData.Object)
        mDragData.Object->destroy();

    mDragData.Object = 0;
}
//----------------------------------------------------------------------------------------
bool EntityViewWidget::OnDragMove(Ogre::Viewport *vp, unsigned int modifier, Ogre::Vector2& position)
{
    if(!mDragData.Object)
        return false;

    Ogre::Ray mouseRay;
    mouseRay = vp->getCamera()->getCameraToViewportRay(position.x, position.y);

    Ogre::Vector3 vPos;
    mDragData.Object->getProperties()->getValue("position", vPos);

    bool hitfound = false;

    if(modifier & Ogitors::DragDropControlModifier)
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
void EntityViewWidget::OnDragWheel(Ogre::Viewport *vp, float delta)
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
void EntityViewWidget::OnDragDropped(Ogre::Viewport *vp, Ogre::Vector2& position)
{
    if(mDragData.Object)
    {
        OgitorsPropertyValueMap params;
        mDragData.Object->getPropertyMap(params);
        mDragData.Object->destroy();
        mDragData.Object = 0;

        mDragData.Parameters["position"] = params["position"];

        OgitorsRoot::getSingletonPtr()->CreateEditorObject(0,mDragData.ObjectType, mDragData.Parameters, true, true);
    }

    mDragData.Parameters.clear();
    mDragData.ObjectType = "";
    mDragData.Object = 0;
}
//----------------------------------------------------------------------------------------
