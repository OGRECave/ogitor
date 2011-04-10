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
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>
#include "tinyxml.h"
#include "TerrainEditor.h"
//#include "OgreResourceGroupManager.h"

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
																	mFreeMove(false),
																	mTerrainCut(0)
{
	mHandle = 0;
    mUsesGizmos = true;
    mUsesHelper = false;
	
}
//----------------------------------------------------------------------------------------
PortalEditor::~PortalEditor(void)
{
	delete mTerrainCut;
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
	{
		
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
		else
		{
			menuitems.push_back(OTR("Carve Terrain"));
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
	{


		if(mConnected)//if there is a potential destination...
		{
			if(menuresult == 0)//connect 
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
		else
		{
			if(menuresult == 0)//Carve tunnel thru terrain
			{
				carveTerrainTunnel();
				QMessageBox::information(QApplication::activeWindow(),"qtOgitor", "This doesn't do anything yet...",  QMessageBox::Ok);
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
		if(mPortalOutline)
			mPortalOutline->setPortalState(PortalOutlineRenderable::PS_LINKED);
	}
	else
	{
		mDestination->set("");
		mParentZone->setPortalLocked(false);
		mLinked = false;
		if(mPortalOutline)
			mPortalOutline->setPortalState(PortalOutlineRenderable::PS_CONNECTED);
	}
}
void PortalEditor::connect(PortalEditor* dest)
{
	mConnected = dest;
	if(mPortalOutline)
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
	if(mPortalOutline)
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
			if(mPortalOutline)
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
TiXmlElement* PortalEditor::exportDotScene(TiXmlElement *pParent)
{
    TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement("portal"))->ToElement();

    // node properties
    pNode->SetAttribute("name", mName->get().c_str());
    pNode->SetAttribute("id", Ogre::StringConverter::toString(mObjectID->get()).c_str());
    // position
    TiXmlElement *pPosition = pNode->InsertEndChild(TiXmlElement("position"))->ToElement();
    pPosition->SetAttribute("x", Ogre::StringConverter::toString(mPosition->get().x).c_str());
    pPosition->SetAttribute("y", Ogre::StringConverter::toString(mPosition->get().y).c_str());
    pPosition->SetAttribute("z", Ogre::StringConverter::toString(mPosition->get().z).c_str());
    // rotation
    TiXmlElement *pRotation = pNode->InsertEndChild(TiXmlElement("rotation"))->ToElement();
    pRotation->SetAttribute("qw", Ogre::StringConverter::toString(mOrientation->get().w).c_str());
    pRotation->SetAttribute("qx", Ogre::StringConverter::toString(mOrientation->get().x).c_str());
    pRotation->SetAttribute("qy", Ogre::StringConverter::toString(mOrientation->get().y).c_str());
    pRotation->SetAttribute("qz", Ogre::StringConverter::toString(mOrientation->get().z).c_str());
    // scale
    TiXmlElement *pScale = pNode->InsertEndChild(TiXmlElement("scale"))->ToElement();
    pScale->SetAttribute("x", Ogre::StringConverter::toString(mScale->get().x).c_str());
    pScale->SetAttribute("y", Ogre::StringConverter::toString(mScale->get().y).c_str());
    pScale->SetAttribute("z", Ogre::StringConverter::toString(mScale->get().z).c_str());

	//*coords of portal corners*
	//(use that instead of width/height, thus this format can be used for 
	//more fully implemented PCZSM editors)
	TiXmlElement *pCorners = pNode->InsertEndChild(TiXmlElement("corners"))->ToElement();

	TiXmlElement *pCorner0 = pCorners->InsertEndChild(TiXmlElement("corner0"))->ToElement();
	pCorner0->SetAttribute("x", Ogre::StringConverter::toString(-(mWidth->get())/2).c_str()); 
	pCorner0->SetAttribute("y", Ogre::StringConverter::toString((mHeight->get())/2).c_str());
	pCorner0->SetAttribute("z", "0.0");

	TiXmlElement *pCorner1 = pCorners->InsertEndChild(TiXmlElement("corner1"))->ToElement();
	pCorner1->SetAttribute("x", Ogre::StringConverter::toString((mWidth->get())/2).c_str()); 
	pCorner1->SetAttribute("y", Ogre::StringConverter::toString((mHeight->get())/2).c_str());
	pCorner1->SetAttribute("z", "0.0");

	TiXmlElement *pCorner2 = pCorners->InsertEndChild(TiXmlElement("corner2"))->ToElement();
	pCorner2->SetAttribute("x", Ogre::StringConverter::toString((mWidth->get())/2).c_str()); 
	pCorner2->SetAttribute("y", Ogre::StringConverter::toString(-(mHeight->get())/2).c_str());
	pCorner2->SetAttribute("z", "0.0");

	TiXmlElement *pCorner3 = pCorners->InsertEndChild(TiXmlElement("corner3"))->ToElement();
	pCorner3->SetAttribute("x", Ogre::StringConverter::toString(-(mWidth->get())/2).c_str()); 
	pCorner3->SetAttribute("y", Ogre::StringConverter::toString(-(mHeight->get())/2).c_str());
	pCorner3->SetAttribute("z", "0.0");

	//*destination*
    TiXmlElement *pDestination = pNode->InsertEndChild(TiXmlElement("destination"))->ToElement();
	if(mConnected)
	{
		pDestination->SetAttribute("zone", mConnected->mParentZone->getName().c_str());
		pDestination->SetAttribute("portal",mConnected->getName().c_str());
	}


	return pNode;
    
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

//----------------------------------------------------------------------------------------
bool PortalEditor::carveTerrainTunnel()
{
	try
	{

	if(!mTerrainCut)
	{
		mTerrainCut = new TerrainCut();
		mTerrainCut->create(getName(),this->getDerivedPosition(),this->getDerivedOrientation(),mWidth->get(),mHeight->get());
		//this->getNode()->attachObject(mTerrainCut->mStencil);
		OgitorsRoot::getSingletonPtr()->GetSceneManager()->getRootSceneNode()->attachObject(mTerrainCut->mStencil);
	}
	else
	{
		mTerrainCut->update(this->getDerivedPosition(),this->getDerivedOrientation(),mWidth->get(),mHeight->get());
	}
	}
	catch(Ogre::Exception &e)
	{
		QMessageBox::information(QApplication::activeWindow(),"qtOgitor", "ARRRGH...",  QMessageBox::Ok);
	}

	
	return true;
}
//----------------------------------------------------------------------------------------
void TerrainCut::create(Ogre::String name, Ogre::Vector3 position,Ogre::Quaternion orientation,Ogre::Real width,Ogre::Real height)
{
	//create mesh for stencil cut:
	Ogre::ResourceManager::ResourceCreateOrRetrieveResult result = Ogre::MaterialManager::getSingletonPtr()->createOrRetrieve("MZP_StencilMat", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	
	if(result.second)
	{   //TODO: don't neeed member var just use local, we can use "MZP_StencilMat" elsewhere
		mBlendMaterial = result.first;
		mBlendMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(false);/*//should be false, true for debugging only
		mBlendMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(Ogre::ColourValue(1,1,0));//colours just for debugging
		mBlendMaterial->getTechnique(0)->getPass(0)->setDiffuse(Ogre::ColourValue(1,0,1));*/
	}
	mStencil = OgitorsRoot::getSingletonPtr()->GetSceneManager()->createManualObject(name + "Stencil");
	mStencil->begin("MZP_StencilMat", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	for(int p = 0;p<9;++p)
	{
		mStencil->position(Ogre::Vector3(0,0,0));
		mStencil->textureCoord(0.0,1.0);
	}
	mStencil->quad(0,1,8,7);
	mStencil->quad(1,2,3,8);
	mStencil->quad(8,3,4,5);
	mStencil->quad(7,8,5,6);
	mStencil->end();
	update(position,orientation,width,height);


	ITerrainEditor * terrain = OgitorsRoot::getSingletonPtr()->GetTerrainEditor();
	Ogre::Vector3 hitpoint;
	Ogre::Ray ray(position, orientation * Ogre::Vector3::UNIT_Z );
	if(terrain->hitTest(ray,&hitpoint))
	{
		Ogre::ManualObject* manual = OgitorsRoot::getSingletonPtr()->GetSceneManager()->createManualObject(name + "DebugRay");
		manual->begin("PortalOutlineMaterial",Ogre::RenderOperation::OT_LINE_LIST);
		manual->position(position);
		manual->position(hitpoint);
		manual->end();

		OgitorsRoot::getSingletonPtr()->GetSceneManager()->getRootSceneNode()->attachObject(manual);
	}
	//OgitorsRoot::getSingletonPtr()->GetSceneManager()->getRootSceneNode()->attachObject(mStencil);
}
//----------------------------------------------------------------------------------------
bool TerrainCut::update(Ogre::Vector3 position,Ogre::Quaternion orientation,Ogre::Real width,Ogre::Real height)
{
		//retrieve terrain
	ITerrainEditor * terrain = OgitorsRoot::getSingletonPtr()->GetTerrainEditor();
	if(!terrain)return false; //no terrain 
	//calc portal corners & midpoints in local space
	
	std::vector<Ogre::Vector3> points;
	std::vector<Ogre::Vector2> tex;
	const Ogre::Real left = -width/2;
	const Ogre::Real right = width/2;
	const Ogre::Real top = height/2;
	const Ogre::Real bottom = -height/2;
	
	points.clear();//why didn't i just use a fixed array... TODO
	points.push_back(Ogre::Vector3(left,top,0.0));//1st point top left
	points.push_back(Ogre::Vector3(0.0,top,0.0));//top middle
	points.push_back(Ogre::Vector3(right,top,0.0));//top right
	points.push_back(Ogre::Vector3(right,0.0,0.0));//mid right
	points.push_back(Ogre::Vector3(right,bottom,0.0));//bottom right
	points.push_back(Ogre::Vector3(0.0,bottom,0.0));//bottom middle
	points.push_back(Ogre::Vector3(left,bottom,0.0));//bottom left
	points.push_back(Ogre::Vector3(left,0.0,0.0));//middle left
	points.push_back(Ogre::Vector3(0.0,0.0,0.0));//centre

	tex.push_back(Ogre::Vector2(0.0,0.0));//1st point top left
	tex.push_back(Ogre::Vector2(0.5,0.0));//top middle
	tex.push_back(Ogre::Vector2(1.0,0.0));//top right
	tex.push_back(Ogre::Vector2(1.0,0.5));//mid right
	tex.push_back(Ogre::Vector2(1.0,1.0));//bottom right
	tex.push_back(Ogre::Vector2(0.5,1.0));//bottom middle
	tex.push_back(Ogre::Vector2(0.0,1.0));//bottom left
	tex.push_back(Ogre::Vector2(0.0,0.5));//middle left
	tex.push_back(Ogre::Vector2(0.5,0.5));//centre
	
	std::vector<Ogre::Vector3>::iterator point;
	std::vector<Ogre::Vector2>::iterator texture;
	
	//project points onto terrain
	for(point = points.begin();point!=points.end();++point)
	{
		//apply orientation
		(*point) =   orientation*(*point);
		//convert to world coords
		(*point) =   position + (*point);
		//do ray cast for terrain hit
		Ogre::Ray ray((*point), orientation * Ogre::Vector3::UNIT_Z );
		if(!terrain->hitTest(ray,&(*point)))
		{return false;}//if no hit, abort
	}
	
	mStencil->beginUpdate(0);//(mBlendMaterial->getName(), Ogre::RenderOperation::OT_TRIANGLE_LIST);
	//update points
	for(point = points.begin(),texture = tex.begin();point!=points.end();++point,++texture)
	{
		mStencil->position((*point));
		mStencil->textureCoord((*texture).x,(*texture).y);
	}

	mStencil->quad(0,1,8,7);
	mStencil->quad(1,2,3,8);
	mStencil->quad(8,3,4,5);
	mStencil->quad(7,8,5,6);

	mStencil->end();

	//create tunnel mesh:

	//set up portal for tunnel:
}
//----------------------------------------------------------------------------------------
