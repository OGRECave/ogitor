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
#include "ModularZoneToolbar.hxx"
#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"
#include "BaseEditor.h"
#include "MultiSelEditor.h"
#include "NodeEditor.h"
#include "EntityEditor.h"
#include "ModularZoneEditor.h"

#include <QtCore/QDir>
#include <QtCore/QStringList>

#include <QtWidgets/QDialog>

using namespace Ogitors;
using namespace MZP;

ModularZoneToolbar::ModularZoneToolbar(QWidget *parent):QWidget(parent)
{

    //set up the toolbar
    setObjectName(QString::fromUtf8("Modular Zones"));
    QBoxLayout *horizontalLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    horizontalLayout->setObjectName(QString::fromUtf8("MZEHorizontalLayout"));
    horizontalLayout->setSpacing(4);
    horizontalLayout->setMargin(2);

    QAction* mActAddProps = new QAction(QObject::tr("Add ParentZone Property"), this);
    mActAddProps->setStatusTip(QObject::tr("Add Zone Custom Properties to selected object"));
    mActAddProps->setIcon( QIcon( std::string(Ogitors::Globals::OGITOR_PLUGIN_ICON_PATH + "/zoneprops.svg").c_str() ));
    mActAddProps->setCheckable(false);

    QAction* mActDesignZone = new QAction(QObject::tr("Convert to Zone"), this);
    mActDesignZone->setStatusTip(QObject::tr("Convert entity to zone for editing"));
    mActDesignZone->setIcon( QIcon( std::string(Ogitors::Globals::OGITOR_PLUGIN_ICON_PATH + "/tozone.svg").c_str()));//why do MY icons never show up?
    mActDesignZone->setCheckable(false);

    mZoneSelectionToolbar = new QToolBar(QObject::tr("Modular Zone"));
    mZoneSelectionToolbar->setIconSize(QSize(24,24));
    mZoneSelectionToolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    mZoneSelectionToolbar->setObjectName(QObject::tr("Mod. Zones"));
    mZoneSelectionToolbar->addAction(mActAddProps);
    mZoneSelectionToolbar->addAction(mActDesignZone);


    QGroupBox* ZoneSelectionGroupBox = new QGroupBox(QObject::tr("My Group Box"), this);
    ZoneSelectionGroupBox->setObjectName("toolbarbox");
    QHBoxLayout* myGroupBoxLayout = new QHBoxLayout;
    myGroupBoxLayout->addWidget(mZoneSelectionToolbar, 0, Qt::AlignLeft | Qt::AlignTop);
    myGroupBoxLayout->setSpacing(0);
    myGroupBoxLayout->setMargin(4);

    QObject::connect(mActAddProps, SIGNAL(triggered()), this, SLOT(addZoneProperties()));
    QObject::connect(mActDesignZone, SIGNAL(triggered()), this, SLOT(createZone()));

    horizontalLayout->addWidget(ZoneSelectionGroupBox);
    ZoneSelectionGroupBox->setLayout(myGroupBoxLayout);

}
//----------------------------------------------------------------------------------------
ModularZoneToolbar::~ModularZoneToolbar(void)
{
}
//----------------------------------------------------------------------------------------
void ModularZoneToolbar::addZoneProperties(void)
{
    //adds necessary custom properties to the selected object

    if(OgitorsRoot::getSingletonPtr()->IsSceneLoaded())//check before calling GetSelection
    {
        Ogitors::CMultiSelEditor* obj = OgitorsRoot::getSingletonPtr()->GetSelection();


        if(obj && !obj->isEmpty())
        {
        //TODO:
        /*It would be cool if a drop down option box could be added containing all the
        zones. However, whenever a new zone was added, the options of every object with
        this custom  property would have to be updated*/

        /*Adding physics props here would be handy - then I could select NONE, STATIC or DYNAMIC
        for an object. Also collision shape type. But how would I attach info about a collison shape
        to use. I wouldn't want to use BtOgres create box for example, I would want to define my own
        box that conforms to the shape better.*/

            Ogitors::ObjectVector objects;
            obj->getSelection(objects);

            for(unsigned int i = 0;i < objects.size();i++)
            {
                if(objects[i]->getTypeName() != "Modular Zone")
                    continue;

                if(objects[i]->getCustomProperties()->hasProperty("ParentZone"))
                    continue;

                objects[i]->getCustomProperties()->addProperty("ParentZone",Ogitors::PROP_STRING);
            }
        }
    }
}
//----------------------------------------------------------------------------------------
void ModularZoneToolbar::createZone(void)
{
    //creates a new design zone design based on the selected entity
    //(removes the entity as well)
    OgitorsRoot* root = OgitorsRoot::getSingletonPtr();
    if(root->IsSceneLoaded())//check before calling GetSelection
    {
        Ogitors::CMultiSelEditor* obj = OgitorsRoot::getSingletonPtr()->GetSelection();


        if(obj && !obj->isEmpty())
        {

            //TODO:
            //we only want to work with a single object
            //and it must be an Entity (or something that can be converted to a mesh
            //at least - eg meshmagick object (check this ...)
            Ogitors::ObjectVector objects;
            obj->getSelection(objects);
            Ogitors::ObjectVector::iterator itr;
            itr = objects.begin();
            if((*itr)->getTypeName()=="Entity")//
            {
            //Find the mesh,position, etc from this entity
            Ogitors::CEntityEditor* ent = dynamic_cast<Ogitors::CEntityEditor*>((*itr));
            Ogitors::OgitorsPropertyValueMap entparams, zoneparams;
            ent->getPropertyMap(entparams);

            //append the .mesh extension to mesh file name
            Ogre::String meshfile = Ogre::any_cast<Ogre::String>(entparams["meshfile"].val) + ".mesh";
            OgitorsPropertyValue propMesh;
            propMesh.propType = PROP_STRING;
            propMesh.val = Ogre::Any(meshfile);

            zoneparams["meshname"] = propMesh;
            zoneparams["position"] = entparams["position"];
            zoneparams["orientation"] = entparams["orientation"];
            //set the init param to create a new object
            zoneparams["init"] = EMPTY_PROPERTY_VALUE;

            //set the zone properties to suit a new empty zone
            OgitorsPropertyValue propValue;
            propValue.propType = PROP_INT;
            propValue.val = Ogre::Any(-1);//no zone description chosen - we are creating a new one
            zoneparams["zonetemplate"] = propValue;
            propValue.propType = PROP_INT;
            propValue.val = Ogre::Any(0);//no portals yet
            zoneparams["portalcount"] = propValue;

            //destroy the entity
            root->DestroyEditorObject(ent,true);
            //create a zone with the same mesh
            ModularZoneEditor* zone = dynamic_cast<ModularZoneEditor*>(root->CreateEditorObject(0,"Modular Zone",zoneparams,true,true));
            //TODO: should AddtoTreeList be TRUE? This isn't really part of the scene, its just a new
            //zone (unfinished) design. But it may make its easier to locate if you can selected from
            //the treetlist ... hmmm
            //... At the moment yes - so child portals can be selected for moving

            //once a zone is created, set it to desgn mode true so it shows up with
            //selectable verts for hanging portals on
            zone->setDesignMode();
            }
            //else if((*itr)->getTypeName()=="Modular Zone")
            //{
            //    //toggle design mode
            //    ModularZoneEditor* zone = dynamic_cast<ModularZoneEditor*>((*itr));
            //    zone->setDesignMode(!zone->getDesignMode());
            //}

        }
    }
}
//----------------------------------------------------------------------------------------
