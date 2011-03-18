#include "PortalFactory.h"
#include "PortalEditor.h"
#include "ModularZoneEditor.h"
#include "OgitorsRoot.h"

using namespace Ogitors;
using namespace MZP;

PortalFactory::PortalFactory(OgitorsView *view) : CNodeEditorFactory(view)
{
	mTypeName = "MZ Portal Object";
    mEditorType = ETYPE_MOVABLE;
    mAddToObjectList = false; //at the moment, we can't new add portals in the editor...
    mRequirePlacement = true;
	mIcon =  "../Plugins/Icons/portal.svg";//"Icons/zone.svg";
    mCapabilities = CAN_MOVE | CAN_ROTATE | CAN_FOCUS | CAN_DRAG | CAN_UNDO | CAN_ACCEPTCOPY;

 
	AddPropertyDefinition("parentnode","Portal::Zone","Parent zone of this portal",PROP_STRING, true, false);
	AddPropertyDefinition("destination","Portal::Destination Portal","Portal that this one is connected to.",PROP_STRING, true, false);
	AddPropertyDefinition("width","Portal::Width","Width of the portal.",PROP_REAL, true, false);
	AddPropertyDefinition("height","Portal::Height","Height of the portal.",PROP_REAL, true, false);

	
}
//----------------------------------------------------------------------------------------
CBaseEditor *PortalFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    PortalEditor *object = OGRE_NEW PortalEditor(this);
	//TODO: store a pointer to parent in mZone
	//check it is a ModularZoneEditor first...
	if((*parent)->getTypeName()=="Modular Zone Object")
	{
		//the parent is a zone, so we can cast it and store it
		object->mParentZone = dynamic_cast<ModularZoneEditor*>(*parent);
	}

    OgitorsPropertyValueMap::iterator ni;

    if ((ni = params.find("init")) != params.end())
    {
		OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
		value.val = Ogre::Any(CreateUniqueID("MZPortal"));
		params["name"] = value;
		params.erase(ni);
    }
	
    object->createProperties(params);
    object->mParentEditor->init(*parent);
	object->mParentZone->addPortal(object);
	object->registerForPostSceneUpdates();

    mInstanceCount++;
    return object;
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory* PortalFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW PortalFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//----------------------------------------------------------------------------------------
