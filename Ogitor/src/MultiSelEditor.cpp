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
#include "NodeEditor.h"
#include "MultiSelEditor.h"
#include "OgitorsSystem.h"

using namespace Ogitors;

//--------------------------------------------------------------------------------
CMultiSelEditor::CMultiSelEditor(CBaseEditorFactory *factory, const Ogre::String& name) : CBaseEditor(factory)
{
    mUsesGizmos = true;
    mUsesHelper = false;
    mDeletionInProgress = false;

    if(name == "")
        mName->init("MultiSelection" + mOgitorsRoot->CreateUniqueID("MultiSelection",""));
    else if(OgitorsRoot::getSingletonPtr()->FindObject(name))
        mName->init("MultiSelection" + mOgitorsRoot->CreateUniqueID("MultiSelection",""));
    else
        mName->init(name);

    mSelectedObjects.clear();
    mModifyList.clear();
    mWorldAABB = Ogre::AxisAlignedBox::BOX_NULL;
    mNode = mOgitorsRoot->GetSceneManager()->getRootSceneNode()->createChildSceneNode("scbn" + mName->get());
 
    registerObjectName();

    PROPERTY("position"   ,Ogre::Vector3, Ogre::Vector3::ZERO , 0, SETTER(Ogre::Vector3, CMultiSelEditor, _setPosition));
    PROPERTY("scale"      ,Ogre::Vector3, Ogre::Vector3(1,1,1), 0, SETTER(Ogre::Vector3, CMultiSelEditor, _setScale));
    PROPERTY("orientation",Ogre::Quaternion, Ogre::Quaternion::IDENTITY, 0, SETTER(Ogre::Quaternion, CMultiSelEditor, _setOrientation));
}
//--------------------------------------------------------------------------------
CMultiSelEditor::~CMultiSelEditor()
{
    unRegisterObjectName();
}
//--------------------------------------------------------------------------------
Ogre::AxisAlignedBox CMultiSelEditor::getAABB() 
{
    Ogre::AxisAlignedBox box(mWorldAABB.getMinimum() - mWorldAABB.getCenter(), mWorldAABB.getMaximum() - mWorldAABB.getCenter());
    return box;
}
//--------------------------------------------------------------------------------
void CMultiSelEditor::showBoundingBox(bool bShow)
{
    if(mDeletionInProgress)
        return;

    NameObjectPairList::const_iterator it = mSelectedObjects.begin();
    while(it != mSelectedObjects.end())
    {
        it->second->showBoundingBox(bShow);
        it++;
    }
}
//--------------------------------------------------------------------------------
void CMultiSelEditor::_clearSelection()
{
    if(mDeletionInProgress)
        return;
    
    mDeletionInProgress = true;

    NameObjectPairList::const_iterator it = mSelectedObjects.begin();
    while(it != mSelectedObjects.end())
    {
        it->second->setSelected(false);
        it++;
    }

    mSelectedObjects.clear();
    mModifyList.clear();

    mDeletionInProgress = false;
}
//--------------------------------------------------------------------------------
void CMultiSelEditor::setSelection(CBaseEditor *object)
{
    if(mDeletionInProgress)
        return;

    _clearSelection();
    
    if(object)
       add(object);
    else
    {
        mSystem->SelectTreeItem(mOgitorsRoot->GetRootEditor());
        mSystem->PresentPropertiesView(0);
    }
}
//--------------------------------------------------------------------------------
void CMultiSelEditor::setSelection(NameObjectPairList &newselection)
{
    if(mDeletionInProgress)
        return;

    _clearSelection();

    mSelectedObjects = newselection;

    NameObjectPairList::const_iterator it = mSelectedObjects.begin();
    while(it != mSelectedObjects.end())
    {
        it->second->setSelected(true);
        it++;
    }

    _createModifyList();
}
//--------------------------------------------------------------------------------
void CMultiSelEditor::setSelection(const ObjectVector& list)
{
    if(mDeletionInProgress)
        return;

    _clearSelection();

    for(unsigned int i = 0;i < list.size();i++)
    {
        if(list[i])
        {
            list[i]->setSelected(true);
            mSelectedObjects.insert(NameObjectPairList::value_type(list[i]->getName(), list[i]));
        }
    }

    _createModifyList();
}
//--------------------------------------------------------------------------------
void CMultiSelEditor::getSelection(ObjectVector& list)
{
    list.clear();

    NameObjectPairList::const_iterator it = mSelectedObjects.begin();
    while(it != mSelectedObjects.end())
    {
        list.push_back(it->second);
        it++;
    }
}
//--------------------------------------------------------------------------------
void CMultiSelEditor::_createModifyList()
{
    mWorldAABB = Ogre::AxisAlignedBox::BOX_NULL;
    mModifyList.clear();
    NameObjectPairList::const_iterator it = mSelectedObjects.begin();
    while(it != mSelectedObjects.end())
    {
        CBaseEditor *add = it->second;
        Ogre::AxisAlignedBox box = add->getWorldAABB();
        mWorldAABB.merge(box);

        Ogre::String name = add->getName();
        if(mModifyList.find(name) == mModifyList.end()) 
            mModifyList.insert(NameObjectPairList::value_type(add->getName(), add));

        it++;
    }
    
    Ogre::String remname;
    Ogre::StringVector removeList;
    removeList.clear();


    it = mModifyList.begin();
    while(it != mModifyList.end())
    {
        remname = it->second->getParent()->getName();

        if(mModifyList.find(remname) != mModifyList.end()) 
            removeList.push_back(it->first);

        it++;
    }

    for(unsigned int x = 0;x < removeList.size(); x++)
    {
        mModifyList.erase(mModifyList.find(removeList[x]));
    }

    Ogre::Vector3 xTot = Ogre::Vector3::ZERO;
    Ogre::Vector3 yTot = Ogre::Vector3::ZERO;
    Ogre::Vector3 zTot = Ogre::Vector3::ZERO;
    int count = 0;

    it = mModifyList.begin();
    while(it != mModifyList.end())
    {
        xTot += it->second->getDerivedOrientation() * Ogre::Vector3::UNIT_X;
        yTot += it->second->getDerivedOrientation() * Ogre::Vector3::UNIT_Y;
        zTot += it->second->getDerivedOrientation() * Ogre::Vector3::UNIT_Z;
        count++;
        it++;
    }

    if(count)
    {
        xTot /= ((float)count);
        yTot /= ((float)count);
        zTot /= ((float)count);
        Ogre::Vector3 normal = Ogre::Vector3::UNIT_Z;
        Ogre::Quaternion q;
        q.FromAxes(xTot,yTot,zTot);

        mNode->setPosition(mWorldAABB.getCenter());
        mNode->setOrientation(q);
    }
    else
    {
        mNode->setPosition(Ogre::Vector3::ZERO);
        mNode->setOrientation(Ogre::Quaternion::IDENTITY);
    }
    mNode->setScale(1,1,1);

    if(mSelectedObjects.size() == 0)
    {
        mSystem->SelectTreeItem(mOgitorsRoot->GetRootEditor());
        mSystem->PresentPropertiesView(0);
    }
    else if(mSelectedObjects.size() == 1)
    {
        mSystem->SelectTreeItem(mSelectedObjects.begin()->second);
        mSystem->PresentPropertiesView(mSelectedObjects.begin()->second);
    }
    else
    {
        mSystem->SelectTreeItem(this);
        mSystem->PresentPropertiesView(this);
    }
}
//--------------------------------------------------------------------------------
void CMultiSelEditor::setDerivedPosition(Ogre::Vector3 val)
{
    Ogre::Vector3 posdiff = val - mNode->_getDerivedPosition();

    mNode->setPosition(val);

    NameObjectPairList::const_iterator it = mModifyList.begin();
    while(it != mModifyList.end())
    {
        if(!it->second->getLocked())
        {
            Ogre::Vector3 pos = it->second->getDerivedPosition();
            it->second->setDerivedPosition(pos + posdiff);
        }
        it++;
    }
}
//--------------------------------------------------------------------------------
void CMultiSelEditor::setDerivedOrientation(Ogre::Quaternion val)
{
    Ogre::Quaternion qdiff = val * mNode->getOrientation().Inverse(); 

    mNode->setOrientation(val);

    Ogre::Vector3 groupPos = mNode->getPosition();

    NameObjectPairList::const_iterator it = mModifyList.begin();
    while(it != mModifyList.end())
    {
        if(!it->second->getLocked())
        {
            Ogre::Vector3 newpos = (qdiff * (it->second->getDerivedPosition() - groupPos)) + groupPos;
       
            it->second->setDerivedPosition(newpos);
 
            Ogre::Quaternion quat = it->second->getDerivedOrientation();
            it->second->setDerivedOrientation(qdiff * quat);
        }
        it++;
    }
}
//--------------------------------------------------------------------------------
bool CMultiSelEditor::_setScale(OgitorsPropertyBase* property, const Ogre::Vector3& val)
{
    if(val.x == 0.0f || val.y == 0.0f || val.z == 0.0f)
        return false;
    
    Ogre::Vector3 scale;
    Ogre::Vector3 diff = val / mNode->getScale(); 

    mNode->setScale(val);

    Ogre::Vector3 groupPos = mNode->getPosition();

    NameObjectPairList::const_iterator it = mModifyList.begin();
    while(it != mModifyList.end())
    {
        if(!it->second->getLocked())
        {
            Ogre::Vector3 newpos = it->second->getDerivedPosition() - groupPos;
            
            Ogre::Vector3 AxisX = mNode->getOrientation() * Ogre::Vector3::UNIT_X;
            Ogre::Vector3 AxisY = mNode->getOrientation() * Ogre::Vector3::UNIT_Y;
            Ogre::Vector3 AxisZ = mNode->getOrientation() * Ogre::Vector3::UNIT_Z;

            Ogre::Vector3 vPos1 = (AxisX.dotProduct(newpos) * AxisX);
            Ogre::Vector3 vPos2 = (AxisY.dotProduct(newpos) * AxisY);
            Ogre::Vector3 vPos3 = (AxisZ.dotProduct(newpos) * AxisZ);
            newpos = (vPos1 * diff.x) + (vPos2 * diff.y) + (vPos3 * diff.z) + groupPos;
           
            it->second->setDerivedPosition(newpos);

            if(it->second->getProperties()->hasProperty("scale"))
            {
                it->second->getProperties()->getValue("scale",scale);
                scale = it->second->getDerivedOrientation().Inverse() * scale;
                scale = mNode->getOrientation() * scale;
                scale *= diff;
                scale = mNode->getOrientation().Inverse() * scale;
                scale = it->second->getDerivedOrientation() * scale;
            
                it->second->getProperties()->setValue("scale", scale);
            }
        }
        it++;
    }
    return true;
}
//--------------------------------------------------------------------------------
void CMultiSelEditor::deleteObjects()
{
    mDeletionInProgress = true;

    CBaseEditor *ed = 0;

    NameObjectPairList list = mSelectedObjects;
    NameObjectPairList::iterator it = list.begin();
    
    while(it != list.end())
    {
        //First trying to get the object by name, we could use it->second too but then
        //we can never be sure if the objects parent is deleted before the object
        //and that pointer became invalid, this way we make sure object exists...
        ed = mOgitorsRoot->FindObject(it->first);

        if(ed && ed->supports(CAN_DELETE))
            mOgitorsRoot->DestroyEditorObject(ed, true);

        it++;
    }

    mSelectedObjects.clear();
    mModifyList.clear();
    mDeletionInProgress = false;
}
//--------------------------------------------------------------------------------
void CMultiSelEditor::add(CBaseEditor* object)
{
    if(mDeletionInProgress || !object || object == this)
        return;

    if(mSelectedObjects.find(object->getName()) == mSelectedObjects.end())
    {
        mSelectedObjects.insert(NameObjectPairList::value_type(object->getName(), object));

        object->setSelected(true);
        
        _createModifyList();
    }
}
//--------------------------------------------------------------------------------
void CMultiSelEditor::add(const NameObjectPairList& newselection)
{
    if(mDeletionInProgress)
        return;

    NameObjectPairList::const_iterator it = newselection.begin();
    
    while(it != newselection.end())
    {
        if(it->second && (it->second != this) && (mSelectedObjects.find(it->first) == mSelectedObjects.end()))
        {
            mSelectedObjects.insert(NameObjectPairList::value_type(it->first, it->second));

            it->second->setSelected(true);
        }
        
        it++;
    }

    _createModifyList();
}
//--------------------------------------------------------------------------------
void CMultiSelEditor::add(const ObjectVector& newselection)
{
    if(mDeletionInProgress)
        return;

    if(newselection.size() == 0)
        return;

    for(unsigned int i = 0;i < newselection.size();i++)
    {
        if(newselection[i] && (newselection[i] != this) && (mSelectedObjects.find(newselection[i]->getName()) == mSelectedObjects.end()))
        {
            mSelectedObjects.insert(NameObjectPairList::value_type(newselection[i]->getName(), newselection[i]));

            newselection[i]->setSelected(true);
        }
    }

    _createModifyList();
}
//--------------------------------------------------------------------------------
void CMultiSelEditor::add(const Ogre::StringVector& newselection)
{
    if(mDeletionInProgress)
        return;

    Ogre::StringVector::const_iterator it = newselection.begin();
    
    while(it != newselection.end())
    {
        CBaseEditor *object = mOgitorsRoot->FindObject(*it);
       
        if(object && object != this)
        {
            if(mSelectedObjects.find(object->getName()) == mSelectedObjects.end())
            {
                mSelectedObjects.insert(NameObjectPairList::value_type(object->getName(), object));

                object->setSelected(true);
            }
        }

        it++;
    }

    _createModifyList();
}
//--------------------------------------------------------------------------------
void CMultiSelEditor::remove(CBaseEditor* object)
{
    if(mDeletionInProgress || !object)
        return;

    NameObjectPairList::iterator it = mSelectedObjects.find(object->getName());

    if(it != mSelectedObjects.end())
    {
        mSelectedObjects.erase(it);
        object->setSelected(false);
        _createModifyList();
    }
}
//--------------------------------------------------------------------------------
void CMultiSelEditor::remove(const NameObjectPairList& newselection)
{
    if(mDeletionInProgress)
        return;

    NameObjectPairList::const_iterator it = newselection.begin();
    NameObjectPairList::iterator dit;
    
    while(it != newselection.end())
    {
        if(it->second && ((dit = mSelectedObjects.find(it->first)) != mSelectedObjects.end()))
        {
            mSelectedObjects.erase(dit);

            it->second->setSelected(false);
        }
        
        it++;
    }

    _createModifyList();
}
//--------------------------------------------------------------------------------
void CMultiSelEditor::remove(const ObjectVector& newselection)
{
    if(mDeletionInProgress)
        return;

    if(newselection.size() == 0)
        return;

    NameObjectPairList::iterator it;

    for(unsigned int i = 0;i < newselection.size();i++)
    {
        if(newselection[i] && ((it = mSelectedObjects.find(newselection[i]->getName())) != mSelectedObjects.end()))
        {
            mSelectedObjects.erase(it);

            newselection[i]->setSelected(false);
        }
    }

    _createModifyList();
}
//--------------------------------------------------------------------------------
void CMultiSelEditor::remove(const Ogre::StringVector& newselection)
{
    if(mDeletionInProgress)
        return;

    Ogre::StringVector::const_iterator it = newselection.begin();
    NameObjectPairList::iterator dit;
    
    while(it != newselection.end())
    {
        CBaseEditor *object = mOgitorsRoot->FindObject(*it);
       
        if(object)
        {
            if((dit = mSelectedObjects.find(object->getName())) != mSelectedObjects.end())
            {
                mSelectedObjects.erase(dit);

                object->setSelected(false);
            }
        }

        it++;
    }

    _createModifyList();
}
//--------------------------------------------------------------------------------
bool CMultiSelEditor::contains(CBaseEditor* object) const
{
    if(!object)
        return false;

    return (mSelectedObjects.find(object->getName()) != mSelectedObjects.end());
}
//--------------------------------------------------------------------------------
bool CMultiSelEditor::containsOrEqual(CBaseEditor* object) const
{
    if(!object)
        return false;
    
    if(object == this)
        return true;

    return (mSelectedObjects.find(object->getName()) != mSelectedObjects.end());
}
//--------------------------------------------------------------------------------
bool CMultiSelEditor::isSingle() const
{
    return (mSelectedObjects.size() == 1);
}
//--------------------------------------------------------------------------------
bool CMultiSelEditor::isEmpty() const
{
    return (mSelectedObjects.size() == 0);
}
//--------------------------------------------------------------------------------
CBaseEditor *CMultiSelEditor::getFirstObject()
{
    if(mSelectedObjects.size())
        return mSelectedObjects.begin()->second;

    return 0;
}
//--------------------------------------------------------------------------------
CBaseEditor *CMultiSelEditor::getAsSingle()
{
    if(mSelectedObjects.size() == 1)
        return mSelectedObjects.begin()->second;

    return this;
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//---------------CMULTISELEDITORFACTORY-------------------------------------------
//--------------------------------------------------------------------------------
CMultiSelEditorFactory::CMultiSelEditorFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "Multi Selection";
    mEditorType = ETYPE_MULTISEL;
    mIcon = "multisel.svg";
    mCapabilities = CAN_MOVE | CAN_SCALE | CAN_ROTATE | CAN_UNDO  | CAN_FOCUS | CAN_DELETE;

    AddPropertyDefinition("position", "", "The position of the object.",PROP_VECTOR3, false, false);
    AddPropertyDefinition("scale", "", "The scale of the object.",PROP_VECTOR3, false, false);
    AddPropertyDefinition("orientation", "", "The orientation of the object.",PROP_QUATERNION, false, false);

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("layer");
    it->second.setAccess(false, false);

    it = mPropertyDefs.find("name");
    it->second.setAccess(true, false);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CMultiSelEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW CMultiSelEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CMultiSelEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params) 
{
    mInstanceCount++;
    
    OgitorsPropertyValueMap::iterator ni;

    Ogre::String name = "";

    if ((ni = params.find("name")) != params.end())
    {
        name = Ogre::any_cast<Ogre::String>(ni->second.val);
    }

    CMultiSelEditor *object = OGRE_NEW CMultiSelEditor(this, name);
    object->createProperties(params);

    object->mParentEditor->init(*parent);

    return object;
}
//--------------------------------------------------------------------------------
