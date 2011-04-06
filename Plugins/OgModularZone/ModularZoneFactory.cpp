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
#include "ModularZoneFactory.h"
#include "ModularZoneEditor.h"
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QStringList>
#include "tinyxml.h"

using namespace Ogitors;
using namespace MZP;
//-----------------------------------------------------------------------------------------
ModularZoneFactory::ModularZoneFactory(OgitorsView *view) : CNodeEditorFactory(view),
															mZoneType(""),
															mCurrentZoneTemplate(-1),
															mZoneListWidget(0)
{
    mTypeName = "Modular Zone Object";
    mEditorType = ETYPE_CUSTOM_MANAGER ;//was ETYPE_MOVABLE. this should make saving work
    mAddToObjectList = false;//use the dragndrop widget
    mRequirePlacement = true;
    mIcon = "../Plugins/Icons/zone.svg";//"Icons/zone.svg";
    mCapabilities = CAN_MOVE | CAN_ROTATE | CAN_CLONE | CAN_DELETE  | CAN_FOCUS | CAN_DRAG | CAN_UNDO | CAN_ACCEPTCOPY;

	AddPropertyDefinition("zonetemplate","Zone::Description File","XML file that defines this zone",PROP_INT, true, false);
	AddPropertyDefinition("portalcount","Zone::Portal Count","Number of portals in this zone",PROP_INT, true, false);
	AddPropertyDefinition("meshname","Zone::Mesh Name","Enclosing mesh of this zone",PROP_STRING, true, false);

}
//-----------------------------------------------------------------------------------------
CBaseEditor *ModularZoneFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
	ModularZoneEditor *object = OGRE_NEW ModularZoneEditor(this);

    OgitorsPropertyValueMap::iterator ni;

    if ((ni = params.find("init")) != params.end())
    {
		OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
		value.val = Ogre::Any(CreateUniqueID("ModularZone"));
		params["name"] = value;
		params.erase(ni);
    }

    object->createProperties(params);
	object->mParentEditor->init(*parent);

    mInstanceCount++;
    return object;
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory* ModularZoneFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW ModularZoneFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------

void ModularZoneFactory::loadZoneTemplates(void)
{
	mZoneTemplates.clear();
	if(OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
	{
		//scene is loaded, we can get the project directories

		//TODO: parse all the .zone files from the project directory
		//if I had a custom zone resmanager I could get them this way
		//pList = Ogre::ResourceGroupManager::getSingleton().findResourceNames(PROJECT_RESOURCE_GROUP,"*.zone",false);
		//maybe TODO: create .zone resourcemanager

		//Because my.zone files are not a resource I have to manaully get them:
		Ogitors::PROJECTOPTIONS* opt = OgitorsRoot::getSingletonPtr()->GetProjectOptions();

		Ogre::StringVectorPtr pList;
		Ogre::StringVector::iterator itr;
		QStringList list;
 
		for (itr = opt->ResourceDirectories.begin();itr!=opt->ResourceDirectories.end();++itr)
		{

			QString path((*itr).c_str());
			path.remove(0,3);
			if(path.startsWith("./"))
			{
				//QDirIterator won't play nice with "./dirname/subdir"
				//so we make it absolute
				QString projectpath(opt->ProjectDir.c_str());
				path.remove(0,2);
				path = projectpath + path;
				
			}

			if(path.startsWith("../"))
			{
				//fixes a problem with some paths
				QString projectpath(opt->ProjectDir.c_str());
				path = projectpath + path;
				
			}
			QDirIterator directory_walker(path, QDir::Files| QDir::NoSymLinks);

			while(directory_walker.hasNext())
			{
				// then we tell our directory_walker object to explicitly take next element until the loop finishes
				directory_walker.next();
				if(directory_walker.fileInfo().completeSuffix() == "zone")
				{
					list<<(directory_walker.filePath());
				}
			}
		}

		int key = 0;
		QStringList::iterator zonefile;
		for(zonefile = list.begin();zonefile!=list.end();++zonefile)
		{
			//load the .zone XML file and get the zone info
			//.
			ZoneInfo zone = this->_loadZoneDescription((*zonefile).toStdString());
			if(!zone.mName.empty())
			{
				//add to map
				mZoneTemplates.insert(ZoneInfoMap::value_type(key,zone));
				++key;
			}
		}		
	}
}
//-----------------------------------------------------------------------------------------
void ModularZoneFactory::setZoneType(Ogre::String zone)
{
	mZoneType = zone;
}
//-----------------------------------------------------------------------------------------
void ModularZoneFactory::setCurrentZoneTemplate(int key)
{ 
	//sets the current template
	std::map<int,ZoneInfo>::iterator value;
	value = mZoneTemplates.find(key);//check it is a valid key
	if(value!=mZoneTemplates.end())mCurrentZoneTemplate = key;
	else mCurrentZoneTemplate = -1; //if not, set to -1

}
//-----------------------------------------------------------------------------------------
int ModularZoneFactory::getCurrentZoneTemplate(void)
{
	return mCurrentZoneTemplate;//returns the current template or  -1 if none selected
}
//-----------------------------------------------------------------------------------------
ZoneInfo* ModularZoneFactory::getZoneTemplate(int key)
{
	std::map<int,ZoneInfo>::iterator value;
	value = mZoneTemplates.find(key);//returns the Zone matching param key
	if(value!=mZoneTemplates.end())return &((*value).second);
	else return 0;
}
//-----------------------------------------------------------------------------------------
const ZoneInfoMap ModularZoneFactory::getZoneTemplateMap(void)
{
	return mZoneTemplates;
}
//-----------------------------------------------------------------------------------------

int ModularZoneFactory::addZoneTemplate(ZoneInfo zone)
{
	int key = 0;
	if (!mZoneTemplates.empty())
	{
		key = mZoneTemplates.rbegin()->first+1;
	}

	mZoneTemplates.insert(ZoneInfoMap::value_type(key,zone));
	this->mZoneListWidget->addZone(key);
	return key;	
}
//-----------------------------------------------------------------------------------------
void ModularZoneFactory::updateZoneListWidget(int key)
{
	//I really would prefer not to have this function 
	//but I don't want to add a dependency on the widget to the editor
	//This function calls the ZoneWidget's update function
	if(mZoneListWidget)
	{
		mZoneListWidget->updateZoneInfo(key);
	}
}
//-----------------------------------------------------------------------------------------
ZoneInfo ModularZoneFactory::_loadZoneDescription(Ogre::String filename)
{
	ZoneInfo zone;
		
	zone.mName = OgitorsUtils::ExtractFileName(filename);

	Ogre::String meshname,short_desc,long_desc;
	bool flag = true;
	int portal_count = 0; //number of portals to the zone
	const char* pFilename = filename.c_str();

	TiXmlDocument doc(pFilename);
	if (!doc.LoadFile()) return zone;

	TiXmlElement* pElem,*pRoot;

	//<zonedescription 	mesh="room1" portals="4" short="a small room" long="a 5 x 3l room with 4 portals."	>

	// block: zone header
	{
		//pElem=hDoc.FirstChildElement().Element();
		// should always have a valid root but handle gracefully if it doesn't
		pElem = doc.FirstChildElement("zonedescription");
		if (!pElem)
		{ 
			//return an empty ZoneInfo to indicate failure
			zone.mMesh = "";
			zone.mName = "";
			zone.mPortalCount = 0;
			zone.mShortDesc = "";
			zone.mLongDesc = "";
			return zone;
		}

		// save this for later
		pRoot = pElem;

		//get mesh name
		if(pElem->Attribute("mesh"))meshname = pElem->Attribute("mesh");
		else 
		{ 
			//return an empty ZoneInfo to indicate failure
			zone.mMesh = "";
			zone.mName = "";
			zone.mPortalCount = 0;
			zone.mShortDesc = "";
			zone.mLongDesc = "";
			return zone;
		}

		zone.mMesh = meshname;
		//get number of portals
		if(pElem->QueryIntAttribute("portals",&portal_count)!=TIXML_SUCCESS)
		{ 
			//return an empty ZoneInfo to indicate failure
			zone.mMesh = "";
			zone.mName = "";
			zone.mPortalCount = 0;
			zone.mShortDesc = "";
			zone.mLongDesc = "";
			return zone;
		}
		zone.mPortalCount = portal_count;

		//get short description
		if(pElem->Attribute("short"))short_desc = pElem->Attribute("short");
		else 
		{ 
			//return an empty ZoneInfo to indicate failure
			zone.mMesh = "";
			zone.mName = "";
			zone.mPortalCount = 0;
			zone.mShortDesc = "";
			zone.mLongDesc = "";
			return zone;
		}
		zone.mShortDesc = short_desc;
		//get long description
		if(pElem->Attribute("long"))long_desc = pElem->Attribute("long");
		else 
		{ 
			//return an empty ZoneInfo to indicate failure
			zone.mMesh = "";
			zone.mName = "";
			zone.mPortalCount = 0;
			zone.mShortDesc = "";
			zone.mLongDesc = "";
			return zone;
		}
		zone.mLongDesc = long_desc;
	}

	// block: portals
	{
		pElem=pRoot->FirstChildElement( "portal" );

		//get the portal factory:
		CBaseEditorFactory* portalFactory = OgitorsRoot::getSingletonPtr()->GetEditorObjectFactory("MZ Portal Object");
		
		int count = 0;
		for( pElem; pElem; pElem=pElem->NextSiblingElement("portal"))
		{
			PortalInfo portal;
			Ogre::Vector3 position;
			Ogre::Quaternion orientation;
			
			TiXmlElement* pData;
			//get the portal info:
			//portal dimensions
			Ogre::Real width = 0;
			Ogre::Real height = 0;
			if(pElem->QueryFloatAttribute("width",&width)!=TIXML_SUCCESS)width = 2.0;//on failure set to default
			if(pElem->QueryFloatAttribute("height",&height)!=TIXML_SUCCESS)height = 2.0;//on failure set to default
			//position
			pData = pElem->FirstChild("position")->ToElement();
			if (pData)
			{
				//TODO: error handling - default values will not work
				if(pData->QueryFloatAttribute("x",&position.x)!=TIXML_SUCCESS)position.x=0.0f;//on failure ???
				if(pData->QueryFloatAttribute("y",&position.y)!=TIXML_SUCCESS)position.y=0.0f;//on failure ???
				if(pData->QueryFloatAttribute("z",&position.z)!=TIXML_SUCCESS)position.z=0.0f;//on failure ???
			}
			//orientation
			pData = pElem->FirstChild("orientation")->ToElement();
			if (pData)
			{
				
				//TODO: error handling - default values will not work
				if(pData->QueryFloatAttribute("x",&orientation.x)!=TIXML_SUCCESS)orientation.x=0.0f;//on failure ???
				if(pData->QueryFloatAttribute("y",&orientation.y)!=TIXML_SUCCESS)orientation.y=0.0f;//on failure ???
				if(pData->QueryFloatAttribute("z",&orientation.z)!=TIXML_SUCCESS)orientation.z=0.0f;//on failure ???
				if(pData->QueryFloatAttribute("w",&orientation.w)!=TIXML_SUCCESS)orientation.w=1.0f;//on failure ???
			}

			portal.mHeight = height;
			portal.mWidth = width;
			portal.mPosition = position;
			portal.mOrientation = orientation;
			zone.mPortals.push_back(portal);
		}
	}

	return zone;
}
//-----------------------------------------------------------------------------------------