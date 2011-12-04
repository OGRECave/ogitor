/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2011 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
//
/// The MIT License
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////*/

#include "OgitorsPrerequisites.h"
#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "VisualHelper.h"

using namespace Ogitors;

//--------------------------------------------------------------------------------
CVisualHelper::CVisualHelper(CBaseEditor *parent) : mParent(parent), mVisible(true)
{
    mNode = parent->getNode()->createChildSceneNode("scbnh" + parent->getName());
    mNode->setInheritScale(false);
}
//--------------------------------------------------------------------------------
CVisualHelper::~CVisualHelper()
{
    mNode->getParentSceneNode()->removeAndDestroyChild(mNode->getName());
}
//--------------------------------------------------------------------------------
CEntityVisualHelper::CEntityVisualHelper(CBaseEditor *parent, bool scalesensitive) 
: CVisualHelper(parent), mScaleSensitive(scalesensitive)
{
    mHandle = 0;

    Ogre::Vector3 position;
    Ogre::Quaternion orientation;
    mParent->getProperties()->getValue("position", position);
    mParent->getProperties()->getValue("orientation", orientation);

    if(scalesensitive)
    {
        Ogre::Vector3 scale = mNode->_getDerivedScale();
        mNode->setPosition(position / scale);
    }
    else
        mNode->setPosition(position);

    mNode->setOrientation(orientation);

    CONNECT_PROPERTY_MEMFN(mParent, "parent", CEntityVisualHelper, OnParentsParentChange, mConnections[0]);
    CONNECT_PROPERTY_MEMFN(mParent, "position", CEntityVisualHelper, OnParentPositionChange, mConnections[1]);
    CONNECT_PROPERTY_MEMFN(mParent, "orientation", CEntityVisualHelper, OnParentOrientationChange, mConnections[2]);
    if(mScaleSensitive && parent->getParent())
        CONNECT_PROPERTY_MEMFN(mParent->getParent(), "scale", CEntityVisualHelper, OnParentsNodeScaleChange, mConnections[3]);
}
//--------------------------------------------------------------------------------
CEntityVisualHelper::~CEntityVisualHelper()
{
    Destroy();
}
//--------------------------------------------------------------------------------
void CEntityVisualHelper::setVisiblityFlags(unsigned int flags)
{
    if(mHandle)
    {
        mHandle->setVisibilityFlags(flags);
    }
}
//--------------------------------------------------------------------------------
void CEntityVisualHelper::SetModel(Ogre::String model)
{
    if(mHandle)
    {
        mHandle->detachFromParent();
        mHandle->_getManager()->destroyEntity(mHandle);
    }
    mHandle = OgitorsRoot::getSingletonPtr()->GetSceneManager()->createEntity(/*"scbh" + */mParent->getName(),model); 
    mHandle->setQueryFlags(QUERYFLAG_MOVABLE);
    mHandle->setCastShadows(false);
    mNode->attachObject(mHandle);
}
//--------------------------------------------------------------------------------
void CEntityVisualHelper::SetMaterial(Ogre::MaterialPtr material)
{
    if(mHandle)
        mHandle->setMaterial(material);
}
//--------------------------------------------------------------------------------
void CEntityVisualHelper::SetMaterialName(const Ogre::String& matname)
{
    if(mHandle)
        mHandle->setMaterialName(matname);
}
//--------------------------------------------------------------------------------
void CEntityVisualHelper::Show(bool bShow)
{
    mVisible = bShow;
    if(mHandle)
        mHandle->setVisible(bShow);
}
//--------------------------------------------------------------------------------
Ogre::AxisAlignedBox CEntityVisualHelper::getAABB()
{
    if(mHandle) 
        return mHandle->getBoundingBox(); 
    else 
        return Ogre::AxisAlignedBox::BOX_NULL;
}
//--------------------------------------------------------------------------------
Ogre::AxisAlignedBox CEntityVisualHelper::getWorldAABB()
{
    if(mNode)
    {
        Ogre::AxisAlignedBox box = getAABB();

        Ogre::Vector3 pos = mNode->_getDerivedPosition();

        box = Ogre::AxisAlignedBox(box.getMinimum() + pos, box.getMaximum() + pos);

        return box;
    }
    else 
        return Ogre::AxisAlignedBox::BOX_NULL;
}
//--------------------------------------------------------------------------------
void CEntityVisualHelper::Destroy()
{
    mParent = 0;

    if(mHandle)
    {
        mHandle->detachFromParent();
        mHandle->_getManager()->destroyEntity(mHandle);
        mHandle = 0;
    }
}
//--------------------------------------------------------------------------------
void CEntityVisualHelper::OnParentPositionChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    if(mScaleSensitive)
    {
        Ogre::Vector3 scale = mNode->getParentSceneNode()->_getDerivedScale();
        mNode->setPosition(Ogre::any_cast<Ogre::Vector3>(value) / scale);
    }
    else
        mNode->setPosition(Ogre::any_cast<Ogre::Vector3>(value));
}
//--------------------------------------------------------------------------------
void CEntityVisualHelper::OnParentsNodeScaleChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    if(mScaleSensitive)
    {
        Ogre::Vector3 scale = mNode->getParentSceneNode()->_getDerivedScale();
        Ogre::Vector3 position;
        mParent->getProperties()->getValue("position", position);
        mNode->setPosition(position / scale);
    }
}
//--------------------------------------------------------------------------------
void CEntityVisualHelper::OnParentOrientationChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    mNode->setOrientation(Ogre::any_cast<Ogre::Quaternion>(value));
}
//--------------------------------------------------------------------------------
void CEntityVisualHelper::OnParentsParentChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    if(mScaleSensitive)
    {
        CBaseEditor *newparent = (CBaseEditor*)Ogre::any_cast<unsigned long>(value);
        mConnections[3].disconnect();
        CONNECT_PROPERTY_MEMFN(newparent, "scale", CEntityVisualHelper, OnParentsNodeScaleChange, mConnections[3]);
    }

    Ogre::Vector3 position;
    mParent->getProperties()->getValue("position", position);
    if(mScaleSensitive)
    {
        Ogre::Vector3 scale = mNode->getParentSceneNode()->_getDerivedScale();
        mNode->setPosition(position / scale);
    }
}
//--------------------------------------------------------------------------------
