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

#include "Ogitors.h"
#include "ModularZoneToolbar.hxx"
#include "ModularZoneFactory.h"
#include "PortalFactory.h"
#include "ZoneListWidget.hxx"
#include <QtGui/QDockWidget>

using namespace Ogitors;
using namespace MZP;

//----------------------------------------------------------------------------
bool dllStartPlugin(void *identifier, Ogre::String& name)
{
    name = "Modular Zone Plugin";

    ModularZoneFactory* zonefactory = OGRE_NEW ModularZoneFactory();
    PortalFactory* portalfactory = OGRE_NEW PortalFactory();
    ModularZoneToolbar* toolbar = new ModularZoneToolbar();
    ZoneListWidget* widget = new ZoneListWidget();
    zonefactory->setZoneListWidget(widget);

    Ogitors::DockWidgetData data;
    data.mCaption = "Modular Zones";
    data.mHandle = widget;
    data.mIcon = "../Plugins/Icons/zone.svg";
    data.mParent = DOCK_RESOURCES;
       
    OgitorsRoot::getSingletonPtr()->RegisterEditorFactory(identifier, zonefactory);
    OgitorsRoot::getSingletonPtr()->RegisterEditorFactory(identifier, portalfactory);
    OgitorsRoot::getSingletonPtr()->RegisterToolBar(identifier,toolbar->getToolbar());
    OgitorsRoot::getSingletonPtr()->RegisterDockWidget(identifier,data);

    return true;
}
//----------------------------------------------------------------------------
bool dllGetPluginName(Ogre::String& name)
{
    name = "Modular Zone Plugin";
    return true;
}
//----------------------------------------------------------------------------
bool dllStopPlugin(void)
{
    return true;
}
//----------------------------------------------------------------------------