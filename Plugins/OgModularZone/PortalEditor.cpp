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
#include "PortalEditor.h"
#include "ModularZoneEditor.h"
#include "ZoneInfo.h"
#include "portalsizedialog.hxx"
#include "OgitorsSystem.h"
#include <QtGui/QMenu>

using namespace Ogitors;
using namespace MZP;

//initialise static constants
const Ogre::Real PortalEditor::DEFAULT_WIDTH = 2.0;
const Ogre::Real PortalEditor::DEFAULT_HEIGHT = 2.0;
const Ogre::Real PortalEditor::MIN_WIDTH = 0.1;
const Ogre::Real PortalEditor::MIN_HEIGHT = 0.1;

PortalEditor::PortalEditor(Ogitors::CBaseEditorFactory *factory) : Ogitors::CNodeEditor(factory),
																	mPortalOutline(0),
																	mParentZone(0),
																	mPlaneHandle(0),
																	mConnected(0),
																	mLinked(false),
																	mFreeMove(false)
{
	mHandle = 0;
    mUsesGizmos = true;
    mUsesHelper = false;
	
}
//----------------------------------------------------------------------------------------
PortalEditor::~PortalEditor(void)
{

}
//----------------------------------------------------------------------------------------
void PortalEditor::showBoundingBox(bool bShow) 
{
    if(!mBoxParentNode)
        createBoundingBox();

    mBBoxNode->setVisible(bShow);
}
//-----------------------------------------------------------------------------------------
void PortalEditor::createProperties(Ogitors::OgitorsPropertyValueMap &params)
{
	PROPERTY_PTR(mDestination, "destination", Ogre::String, "", 0, SETTER(Ogre::String, PortalEditor, _setDestination));
	PROPERTY_PTR(mWidth, "width", Ogre::Real, 0.0, 0, SETTER(Ogre::Real, PortalEditor, _setWidth));
	PROPERTY_PTR(mHeight, "height", Ogre::Real, 0.0, 0, SETTER(Ogre::Real, PortalEditor, _setHeight));

	//always call this even if we add no props
	//it will init inherited props
	mProperties.initValueMap(params);
}
//-----------------------------------------------------------------------------------------
bool PortalEditor::_setDestination(OgitorsPropertyBase* property, const Ogre::String& value)
{
	mDestination->set(value);
	return true;
}
//-----------------------------------------------------------------------------------------
bool PortalEditor::_setWidth(OgitorsPropertyBase* property, const Ogre::Real& value)
{
	mWidth->set(value);
	return true;
}
//----------------------------------------------------------------------------------------
bool PortalEditor::_setHeight(OgitorsPropertyBase* property, const Ogre::Real& value)
{
	mHeight->set(value);
	return true;
}
//-----------------------------------------------------------------------------------------

bool PortalEditor::load(bool async)
{
    if(mLoaded->get())
        return true;
    
    if(Ogitors::CNodeEditor::load())
    {
		//create portal 
		mPortalOutline = OGRE_NEW PortalOutlineRenderable();
		mPortalOutline->setupVertices(mWidth->get(),mHeight->get());
		mHandle->attachObject(mPortalOutline);

		_createMaterial();
		_createPlane();
    }
    else
        return false;

    return true;
}
//-----------------------------------------------------------------------------------------
bool PortalEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

	//TODO: unload portal stuff
	if(mConnected)
	{
		//we have a connection - we need to tidy up
		mConnected->link(0); //make sure mConnected doesn't think its still 
		mConnected->connect(0);//linked to this portal (which is soon to be gone!)
	}

	if(mPlaneHandle)
	{
		mPlaneHandle->detachFromParent();
   		mPlaneHandle->_getManager()->destroyEntity(mPlaneHandle);
		mPlaneHandle = 0;
	}

	if(mPortalOutline)
	{
		mPortalOutline->detachFromParent();
		OGRE_DELETE mPortalOutline;
		mPortalOutline = 0;
	}

  
    return Ogitors::CNodeEditor::unLoad();
}

//-------------------------------------------------------------------------------
bool PortalEditor::getObjectContextMenu(UTFStringVector &menuitems) 
{
    menuitems.clear();
    
    
	if(this->mParentZone->getDesignMode())
	{
		if(mFreeMove)menuitems.push_back(OTR("Free Move;:/icons/debuginfo.svg"));//TODO: check box
		else menuitems.push_back(OTR("Free Move;:/icons/fileclose.svg"));		 //icons - tick/cross
		menuitems.push_back(OTR("Resize..."));
	}
	else
	if(mConnected) //if there is a potential destination...
	{
		if(mLinked) //already linked - menu item to unlink
		{
			menuitems.push_back(OTR("Unlink"));
		}
		else //not linked - menu item to link
		{
			menuitems.push_back(OTR("Link"));
		}
	}

	

	//possible feature -> snap-to ->portal_name.

    return true;
}
//-------------------------------------------------------------------------------
void PortalEditor::onObjectContextMenu(int menuresult) 
{
	if(this->mParentZone->getDesignMode())
	{
		if(menuresult == 0)//freemove
		{
			mFreeMove = !mFreeMove;
		}

		if(menuresult == 1)//resize
		{
			//open resize menu
			PortalSizeDialog dlg;

			//give the dialog the current dimensions
			dlg.setWidth(this->mWidth->get());
			dlg.setHeight(this->mHeight->get());
	
			if(dlg.exec())//OK
			{
				//retrieve new portal dimensions
				Ogre::Real x = dlg.getWidth();
				Ogre::Real y = dlg.getHeight();

				//resize portal
				this->mWidth->set(x);
				this->mHeight->set(y);

				//update portal outline
				mPortalOutline->setupVertices(x,y);

				//update selection plane
				_createPlane();
								
			}
		}
	}
	else
    if(menuresult == 0)//connect
	{
		if(mConnected) //if there is a potential destination...
		{
			if(mLinked) //unlink it
			{
				link(0);
				mConnected->link(0);
			}
			else //link it
			{
				link(mConnected);
				mConnected->link(this);
			}
		}

	}
   

}
//----------------------------------------------------------------------------------------
void PortalEditor::link(PortalEditor* dest)
{
	if(dest)
	{
		mDestination->set(dest->getName());
		mParentZone->setPortalLocked(true);
		mLinked = true;
		mPortalOutline->setPortalState(PortalOutlineRenderable::PS_LINKED);
	}
	else
	{
		mDestination->set("");
		mParentZone->setPortalLocked(false);
		mLinked = false;
		mPortalOutline->setPortalState(PortalOutlineRenderable::PS_CONNECTED);
	}
}
void PortalEditor::connect(PortalEditor* dest)
{
	mConnected = dest;
	mPortalOutline->setPortalState(PortalOutlineRenderable::PS_CONNECTED);
}
//-------------------------------------------------------------------------------
bool PortalEditor::connectNearPortals(bool bAllowMove)
{
	//compare this portal with others in the scene
	//NameObjectPairList portalList = OgitorsRoot::getSingletonPtr()->GetObjectsByType(this->getTypeID());
	NameObjectPairList portalList = OgitorsRoot::getSingletonPtr()->GetObjectsByTypeName("MZ Portal Object");
	NameObjectPairList::iterator itr;

	//check current connection
	if(mConnected)
	{
		Ogre::Real d = mConnected->getDerivedPosition().distance(this->getDerivedPosition());
					
			if(d>=1.0f) 
			{
				//break connection
				mConnected->connect(0);
				this->connect(0);
			}

	}

	for(itr=portalList.begin();itr!=portalList.end();++itr )
	{
		PortalEditor* that = dynamic_cast<PortalEditor*>((*itr).second);
		//check its not in the same Zone
		unsigned int id1 = that->getParent()->getObjectID();
		unsigned int id2 = (this->getParent()->getObjectID());
		//if((*itr).second->getParent()->getObjectID()!=this->getParent()->getObjectID())
		if(id1!=id2)
		{
			//check portal proximity
			Ogre::Real d = that->getDerivedPosition().distance(this->getDerivedPosition());
					
			if(d<1.0f) 
			{
				
				if(this->snapTpPortal(that,bAllowMove))
				{
					//join the portals,set lock
					this->connect(that);
					that->connect(this);
					return true;
			
				}

			}

		}
	}
	mPortalOutline->setPortalState(PortalOutlineRenderable::PS_FREE);
	return false;

}
//-------------------------------------------------------------------------------
bool PortalEditor::snapTpPortal(PortalEditor* dest,bool bAllowMove )
{
	//reposition & realign this portal (and its parent zone) 
	//to connect with this portal.

	//Before snapping portals togther, we should check that the zone is 
	//not already locked into position by another portal join.
	//However, even if this is the case, we can still join portals if 
	//they are already in the correct position.

	//get current position data:
	Ogre::Quaternion qZone = mParentZone->getDerivedOrientation();
	Ogre::Quaternion qDest = dest->getDerivedOrientation();
	Ogre::Quaternion qPortal = this->getOrientation();
	Ogre::Vector3 vDest = dest->getDerivedPosition();
	Ogre::Vector3 vPortal = this->getDerivedPosition();

	
	const Ogre::Real DIST_EPSILON(0.01f);//fudge factor
	const Ogre::Radian ANG_EPSILON(0.01f);
	if(vPortal.distance(vDest)<DIST_EPSILON && qPortal.equals(qDest*Ogre::Quaternion(0,0,1,0),ANG_EPSILON))return true;
	if(!bAllowMove)return false;  

	//orientation
	Ogre::Quaternion qNew = (qDest*Ogre::Quaternion(0,0,1,0))*qPortal.Inverse();
	mParentZone->setDerivedOrientation(qNew);

	//position
	Ogre::Vector3 vZone = mParentZone->getDerivedPosition();
	vPortal = this->getDerivedPosition();

	mParentZone->setDerivedPosition( (vDest - (vPortal-vZone)));

	return true;
}
//-------------------------------------------------------------------------------
Ogre::Entity* PortalEditor::_createPlane()
{
    Ogre::MeshPtr mesh;
    if(mPlaneHandle)
    {
        mPlaneHandle->detachFromParent();
        mPlaneHandle->_getManager()->destroyEntity(mPlaneHandle);
        Ogre::MeshManager::getSingletonPtr()->remove(mName->get());        
    }

	Ogre::Plane plane(Ogre::Vector3::UNIT_Z, 0.0);
    mesh = Ogre::MeshManager::getSingletonPtr()->createPlane(mName->get(),PROJECT_RESOURCE_GROUP,plane,mWidth->get(),mHeight->get());
    mPlaneHandle = OgitorsRoot::getSingletonPtr()->GetSceneManager()->createEntity(mName->get(), mName->get());
    mPlaneHandle->setQueryFlags(QUERYFLAG_MOVABLE);
    
	


	mPlaneHandle->setMaterialName("MZP_Portal_Material");

    mPlaneHandle->setCastShadows(false);

    mHandle->attachObject(mPlaneHandle);

	/*if(mBBoxNode)
		adjustBoundingBox();*/

    return mPlaneHandle;
}
void PortalEditor::_createMaterial()
{
	// create a transparent material for the portal plane.
	//The plane only exists to make it selectable, so we don't want to see it
	Ogre::MaterialPtr       material; 
	//TODO:  **********************ModularZone Resource Group ??? *******************************
	Ogre::ResourceManager::ResourceCreateOrRetrieveResult result = Ogre::MaterialManager::getSingleton().createOrRetrieve("MZP_Portal_Material", "General");
	if(result.second)
	{
		material = result.first;
		material->getTechnique(0)->getPass(0)->setPolygonMode(Ogre::PM_POINTS);
		//FOR SOME REASON TRANSPARENCY DOESN'T WORK, so I set polygon mode to points
	
	}

}
//-------------------------------------------------------------------------------
bool PortalEditor::postSceneUpdate(Ogre::SceneManager *SceneMngr, Ogre::Camera *Camera, Ogre::RenderWindow *RenderWindow) 
{
	//Called after scene load to reconnect all linked portals
	Ogitors::OgitorsPropertyBase*prop;
	Ogitors::OgitorsPropertySet* props;
	props = getProperties();
	prop = props->getProperty("destination");
	Ogre::String dest = Ogre::any_cast<Ogre::String>(prop->getValue());
	if(!dest.empty())
	{
		//find portal matching dest name
		mConnected = dynamic_cast<PortalEditor*>(OgitorsRoot::getSingletonPtr()->FindObject(dest,this->getEditorType()));
		if(mConnected)
		{
			mLinked=true;
			mPortalOutline->setPortalState(PortalOutlineRenderable::PS_LINKED);

		}
		
	}

	this->setLocked(true);
	return false;
}

//----------------------------------------------------------------------------------------
void PortalEditor::setSelectedImpl(bool bSelected)
{
	
	if (bSelected && this->mParentZone->getDesignMode())
	{
		//because the zone is in design mode, we need to get updates
		//because this portals can be moved and we want check for positioning
		//tools in the parent zone

		//register for updates
		OgitorsRoot::getSingletonPtr()->RegisterForUpdates(this);
	}
	else
	{
		//unregister
		OgitorsRoot::getSingletonPtr()->UnRegisterForUpdates(this);
	}
	
	CBaseEditor::setSelectedImpl(bSelected);

}
//----------------------------------------------------------------------------------------
bool PortalEditor::update(float timePassed)
{
	//TODO: check for proximity to positioning tools in parent mesh
	//trigger snap-to when close enough.
	bool bModified = false;//return value for Ogitor

	if (getSelected()&& (!mFreeMove))
	{
		bool snapped = false;
		ZoneDesignTools* tools = this->mParentZone->getTools();
		//Check against tools->mHoles
		std::vector<HoleSnapPoint>::iterator itr;
		for(itr = tools->mHoles.begin();itr != tools->mHoles.end() && !snapped ;++itr)
		{

			//get current position data:
			Ogre::Vector3 vDest = (*itr).first;
			Ogre::Vector3 vPortal = this->getPosition();
			Ogre::Quaternion qDest = (*itr).second;
			Ogre::Quaternion qPortal = this->getOrientation();
			

			//check portal proximity
			Ogre::Real d = vPortal.distance(vDest);
					
			if(d<1.0f) 
			{
				
				this->setPosition(vDest);//move into position
				this->setOrientation(qDest);//adjust orientation
				snapped = true;

			}

		}

		bModified = true;
		
	}

	return bModified;
}