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

#ifndef PORTAL_EDITOR_H
#define PORTAL_EDITOR_H

#include "OgitorsPrerequisites.h"
#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "NodeEditor.h"
#include "PortalOutlineRenderable.h"

using namespace Ogitors;

namespace MZP
{
	class ModularZoneEditor;
	class TerrainCut;

	class PortalEditor : public CNodeEditor
	{
	   friend class PortalFactory;
    public:

		virtual bool isNodeType() {return false;};//Prevent CNodeEditor descendants to behave as nodes
        /** @copydoc CBaseEditor::load() */
        virtual bool      load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool      unLoad();
        /** @copydoc CBaseEditor::createProperties(OgitorsPropertyValueMap &) */
        virtual void      createProperties(OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditor::showBoundingBox(bool) */
        virtual void      showBoundingBox(bool bShow);
        /** @copydoc CBaseEditor::getAABB() */
        virtual Ogre::AxisAlignedBox getAABB() {if(mPortalOutline) return mPortalOutline->getBoundingBox();else return Ogre::AxisAlignedBox::BOX_NULL;};
		
		virtual TiXmlElement* exportDotScene(TiXmlElement *pParent);

		virtual bool getObjectContextMenu(UTFStringVector &menuitems);

		void setSelectedImpl(bool bSelected);
		bool update(float timePassed);
		
		virtual Ogre::String GetPlaceHolderName() {return "scbMarker.mesh";};
		//virtual bool isSerializable() {return false;};//zone incharge of creating portals
		
		virtual bool postSceneUpdate(Ogre::SceneManager *SceneMngr, Ogre::Camera *Camera, Ogre::RenderWindow *RenderWindow) ;

		virtual void onObjectContextMenu(int menuresult);
		
		//joins portals if within snap-to distance
		bool connectNearPortals(bool);
		//realigns this portal (and zone) to join destination portal
		bool snapTpPortal(PortalEditor* dest,bool);
		void connect(PortalEditor*);//sets the destination of this portal
		void link(PortalEditor*);//links/unlinks this portal

		static const Ogre::Real DEFAULT_WIDTH;
		static const Ogre::Real DEFAULT_HEIGHT;
		static const Ogre::Real MIN_WIDTH;
		static const Ogre::Real MIN_HEIGHT;

	protected:
		Ogre::Entity* mPlaneHandle;//needed so portals can be selected
		PortalOutlineRenderable* mPortalOutline;//renders an outline to aid user
		ModularZoneEditor* mParentZone;//zone this portal is attached to
		PortalEditor* mConnected;//pointer to the destination portal
		bool mLinked; //true when portal is linked to destination portal
		bool mFreeMove;//if true, prevents snap-to  during design mode
		TerrainCut* mTerrainCut;

		PortalEditor(Ogitors::CBaseEditorFactory *factory);
		virtual ~PortalEditor(void);	

		Ogre::Entity* _createPlane();
		void _createMaterial();
		bool carveTerrainTunnel();

		//PROPERTIES
		//OgitorsProperty<Ogre::String>* mZone; //name of the parent zone
		OgitorsProperty<Ogre::String>* mDestination; //name of destination portal 
		OgitorsProperty<Ogre::Real>* mWidth; //width of  portal 
		OgitorsProperty<Ogre::Real>* mHeight; //height of  portal 

		//SETTERS
		//bool _setZone(OgitorsPropertyBase* property, const Ogre::String& value);
		bool _setDestination(OgitorsPropertyBase* property, const Ogre::String& value);
		bool _setWidth(OgitorsPropertyBase* property, const Ogre::Real& value);
		bool _setHeight(OgitorsPropertyBase* property, const Ogre::Real& value);
	};



	class TerrainCut
	{
	public:
		TerrainCut():mStencil(0),mTunnel(0){}
		~TerrainCut(){}
		Ogre::ManualObject* mStencil;
	private:
		std::vector<Ogre::Vector3> points;
		
		Ogre::ManualObject* mTunnel;
		Ogre::MaterialPtr mBlendMaterial;

	public:
		void create(Ogre::String name, Ogre::Vector3 position,Ogre::Quaternion orientation,Ogre::Real width,Ogre::Real height);
		bool update(Ogre::Vector3 position,Ogre::Quaternion orientation,Ogre::Real width,Ogre::Real height);

	};
}

#endif