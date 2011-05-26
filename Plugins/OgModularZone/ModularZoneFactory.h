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

#ifndef MODULAR_ZONE_FACTORY_H
#define MODULAR_ZONE_FACTORY_H

#include "OgitorsPrerequisites.h"
#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "NodeEditor.h"
#include "ModularZonePlugin.h"
#include "ZoneInfo.h"
#include "ZoneListWidget.hxx"

using namespace Ogitors;

namespace MZP
{

	class PluginExport ModularZoneFactory: public CNodeEditorFactory
    {
    public:
        /** @copydoc CBaseEditorFactory::CBaseEditorFactory() */
        ModularZoneFactory(OgitorsView *view = 0);
        /** @copydoc CBaseEditorFactory::~CBaseEditorFactory() */
        virtual ~ModularZoneFactory() {};
        /** @copydoc CBaseEditorFactory::CreateObject(CBaseEditor **, OgitorsPropertyValueMap &) */
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditorFactory::GetPlaceHolderName() */
        virtual Ogre::String GetPlaceHolderName() {return "scbMarker.mesh";};

		CBaseEditorFactory* duplicate(OgitorsView *view);

		void setZoneType(Ogre::String);

		void setZoneListWidget(ZoneListWidget* widget){mZoneListWidget = widget;}
		void loadZoneTemplates(void);//load the available designs for the current project
		void setCurrentZoneTemplate(int key);//sets the current template
		int getCurrentZoneTemplate(void);//returns the current template or  -1 if none selected
		ZoneInfo* getZoneTemplate(int key);//returns the Zone matching param key
		const ZoneInfoMap getZoneTemplateMap(void);
		int addZoneTemplate(ZoneInfo);
		void updateZoneListWidget(int);
	private:
		Ogre::String mZoneType;//(for future use)
		int mCurrentZoneTemplate;// key of current design. -1 = none selected
		ZoneInfoMap mZoneTemplates; //a map of available zone designs
		ZoneListWidget* mZoneListWidget; 

		ZoneInfo _loadZoneDescription(Ogre::String filename);
    };
}

#endif