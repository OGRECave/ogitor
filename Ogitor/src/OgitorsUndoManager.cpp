/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2013 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
///
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
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "BaseEditor.h"
#include "OgitorsUndoManager.h"
#include "DefaultEvents.h"
#include "EventManager.h"

#include "tinyxml.h"

using namespace Ogitors;

template<> OgitorsUndoManager* Ogitors::Singleton<OgitorsUndoManager>::ms_Singleton = 0;

//----------------------------------------------------------------------------------
PropertyUndo::PropertyUndo(OgitorsPropertySet *set, OgitorsPropertyBase *property)
{
    assert(set->getOwnerData().mOwnerType == PROPSETOWNER_EDITOR);
    mObjectID = static_cast<CBaseEditor*>(set->getOwnerData().mOwnerPtr)->getObjectID();
    mDescription = static_cast<CBaseEditor*>(set->getOwnerData().mOwnerPtr)->getName();
    mSetType = set->getType();
    mPropertyName = property->getName();
    mValue.propType = property->getType();
    if(mValue.propType == PROP_UNSIGNED_LONG && mPropertyName == "parent")
        mValue.val = Ogre::Any((unsigned long)((OgitorsParentProperty*)property)->getOld()->getObjectID());
    else
        mValue.val = property->getOldValue();
    mDescription += "'s " + property->getDefinition()->getDisplayName() + " change";
}
//----------------------------------------------------------------------------------
bool PropertyUndo::apply()
{
    CBaseEditor *object = OgitorsRoot::getSingletonPtr()->FindObject(mObjectID);
    if(!object)
        return false;

    if(mValue.propType == PROP_UNSIGNED_LONG && mPropertyName == "parent")
    {
        CBaseEditor *parent = OgitorsRoot::getSingletonPtr()->FindObject(Ogre::any_cast<unsigned long>(mValue.val));
        if(!parent)
            return false;

        mValue.val = Ogre::Any((unsigned long)parent);
    }

    OgitorsPropertyValueMap map;
    map.insert(OgitorsPropertyValueMap::value_type(mPropertyName, mValue));
    if(mSetType == PROPSET_OBJECT)
    {
        object->getProperties()->setValueMap(map);
        return true;
    }
    else if(mSetType == PROPSET_CUSTOM)
    {
        object->getCustomProperties()->setValueMap(map);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
ObjectCreationUndo::ObjectCreationUndo(CBaseEditor *object)
{
    mObjectID = object->getObjectID();
    mDescription = "Create " + object->getName();
}
//----------------------------------------------------------------------------------
bool ObjectCreationUndo::apply()
{
    CBaseEditor *object = OgitorsRoot::getSingletonPtr()->FindObject(mObjectID);
    if(!object)
        return false;
    
    OgitorsRoot::getSingletonPtr()->DestroyEditorObject(object, true, true);
    return true;
}
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
ObjectDeletionUndo::ObjectDeletionUndo(CBaseEditor *object)
{
    mTypeName = object->getTypeName();
    mParentObjectID = 0xFFFFFFFF;
    if(object->getParent())   
        mParentObjectID = object->getParent()->getObjectID();
    
    mCustomProperties = OGRE_NEW OgitorsCustomPropertySet();

    object->getPropertyMap(mObjectProperties);
    object->getCustomProperties()->cloneSet(*mCustomProperties);
}
//----------------------------------------------------------------------------------
ObjectDeletionUndo::~ObjectDeletionUndo()
{
    OGRE_DELETE mCustomProperties;
}
//----------------------------------------------------------------------------------
bool ObjectDeletionUndo::apply()
{
    CBaseEditor *object;
    CBaseEditor *parent = OgitorsRoot::getSingletonPtr()->FindObject(mParentObjectID);

    object = OgitorsRoot::getSingletonPtr()->CreateEditorObject(parent, mTypeName, mObjectProperties, true, true);
    if(object)
    {
        object->getCustomProperties()->initFromSet(*mCustomProperties);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
CustomSetRebuiltUndo::CustomSetRebuiltUndo(OgitorsCustomPropertySet *set)
{
    assert(set->getOwnerData().mOwnerType == PROPSETOWNER_EDITOR);
    CBaseEditor *object = static_cast<CBaseEditor*>(set->getOwnerData().mOwnerPtr);
    mObjectID = object->getObjectID();
    
    mCustomProperties = OGRE_NEW OgitorsCustomPropertySet();
    object->getCustomProperties()->cloneSet(*mCustomProperties);

    mDescription = object->getName() + " Custom Property Set Change";
}
//----------------------------------------------------------------------------------
CustomSetRebuiltUndo::~CustomSetRebuiltUndo()
{
    OGRE_DELETE mCustomProperties;
}
//----------------------------------------------------------------------------------
bool CustomSetRebuiltUndo::apply()
{
    CBaseEditor *object = OgitorsRoot::getSingletonPtr()->FindObject(mObjectID);

    if(object)
    {
        object->getCustomProperties()->initFromSet(*mCustomProperties);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
UndoCollection::UndoCollection(const Ogre::String& desc)
{
    mDescription = desc;
    mBuffer.clear();
}
//----------------------------------------------------------------------------------
UndoCollection::~UndoCollection()
{
    for(unsigned int i = 0;i < mBuffer.size();i++)
    {
        OGRE_DELETE mBuffer[i];
    }
    mBuffer.clear();
}
//----------------------------------------------------------------------------------
void UndoCollection::addUndo(OgitorsUndoBase *undo)
{
    mBuffer.push_back(undo);
}
//----------------------------------------------------------------------------------
bool UndoCollection::apply()
{
    bool result = true;
    for(int i = (int)mBuffer.size() - 1;i >= 0;--i)
    {
        result &= mBuffer[i]->apply();
    }

    return result;
}
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
OgitorsUndoManager::OgitorsUndoManager() :
mCurrentIndex(0), mListeningActive(false)
{
    mBuffer.clear();
}
//----------------------------------------------------------------------------------  
OgitorsUndoManager::~OgitorsUndoManager()
{
    Clear();
}
//----------------------------------------------------------------------------------
void OgitorsUndoManager::Clear()
{
    for(unsigned int i = 0;i < mBuffer.size();i++)
    {
        OGRE_DELETE mBuffer[i];
    }

    mBuffer.clear();
    
    for(unsigned int i = 0;i < mCollections.size();i++)
    {
        OGRE_DELETE mCollections[i];
    }

    mCollections.clear();

    mListeningActive = false;
    mCurrentIndex = 0;

    std::string desc = "";
    UndoManagerNotificationEvent evt1(0, false, desc);
    UndoManagerNotificationEvent evt2(1, false, desc);
    EventManager::getSingletonPtr()->sendEvent(this, 0, &evt1);
    EventManager::getSingletonPtr()->sendEvent(this, 0, &evt2);
}
//----------------------------------------------------------------------------------
void OgitorsUndoManager::AddUndo(OgitorsUndoBase *undo)
{
    if(undo == 0)
        return;

    if(mListeningActive)
        mCollections[mCollections.size() - 1]->addUndo(undo);
    else
    {
        if(mCurrentIndex < mBuffer.size())
        {
            for(unsigned int i = mCurrentIndex;i < mBuffer.size();i++)
            {
                OGRE_DELETE mBuffer[i];
            }
            mBuffer.erase(mBuffer.begin() + mCurrentIndex, mBuffer.end());
        }
        mBuffer.push_back(undo);
        ++mCurrentIndex;
		
		std::string desc=undo->getDescription();

        UndoManagerNotificationEvent evt(0, true, desc);
        EventManager::getSingletonPtr()->sendEvent(this, 0, &evt);
    }
}
//----------------------------------------------------------------------------------
void OgitorsUndoManager::Undo()
{
    --mCurrentIndex;
    BeginCollection(mBuffer[mCurrentIndex]->getDescription());
    mBuffer[mCurrentIndex]->apply();
    OGRE_DELETE mBuffer[mCurrentIndex];
    mBuffer[mCurrentIndex] = EndCollection();
    if(mBuffer[mCurrentIndex] == 0)
    {
        mBuffer.erase(mBuffer.begin() + mCurrentIndex);
    }

    std::string desc = GetUndoString();
    UndoManagerNotificationEvent evt(0, CanUndo(), desc);
    EventManager::getSingletonPtr()->sendEvent(this, 0, &evt);
}
//----------------------------------------------------------------------------------
void OgitorsUndoManager::Redo()
{
    BeginCollection(mBuffer[mCurrentIndex]->getDescription());
    mBuffer[mCurrentIndex]->apply();
    OGRE_DELETE mBuffer[mCurrentIndex];
    mBuffer[mCurrentIndex] = EndCollection();
    if(mBuffer[mCurrentIndex] != 0)
        ++mCurrentIndex;
    else
        mBuffer.erase(mBuffer.begin() + mCurrentIndex);

    std::string desc = GetRedoString();
    UndoManagerNotificationEvent evt(1, CanRedo(), desc);
    EventManager::getSingletonPtr()->sendEvent(this, 0, &evt);
}
//----------------------------------------------------------------------------------
void OgitorsUndoManager::BeginCollection(const Ogre::String& desc)
{
    UndoCollection *collection = OGRE_NEW UndoCollection(desc);
    
    mCollections.push_back(collection);
    mListeningActive = true;
}
//----------------------------------------------------------------------------------
void OgitorsUndoManager::BeginCollection(UndoCollection *collection)
{
    if(collection)
    {    
        mCollections.push_back(collection);
        mListeningActive = true;
    }
}
//----------------------------------------------------------------------------------
UndoCollection *OgitorsUndoManager::EndCollection(bool add, bool ignore)
{
    if(mCollections.size() < 1) 
        return 0;

    UndoCollection *collection = mCollections[mCollections.size() -1];
    mCollections.erase(mCollections.end() - 1);
    mListeningActive = (mCollections.size() > 0);
    if(collection->isEmpty() || ignore)
    {
        OGRE_DELETE collection;
        collection = 0;
    }
    else if(add)
        AddUndo(collection);

    return collection;
}
//----------------------------------------------------------------------------------
void OgitorsUndoManager::OnPropertyRemoved(OgitorsPropertySet* set, OgitorsPropertyBase* property) 
{
}
//----------------------------------------------------------------------------------
void OgitorsUndoManager::OnPropertyAdded(OgitorsPropertySet* set, OgitorsPropertyBase* property)
{
}
//----------------------------------------------------------------------------------
void OgitorsUndoManager::OnPropertyChanged(OgitorsPropertySet* set, OgitorsPropertyBase* property) 
{
    if(mListeningActive)
    {
        PropertyUndo *undo = OGRE_NEW PropertyUndo(set, property);
        AddUndo(undo);
    }
}
//----------------------------------------------------------------------------------
void OgitorsUndoManager::OnPropertySetRebuilt(OgitorsPropertySet* set)
{
}
//----------------------------------------------------------------------------------
