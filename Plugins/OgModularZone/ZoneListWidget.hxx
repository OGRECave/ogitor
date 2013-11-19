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

#ifndef ZONE_LIST_WIDGET_H
#define ZONE_LIST_WIDGET_H

#include <QtWidgets/QWidget>
#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"
#include "OgitorsProperty.h"

class QListWidget;
class QLineEdit;
class QToolButton;
class QLabel;

namespace MZP
{

    class ModularZoneFactory;
    struct ZoneInfo;
    //! Drag'n'drop structure
    /*!  
        A structure that is used to track object state after initiating dragging
    */
    struct DRAGDATA
    {
        Ogitors::CBaseEditor            *Object;                /** Pointer to Dragged Object */
        Ogre::String                     ObjectType;            /** Type of an object being dragged */
        Ogitors::OgitorsPropertyValueMap Parameters;            /** Information about the object */
    };

    typedef std::map<int, unsigned char *> ImageMap;
    class ZoneListWidget : public QWidget,public Ogitors::DragDropHandler
    {
        Q_OBJECT;
    public:
        explicit ZoneListWidget(QWidget* parent=0);
        virtual ~ZoneListWidget(void);
        
    public:
        void prepareView();
        void clearView();
        void addZone(int);
        void updateZoneInfo(int);

        QListWidget *getListWidget() {return listWidget;};
        
    public Q_SLOTS:
        void filterBoxTextChanged(QString text);

    protected:
        QListWidget *listWidget;
        QLineEdit   *filterBox;
        QToolButton *clearFilterButton;
        ImageMap     mIcons;
        DRAGDATA     mDragData; 

        void resizeEvent(QResizeEvent* evt);
        void _createImages(ImageMap& retlist);

        /** DragDropHandler functions**/
        virtual bool OnDragEnter();
        virtual void OnDragLeave();
        virtual bool OnDragMove (Ogre::Viewport *vp, unsigned int modifier, Ogre::Vector2& position);
        virtual void OnDragDropped(Ogre::Viewport *vp, Ogre::Vector2& position);
        virtual void OnDragWheel(Ogre::Viewport *vp, float delta);

        // Refresh 
        void onSceneLoadStateChange(Ogitors::IEvent* evt);
    };

}

#endif