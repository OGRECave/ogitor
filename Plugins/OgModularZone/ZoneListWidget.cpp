///Modular Zone Plugin
///
/// Copyright (c) 2009 Gary Mclean
//
//This program is free software; you can redistribute it and/or modify it under
//the terms of the GNU Lesser General Public License as published by the Free Software
//Foundation; either version 2 of the License, or (at your option) any later
//version.
//
//This program is distributed in the hope that it will be useful, but WITHOUT
//ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
//FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
//
//You should have received a copy of the GNU Lesser General Public License along with
//this program; if not, write to the Free Software Foundation, Inc., 59 Temple
//Place - Suite 330, Boston, MA 02111-1307, USA, or go to
//http://www.gnu.org/copyleft/lesser.txt.
////////////////////////////////////////////////////////////////////////////////*/

//Based on the EntityView class from Ogitor

#include "ZoneListWidget.hxx"
#include "ModularZoneEditor.h"
#include "ModularZoneFactory.h"
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QListWidget>
#include <QtGui/QToolButton>
#include <QtGui/QLineEdit>
#include <QtGui/QPixmap>
#include <QtGui/QImage>
#include <QtGui/QLabel>
#include <QtGui/QFrame>
#include <QtCore/QEvent>
#include "BaseEditor.h"
#include "NodeEditor.h"
#include "ViewportEditor.h"
#include "SceneManagerEditor.h"
#include "DefaultEvents.h"
#include "EventManager.h"
#include "OgreMeshManager.h"

using namespace Ogitors;
using namespace MZP;

ZoneListWidget::ZoneListWidget(QWidget* parent):QWidget(parent)
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
	listWidget->setMouseTracking(true);
	
    verticalLayout->addWidget(listWidget);
    verticalLayout->setMargin(0);

    QObject::connect(clearFilterButton, SIGNAL(clicked()), filterBox, SLOT(clear()));
    QObject::connect(filterBox, SIGNAL(textChanged(QString)), this, SLOT(filterBoxTextChanged(QString)));
	
	OgitorsRoot::getSingletonPtr()->RegisterDragDropHandler((void*)listWidget, this);
	
    EventManager::getSingletonPtr()->connectEvent(EventManager::LOAD_STATE_CHANGE, this, true, 0, true, 0, EVENT_CALLBACK(ZoneListWidget, onSceneLoadStateChange));

    /*this->setWindowTitle(QString::fromUtf8("Zones"));
	this->setWindowIconText("../Plugins/Icons/zone.svg");*/
}
//----------------------------------------------------------------------------------------
ZoneListWidget::~ZoneListWidget(void)
{
	OgitorsRoot::getSingletonPtr()->UnRegisterDragDropHandler((void*)listWidget, this);
    EventManager::getSingletonPtr()->disconnectEvent(EventManager::LOAD_STATE_CHANGE, this);
}

//----------------------------------------------------------------------------------------
void ZoneListWidget::resizeEvent(QResizeEvent* evt)
{
    if(listWidget)
        listWidget->setGridSize(QSize(64,64));
}
//----------------------------------------------------------------------------------------
void ZoneListWidget::prepareView()
{
    _createImages(mIcons);

    listWidget->clear();

    Ogre::String filename;
    Ogre::String itemname;
	ImageMap::iterator it = mIcons.begin();

	ModularZoneFactory* factory = dynamic_cast<ModularZoneFactory*>(OgitorsRoot::getSingletonPtr()->GetEditorObjectFactory("Modular Zone Object"));
	if(!factory)return;
		
	while(it != mIcons.end())
    {
		
		QImage pImg(it->second, 96, 96, QImage::Format_ARGB32);
		QPixmap pmap = QPixmap::fromImage(pImg);
		QListWidgetItem *item = new QListWidgetItem(QIcon(pmap),(factory->getZoneTemplate(it->first))->mName.c_str() , listWidget);
		item->setData(Qt::UserRole,QVariant(it->first));//key to ZoneTemplatesMap 
		item->setToolTip((factory->getZoneTemplate(it->first))->mShortDesc.c_str());// a short description of the zone
		item->setStatusTip((factory->getZoneTemplate(it->first))->mLongDesc.c_str());//a detailed description
        
		listWidget->addItem(item);
		delete [] it->second;
		it++;
    }
	mIcons.clear();
}
//----------------------------------------------------------------------------------------
void ZoneListWidget::addZone(int key)
{
	ModularZoneFactory* factory = dynamic_cast<ModularZoneFactory*>(OgitorsRoot::getSingletonPtr()->GetEditorObjectFactory("Modular Zone Object"));
	if(!factory)return;
	ZoneInfo* zone = factory->getZoneTemplate(key);
	if(zone)
	{
		
		QIcon icon( "../Plugins/Icons/zone.svg");
		QListWidgetItem *item = new QListWidgetItem(icon,zone->mName.c_str() , listWidget);
		item->setData(Qt::UserRole,QVariant(key));//key to ZoneTemplatesMap 
		item->setToolTip(zone->mShortDesc.c_str());// a short description of the zone
		item->setStatusTip(zone->mLongDesc.c_str());//a detailed description
        
		listWidget->addItem(item);
	}
}
//----------------------------------------------------------------------------------------
void ZoneListWidget::updateZoneInfo(int key)
{
	//TODO: there's gotta be a better way to do this...

	//update zone description 
	ModularZoneFactory* factory = dynamic_cast<ModularZoneFactory*>(OgitorsRoot::getSingletonPtr()->GetEditorObjectFactory("Modular Zone Object"));
	if(!factory)return;
	ZoneInfo* zone = factory->getZoneTemplate(key);

	for(int i=0;i<listWidget->count();i++)
	{
		QListWidgetItem *item = listWidget->item(i);
		if(item->data(Qt::UserRole).toInt()==key)
		{
			item->setToolTip(zone->mShortDesc.c_str());// a short description of the zone
			item->setStatusTip(zone->mLongDesc.c_str());//a detailed description
		}
	}
}

//----------------------------------------------------------------------------------------
void ZoneListWidget::clearView()
{
    listWidget->clear();
}
//----------------------------------------------------------------------------------------
typedef std::map<int,Ogre::String> EntityMap;

void ZoneListWidget::_createImages(ImageMap& retlist)
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

	ModularZoneFactory* factory = dynamic_cast<ModularZoneFactory*>(OgitorsRoot::getSingletonPtr()->GetEditorObjectFactory("Modular Zone Object"));
	if(!factory)return;
	factory->loadZoneTemplates();
	ZoneInfoMap zoneTemplates = factory->getZoneTemplateMap();

    Ogre::Entity *mEntity;

    unsigned char dataptr[300 * 300 * 6]; 
    unsigned char *dataptr2;
    Ogre::PixelBox pb(256,256,1,Ogre::PF_A8R8G8B8, dataptr);

	EntityMap entities;
	ZoneInfoMap::iterator zi;
	for(zi=zoneTemplates.begin();zi!=zoneTemplates.end();++zi)
	{
		Ogre::String addstr = (*zi).second.mMesh;
		if(entities.find((*zi).first) == entities.end())
			entities.insert(EntityMap::value_type((*zi).first,addstr));
	}

	EntityMap::const_iterator ite = entities.begin();
	
    while(ite != entities.end())
	{
		Ogre::String addstr = ite->second;

		mEntity = mSceneMgr->createEntity("MZP_Preview", addstr);

		mSceneMgr->getRootSceneNode()->attachObject(mEntity);

		//TODO: It would be nice to retrieve a Preview Camera Position from 
		//the .zone file
		//TODO: also render portal outlines clearly so that the user can see
		//how the zone is laid out
		Ogre::Vector3 vSize = mEntity->getBoundingBox().getCorner(Ogre::AxisAlignedBox::NEAR_RIGHT_TOP);//.getHalfSize();//============
	    Ogre::Vector3 vCenter = mEntity->getBoundingBox().getCenter(); 
	
//FIXME ------ NICE PREVIEWS NEEDED - bigger

        vSize += Ogre::Vector3(vSize.z, vSize.z, vSize.z);

        float maxsize = std::max(std::max(vSize.x,vSize.y),vSize.z);
	
	    //vSize = Ogre::Vector3(0, 0, maxsize * 1.1f) + vCenter;
		vSize = Ogre::Vector3(maxsize * 0.5f, vSize.y, maxsize * 0.5f) + vCenter;
		//vSize.x +=vSize.x/2;//Maybe test to see which is larger x/2 or z/2 and use that?
		//vSize.z +=vSize.x/2;
		//RTTCam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	
	    RTTCam->setPosition(vSize.x,vSize.y,vSize.z);
	    RTTCam->lookAt(vCenter.x,vCenter.y,vCenter.z);

        rttTex->update();
        rttTex->copyContentsToMemory(pb, Ogre::RenderTarget::FB_FRONT);
        

        dataptr2 = new unsigned char[96 * 96 * 4];
        Ogre::PixelBox pb2(96,96,1,Ogre::PF_A8R8G8B8, dataptr2);
		Ogre::Image::scale(pb,pb2);

		addstr.erase(addstr.length() - 5, 5);
		retlist.insert(ImageMap::value_type((*ite).first, dataptr2));

        mEntity->detachFromParent();
	    mSceneMgr->destroyEntity(mEntity);

		ite++;
    }

    rttTex->removeAllViewports();
    Ogre::Root::getSingletonPtr()->destroySceneManager(mSceneMgr);
    Ogre::TextureManager::getSingletonPtr()->unload(texture->getName());
    Ogre::TextureManager::getSingletonPtr()->remove(texture->getName());
}	
//----------------------------------------------------------------------------------------
void ZoneListWidget::filterBoxTextChanged(QString text)
{
    if(!Ogitors::OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
        return;

    for(int i = 0;i < listWidget->count();i++)
    {
        QString name = listWidget->item(i)->text();
		if(name.contains(text) || text.trimmed().isEmpty())
            listWidget->item(i)->setHidden(false);
		else
            listWidget->item(i)->setHidden(true);
    }
    listWidget->setGridSize(QSize(64,64));
}
//----------------------------------------------------------------------------------------
bool ZoneListWidget::OnDragEnter()
{
    QList<QListWidgetItem*> selected = listWidget->selectedItems();
    if(selected.size())
    {
        QString stxt = selected[0]->text();
		mDragData.ObjectType = "Modular Zone Object";
        OgitorsPropertyValue pvalue;
		mDragData.Parameters["init"] = EMPTY_PROPERTY_VALUE;

        pvalue.propType = PROP_INT;
		pvalue.val = Ogre::Any(selected[0]->data(Qt::UserRole).toInt());
		mDragData.Parameters["zonetemplate"] = pvalue;
        
		CBaseEditor *parent = OgitorsRoot::getSingletonPtr()->GetSceneManagerEditor();
        CBaseEditorFactory *factory = OgitorsRoot::getSingletonPtr()->GetEditorObjectFactory("Modular Zone Object");
		mDragData.Object = factory->CreateObject(&parent, mDragData.Parameters);
		dynamic_cast<ModularZoneEditor*>(mDragData.Object)->mDragging = true;
		mDragData.Object->load();

		static_cast<ModularZoneEditor*>(mDragData.Object)->getZoneMesh()->setMaterialName("scbMATWIREFRAME");
        static_cast<ModularZoneEditor*>(mDragData.Object)->getZoneMesh()->setQueryFlags(0);

		return true;
    }
	return false;
}
//----------------------------------------------------------------------------------------
void ZoneListWidget::OnDragLeave()
{
	if(mDragData.Object)
		mDragData.Object->destroy();

	mDragData.Object = 0;
}
//----------------------------------------------------------------------------------------
bool ZoneListWidget::OnDragMove (Ogre::Viewport *vp, unsigned int modifier, Ogre::Vector2& position)
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
void ZoneListWidget::OnDragWheel(Ogre::Viewport *vp, float delta)
{
    if(!mDragData.Object)
		return;
	//TODO : rotate around Y axis
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
void ZoneListWidget::OnDragDropped(Ogre::Viewport *vp, Ogre::Vector2& position)
{
	Ogitors::CBaseEditor* obj =0;
	if(mDragData.Object)
    {
        OgitorsPropertyValueMap params;
        mDragData.Object->getPropertyMap(params);
        mDragData.Object->destroy();
        mDragData.Object = 0;

        mDragData.Parameters["position"] = params["position"];

	    obj = OgitorsRoot::getSingletonPtr()->CreateEditorObject(0,mDragData.ObjectType, mDragData.Parameters, true, true);
	
	}

	mDragData.Parameters.clear();
    mDragData.ObjectType = "";
	mDragData.Object = 0;
}
//----------------------------------------------------------------------------------------

void ZoneListWidget::onSceneLoadStateChange(Ogitors::IEvent* evt)
{
    LoadStateChangeEvent *change_event = Ogitors::event_cast<LoadStateChangeEvent*>(evt);

    if(change_event)
    {
        //reload the zone selection widget when a scene is loaded
        LoadState state = change_event->getType();
	    
        if(state == LS_LOADED)
            prepareView();
        else if(state == LS_UNLOADED)
        {
            clearView();
        }
    }
}
//----------------------------------------------------------------------------------------
