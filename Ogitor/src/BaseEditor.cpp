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
#include "tinyxml.h"
#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "OgitorsPhysics.h"
#include "OgitorsUndoManager.h"
#include "VisualHelper.h"
#include "PagingEditor.h"
#include "OBBoxRenderable.h"
#include "MultiSelEditor.h"
#include "OgitorsScriptInterpreter.h"

using namespace Ogre;

namespace Ogitors
{

    OgitorsRoot          *CBaseEditor::mOgitorsRoot = 0;
    OgitorsSystem        *CBaseEditor::mSystem = 0;
    OgitorsPhysics       *CBaseEditor::mPhysics = 0;
    Ogre::String          CBaseEditor::mOBBMaterials[3];
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::_initStatic(OgitorsRoot *root)
    {
        mOgitorsRoot = root;
        mSystem = OgitorsSystem::getSingletonPtr();
        mPhysics = OgitorsPhysics::getSingletonPtr();
        mOBBMaterials[0] = "SelectionBBMaterial";
        mOBBMaterials[1] = "HighlightBBMaterial";
        mOBBMaterials[2] = "SelectHighlightBBMaterial";
    }
    //-----------------------------------------------------------------------------------------
    CBaseEditor::CBaseEditor(CBaseEditorFactory *factory) :
    mFactory(factory), mHelper(0), 
        mBoxParentNode(0), mBBoxNode(0), mOBBoxRenderable(0), 
        mOBBoxData(AxisAlignedBox::BOX_NULL), mSceneTreeItemHandle(0), mLayerTreeItemHandle(0), 
        mScriptResourceHandle(0)
    {
        mView = factory->mView;
        mRefCount = 1;

        PropertySetOwnerData data;
        data.mOwnerType = PROPSETOWNER_EDITOR;
        data.mOwnerPtr = (void *)this;
        mProperties.setOwnerData(data);
        mCustomProperties.setOwnerData(data);

        OgitorsPropertyDefMap::iterator defi = mFactory->mPropertyDefs.find("parent");
        if (defi != mFactory->mPropertyDefs.end())
        {
            mParentEditor = OGRE_NEW OgitorsParentProperty(&(defi->second), 0, SETTER(unsigned long, CBaseEditor, _setParent));
            mProperties.addProperty(mParentEditor);
        }

        PROPERTY_PTR(mObjectID,     "object_id",    unsigned int , 0, 0, 0);
        PROPERTY_PTR(mName,         "name"     ,    Ogre::String , ""     , 0, SETTER(Ogre::String, CBaseEditor, _setName));
        PROPERTY_PTR(mLayer,        "layer"    ,    unsigned int , 0      , 0, SETTER(unsigned int, CBaseEditor, _setLayer));
        PROPERTY("typename",                        Ogre::String , getTypeName(), 0, 0);
        PROPERTY("destroyed",                       bool, false  , 0, 0);
        PROPERTY_PTR(mLoaded,       "loaded"   ,    bool         , false  , 0, SETTER(bool, CBaseEditor, _setLoaded));
        PROPERTY_PTR(mSelected,     "selected" ,    bool         , false  , 0, SETTER(bool, CBaseEditor, _setSelected));
        PROPERTY_PTR(mHighlighted,  "highlighted",  bool         , false  , 0, SETTER(bool, CBaseEditor, _setHighlighted));
        PROPERTY_PTR(mLocked,       "locked"   ,    bool         , false  , 0, SETTER(bool, CBaseEditor, _setLocked));
        PROPERTY_PTR(mModified,     "modified" ,    bool         , false  , 0, SETTER(bool, CBaseEditor, _setModified));
        PROPERTY_PTR(mUpdateScript, "updatescript", Ogre::String, "", 0, SETTER(Ogre::String, CBaseEditor, _setUpdateScript));
    
        if(usesHelper())
            PROPERTY_PTR(mShowHelper, "show_helper", bool, true, 0, SETTER(bool, CBaseEditor, _setShowHelper));

    }
    //-----------------------------------------------------------------------------------------
    CBaseEditor::~CBaseEditor()
    {
        mProperties.setValue("destroyed", true);

        if(mScriptResourceHandle && mOgitorsRoot->GetScriptInterpreter())
            mOgitorsRoot->GetScriptInterpreter()->releaseHandle(mScriptResourceHandle);

        mOgitorsRoot->RemoveObjectID(mObjectID->get());

        OGRE_DELETE mOBBoxRenderable;
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::_addRef()
    {
        ++mRefCount;
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::_release()
    {
        --mRefCount;
    }
    //-----------------------------------------------------------------------------------------
    OgitorsPropertyBase *CBaseEditor::getProperty(const Ogre::String& propname)
    {
        return mProperties.getProperty(propname);
    }
    //-----------------------------------------------------------------------------------------
    bool CBaseEditor::hasProperty(const Ogre::String& propname) 
    {
        return mProperties.hasProperty(propname);
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::setObjectID(unsigned int id)
    { 
        mObjectID->init(id); 
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::createProperties(OgitorsPropertyValueMap &params)
    {
        mProperties.initValueMap(params);
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::destroy(bool informparent)
    {
        if(informparent && getParent())
            getParent()->_removeChild(mName->get());

        getFactoryDynamic()->DestroyObject(this);
    }
    //-----------------------------------------------------------------------------------------
    EDITORTYPE CBaseEditor::getEditorType() 
    {
        return getFactoryDynamic()->mEditorType;
    }
    //-----------------------------------------------------------------------------------------
    unsigned int CBaseEditor::getTypeID() 
    {
        return getFactoryDynamic()->mTypeID;
    }
    //-----------------------------------------------------------------------------------------
    Ogre::String CBaseEditor::getTypeName() 
    {
        return getFactoryDynamic()->mTypeName;
    }
    //-----------------------------------------------------------------------------------------
    bool CBaseEditor::supports(unsigned int flags) 
    {
        return ((getFactoryDynamic()->mCapabilities & flags) != 0);
    }
    //-----------------------------------------------------------------------------------------
    OgitorWorldSectionId CBaseEditor::getDefaultWorldSection()
    {
        return (getFactoryDynamic()->mDefaultWorldSection);
    }
    //-----------------------------------------------------------------------------------------
    bool CBaseEditor::_setModified(OgitorsPropertyBase* property, const bool& bModified) 
    {
        mOgitorsRoot->ChangeSceneModified(bModified);
        return true;
    }
    //-----------------------------------------------------------------------------------------
    bool CBaseEditor::_setUpdateScript(OgitorsPropertyBase* property, const Ogre::String& scriptfile) 
    {
        unRegisterForScriptUpdates();

        if(mScriptResourceHandle != 0)
        {
            mOgitorsRoot->GetScriptInterpreter()->releaseHandle(mScriptResourceHandle);
            mScriptResourceHandle = 0;
        }

        if(!scriptfile.empty())
        {
            registerForScriptUpdates();
        }

        return true;
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::setParentImpl(CBaseEditor *oldparent, CBaseEditor *newparent)
    {
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::setHelper(CVisualHelper *objHelper) 
    {
        mHelper = objHelper;
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::_addChild(CBaseEditor *child)
    {
        mChildren.insert(NameObjectPairList::value_type(child->getName(), child));
    }
    //-----------------------------------------------------------------------------------------
    CBaseEditor* CBaseEditor::findChild(Ogre::String name, bool searchchildren)
    {
        NameObjectPairList::const_iterator i = mChildren.find(name);

        if (i != mChildren.end()) 
            return i->second;

        if(searchchildren)
        {
            NameObjectPairList::iterator iend = mChildren.end();
            for (i = mChildren.begin(); i != iend; ++i)
            {
                CBaseEditor *res = i->second->findChild(name, searchchildren);
                if(res) 
                    return res;
            }
        }
        return 0;
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::getNameList(Ogre::StringVector& list)
    {
        list.push_back(mName->get());

        NameObjectPairList::const_iterator i = mChildren.begin();
        while( i != mChildren.end()) 
        {
            i->second->getNameList(list);
            i++;
        }
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::getNodeList(ObjectVector& list)
    {
        NameObjectPairList::const_iterator i = mChildren.begin();
        while( i != mChildren.end()) 
        {
            if(i->second->getEditorType() == ETYPE_NODE)
            {
                list.push_back(i->second);
            }
            i->second->getNodeList(list);
            i++;
        }
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::destroyAllChildren()
    {
        NameObjectPairList::iterator i, iend;
        iend = mChildren.end();

        for (i = mChildren.begin(); i != iend; ++i)
        {
            i->second->destroy();
        }

        mChildren.clear();
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::loadAllChildren(bool dontLoadPaged)
    {
        NameObjectPairList::iterator i, iend;
        iend = mChildren.end();

        for (i = mChildren.begin(); i != iend; ++i)
        {
            if(!(dontLoadPaged && i->second->supports(CAN_PAGE)))
                i->second->load();

            i->second->loadAllChildren(dontLoadPaged);
        }
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::unLoadAllChildren()
    {
        NameObjectPairList::iterator i, iend;
        iend = mChildren.end();

        for (i = mChildren.begin(); i != iend; ++i)
        {
            i->second->unLoad();
        }
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::_refreshPaging()
    {
        NameObjectPairList::iterator i, iend;
        iend = mChildren.end();

        IPagingEditor *pgMan = mOgitorsRoot->GetPagingEditor();

        if(!pgMan)
            return;

        for (i = mChildren.begin(); i != iend; ++i)
        {
            pgMan->addObject(i->second);
            i->second->_refreshPaging();
        }
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::_updatePaging()
    {
        NameObjectPairList::iterator i, iend;
        iend = mChildren.end();

        IPagingEditor *pgMan = mOgitorsRoot->GetPagingEditor();

        if(!pgMan)
            return;

        pgMan->updateObjectPage(this);

        for (i = mChildren.begin(); i != iend; ++i)
        {
            i->second->_updatePaging();
        }
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::_removeChild(Ogre::String name)
    {
        NameObjectPairList::iterator i = mChildren.find(name);

        if (i != mChildren.end())
        {
            mChildren.erase(i);
        }
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::getPropertyMap(OgitorsPropertyValueMap& map)
    {
        map = mProperties.getValueMap();
        map.erase(map.find("parent"));
        map.erase(map.find("selected"));
        map.erase(map.find("modified"));
        map.erase(map.find("loaded"));
        map.erase(map.find("destroyed"));
        map.erase(map.find("highlighted"));

        getPropertyMapImpl(map);
    }
    //-----------------------------------------------------------------------------------------
    Ogre::AxisAlignedBox CBaseEditor::getWorldAABB()
    {
        Ogre::AxisAlignedBox box = getAABB();

        Ogre::Vector3 scale(1,1,1);
        Ogre::Vector3 pos(0,0,0);
        if(mParentEditor->get())
        {
            scale = mParentEditor->get()->getDerivedScale();
        }

        pos = getDerivedPosition();

        box.scale(scale);
        box = Ogre::AxisAlignedBox(box.getMinimum() + pos, box.getMaximum() + pos);

        return box;
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::setDerivedPosition(Ogre::Vector3 val)
    {
        if(getParent())
        {
            Ogre::Quaternion qParent = getParent()->getDerivedOrientation().Inverse();
            Ogre::Vector3 vParent = getParent()->getDerivedPosition();
            Ogre::Vector3 newPos = (val - vParent);
            newPos /= getParent()->getDerivedScale();
            val = qParent * newPos;
        }
        if(mProperties.hasProperty("position"))
            mProperties.setValue("position", val);
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::setDerivedOrientation(Ogre::Quaternion val)
    {
        if(getParent())
        {
            val = getParent()->getDerivedOrientation().Inverse() * val; 
        }

        if(mProperties.hasProperty("orientation"))
            mProperties.setValue("orientation",val);
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::setDerivedScale(Ogre::Vector3 val)
    {
        if(getParent())
        {
            Ogre::Vector3 parentscale = getParent()->getDerivedScale();
            if(parentscale.x != 0.0f)
                val.x /= parentscale.x;
            if(parentscale.y != 0.0f)
                val.y /= parentscale.y;
            if(parentscale.z != 0.0f)
                val.z /= parentscale.z;
        }

        if(mProperties.hasProperty("scale"))
            mProperties.setValue("scale",val);
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::setSelectedImpl(bool bSelected)
    {
        showBoundingBox(bSelected);
    }
    //-----------------------------------------------------------------------------------------
    bool CBaseEditor::_setSelected(OgitorsPropertyBase* property, const bool& bSelected)
    {
        if(bSelected)
        {
            mOgitorsRoot->GetSelection()->add(this);
            if(mOBBoxRenderable) 
                adjustBoundingBox();
        }
        else
        {
            mOgitorsRoot->GetSelection()->remove(this);
        }

        setSelectedImpl(bSelected);
        return true;
    }
    //-----------------------------------------------------------------------------------------
    bool CBaseEditor::_setLoaded(OgitorsPropertyBase* property, const bool& bLoaded)
    {
        mSystem->SetTreeItemColour(this, getTextColourInt());

        return true;
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::showHelper(bool show)
    {
        if(!usesHelper() || !mHelper) 
            return;

        bool globalVisibility = true;

        if(getEditorType() == ETYPE_LIGHT)
            globalVisibility = mOgitorsRoot->GetLightVisiblity();
        else if(getEditorType() == ETYPE_CAMERA)
            globalVisibility = mOgitorsRoot->GetCameraVisiblity();

        mHelper->Show(show && globalVisibility);
    }
    //-----------------------------------------------------------------------------------------
    bool CBaseEditor::setHighlightedImpl(bool highlight )
    {
        showBoundingBox(highlight);
        return true;
    }
    //-----------------------------------------------------------------------------------------
    bool CBaseEditor::_setHighlighted(OgitorsPropertyBase* property, const bool& highlight )
    {
        if(!mLoaded->get())
            return false;

        return setHighlightedImpl(highlight);
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::createBoundingBox()
    {
        if(!mLoaded->get())
            return;

        if(usesHelper())
            mBoxParentNode = mHelper->getNode()->createChildSceneNode("scbno" + mName->get(),Vector3(0,0,0),Quaternion::IDENTITY);
        else
            mBoxParentNode = getNode()->createChildSceneNode("scbno" + mName->get(),Vector3(0,0,0),Quaternion::IDENTITY);

        mOBBoxData = getAABB();

        mBBoxNode = mBoxParentNode->createChildSceneNode("scbnb" + mName->get(),Vector3(0,0,0),Quaternion::IDENTITY);
        mOBBoxRenderable = OGRE_NEW OBBoxRenderable();
        mOBBoxRenderable->setupVertices(mOBBoxData);
        mBBoxNode->attachObject(mOBBoxRenderable); 
        mBBoxNode->setVisible(false);
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::showBoundingBox(bool bShow) 
    {
        if(!mBoxParentNode && getEditorType() != ETYPE_BASE)
            createBoundingBox();

        if(mBBoxNode)
        {
            int matpos = 0;
            if(mHighlighted->get())
            {
                ++matpos;
                if(mSelected->get())
                    ++matpos;
            }

            mOBBoxRenderable->setMaterial(mOBBMaterials[matpos]);
            mBBoxNode->setVisible(bShow || mSelected->get() || mHighlighted->get());
        }
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::destroyBoundingBox()
    {
        if(!mOBBoxRenderable) 
            return;

        mOBBoxRenderable->detachFromParent();
        OGRE_DELETE mOBBoxRenderable;
        mOBBoxRenderable = 0;

        mBoxParentNode->removeAndDestroyChild(mBBoxNode->getName());
        mBBoxNode = 0;

        mBoxParentNode->getParentSceneNode()->removeAndDestroyChild(mBoxParentNode->getName());
        mBoxParentNode = 0;
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::adjustBoundingBox()
    {
        if(mOBBoxRenderable && mOBBoxData != getAABB())
        {
            mOBBoxRenderable->setupVertices(getAABB());
        }
    }
    //-----------------------------------------------------------------------------------------
    bool CBaseEditor::_setParent(OgitorsPropertyBase* property, const unsigned long& value)
    {
        CBaseEditor *oldparent = static_cast<const OgitorsParentProperty*>(property)->getOld();

        if(oldparent)
        {
            if(mBoxParentNode && !usesHelper()) 
                mBoxParentNode->getParentSceneNode()->removeChild(mBoxParentNode);

            if(usesHelper() && mHelper)
                mHelper->getNode()->getParentSceneNode()->removeChild(mHelper->getNode());

            oldparent->_removeChild(mName->get());
        }

        setParentImpl(oldparent, (CBaseEditor*)value);

        if(value)
        {    
            ((CBaseEditor*)value)->_addChild(this);

            if(mBoxParentNode && !usesHelper()) 
                getNode()->addChild(mBoxParentNode);

            if(usesHelper() && mHelper)
                getNode()->addChild(mHelper->getNode());

            adjustBoundingBox();
        }

        if(value)
            mSystem->MoveTreeItem(mParentEditor->get()->getSceneTreeItemHandle(), getSceneTreeItemHandle());
        else
            mSystem->MoveTreeItem(0, getSceneTreeItemHandle());

        return true;
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::registerObjectName()
    {
        mOgitorsRoot->RegisterObjectName(mName->get(), this);
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::unRegisterObjectName()
    {
        mOgitorsRoot->UnRegisterObjectName(mName->get(), this);
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::registerForUpdates()
    {
        mOgitorsRoot->RegisterForUpdates(this);
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::unRegisterForUpdates()
    {
        mOgitorsRoot->UnRegisterForUpdates(this);
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::registerForScriptUpdates()
    {
        mOgitorsRoot->RegisterForScriptUpdates(this);
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::unRegisterForScriptUpdates()
    {
        mOgitorsRoot->UnRegisterForScriptUpdates(this);
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditor::registerForPostSceneUpdates()
    {
        mOgitorsRoot->RegisterForPostSceneUpdates(this);
    }
    //-----------------------------------------------------------------------------------------
    bool CBaseEditor::setNameImpl(Ogre::String name)
    {
        mSystem->SetTreeItemText(this, name);

        if(mLoaded->get())
        {
            // Init name to old name before unLoading
            mName->init(mName->getOld());
            unLoadAllChildren();
            unLoad();
            // Init name to new name before loading
            mName->init(name);
            load();
            loadAllChildren(false);
        }

        return true;
    }
    //-----------------------------------------------------------------------------------------
    bool CBaseEditor::_setName(OgitorsPropertyBase* property, const Ogre::String& value)
    {
        Ogre::String name = value;
        Ogre::StringUtil::trim(name);
        if(name == mName->getOld() || name.empty()) 
            return false;

        if(mOgitorsRoot->FindObject(name) != 0)
            return false;

        bool selected = mSelected->get();
        bool ret = setNameImpl(name);

        if(ret)
        {
            mOgitorsRoot->UnRegisterObjectName(mName->getOld(), this); 
            mOgitorsRoot->RegisterObjectName(mName->get(), this); 

            if(mParentEditor->get())
            {
                mParentEditor->get()->_removeChild(mName->getOld());
                mParentEditor->get()->_addChild(this);
            }

            if(selected)
                showBoundingBox(true);
        }

        return ret;
    }
    //-----------------------------------------------------------------------------------------
    bool CBaseEditor::setLayerImpl(unsigned int newlayer)
    {
        return false;
    }
    //-----------------------------------------------------------------------------------------
    bool CBaseEditor::_setLayer(OgitorsPropertyBase* property, const unsigned int& value)
    {
        if(setLayerImpl(value))
        {
            mSystem->MoveLayerTreeItem(value, this);
            return true;
        }

        return false;
    }
    //-----------------------------------------------------------------------------------------
    bool CBaseEditor::_setLocked(OgitorsPropertyBase* property, const bool& lock)
    {
        mSystem->SetTreeItemColour(this, getTextColourInt());

        NameObjectPairList::iterator it = mChildren.begin();
        while(it != mChildren.end())
        {
            it->second->getProperties()->setValue("locked", lock);
            it++;
        }
        return true;
    }
    //-----------------------------------------------------------------------------------------
    bool CBaseEditor::_setShowHelper(OgitorsPropertyBase* property, const bool& bShowHelper)
    {
        mHelper->Show(bShowHelper);
        return true;
    }
    //-----------------------------------------------------------------------------------------
    void *CBaseEditor::getCustomToolsWindow() 
    { 
        return mFactory->mToolsWindow; 
    }
    //-----------------------------------------------------------------------------------------
    void *CBaseEditor::getPropertyEditorToolWindow() 
    { 
        return mFactory->mPropertyEditorToolWindow; 
    }
    //-----------------------------------------------------------------------------------------
    bool CBaseEditor::usesGizmos()
    {
        return getFactoryDynamic()->mUsesGizmos;
    }
    //-----------------------------------------------------------------------------------------
    bool CBaseEditor::usesHelper()
    {
        return getFactoryDynamic()->mUsesHelper;
    }
    //-----------------------------------------------------------------------------------------

    //-----------------------------------------------------------------------------------------
    // CBaseEditorFactory
    //-----------------------------------------------------------------------------------------
    CBaseEditorFactory::CBaseEditorFactory(OgitorsView *view) : 
    mView(view), mTypeID(0), mTypeName(""), mEditorType(ETYPE_BASE),
        mAddToObjectList(false), mRequirePlacement(false), 
        mIcon(""), mCapabilities(0), mInstanceCount(0), mDefaultWorldSection(SECT_GENERAL),
        mToolsWindow(0), mPropertyEditorToolWindow(0), mUsesHelper(0), mUsesGizmos(0)
    {
        AddPropertyDefinition("object_id",      "", "The unique ID of Object.", PROP_UNSIGNED_INT, false, false, false);
        AddPropertyDefinition("name", "Name",   "The name of the Object.", PROP_STRING, true, true);
        OgitorsPropertyDef *definition = AddPropertyDefinition("layer", "Layer", "The layer this Object belongs to.", PROP_INT, true, true);
        definition->setOptions(OgitorsRoot::GetLayerNames());
        AddPropertyDefinition("typename",       "Type Name", "The type name of the object.", PROP_STRING, true, false);
        AddPropertyDefinition("parent",         "", "The pointer to parent Object.",PROP_UNSIGNED_LONG, false, false);
        AddPropertyDefinition("selected",       "", "Is the Object selected?",PROP_BOOL, false, false, false);
        AddPropertyDefinition("highlighted",    "", "Is the Object highlighted?",PROP_BOOL, false, false, false);
        AddPropertyDefinition("loaded",         "", "Is the Object loaded?",PROP_BOOL, false, false, false);
        AddPropertyDefinition("locked",         "", "Is the Object locked?",PROP_BOOL, false, false);
        AddPropertyDefinition("modified",       "", "Is the Object modified?",PROP_BOOL, false, false);
        AddPropertyDefinition("destroyed",      "", "Is the Object destroyed?",PROP_BOOL, false, false);
        AddPropertyDefinition("show_helper",    "Show Helper", "Should the visual helper be rendered?", PROP_BOOL, true, true);
        definition = AddPropertyDefinition("updatescript", "Update Script", "The Script to be called during update.", PROP_STRING, false, false);
        definition->setOptions(OgitorsRoot::GetScriptNames());
    }
    //-----------------------------------------------------------------------------------------
    CBaseEditorFactory *CBaseEditorFactory::duplicate(OgitorsView *view)
    {
        CBaseEditorFactory *ret = OGRE_NEW CBaseEditorFactory(view);
        ret->mTypeID = mTypeID;

        return ret;
    }
    //-----------------------------------------------------------------------------------------
    CBaseEditor *CBaseEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
    {
        mInstanceCount++;
        CBaseEditor *object = OGRE_NEW CBaseEditor(this);

        object->createProperties(params);
        object->mParentEditor->init(0);
        return object;
    }
    //-----------------------------------------------------------------------------------------
    void CBaseEditorFactory::DestroyObject(CBaseEditor *object)
    {
        object->unLoad();
        object->destroyAllChildren();
        if(object->mName->get() != "") 
            OgitorsRoot::getSingletonPtr()->UnRegisterObjectName(object->mName->get(), object);
        OGRE_DELETE object;
        mInstanceCount--;
    }
    //-----------------------------------------------------------------------------------------
    CBaseEditor *CBaseEditorFactory::CloneObject(CBaseEditor **parent, CBaseEditor *object)
    {
        OgitorsPropertyValueMap params;
        object->getPropertyMap(params);
        params["init"] = EMPTY_PROPERTY_VALUE;
        return CreateObject(parent, params);
    }
    //-----------------------------------------------------------------------------------------
    OgitorsPropertyDef *CBaseEditorFactory::AddPropertyDefinition(const Ogre::String& name, const Ogre::String& displayname, const Ogre::String& desc, OgitorsPropertyType pType, bool read, bool write, bool trackchanges)
    {
        OgitorsPropertyDefMap::iterator defi = mPropertyDefs.find(name);
        if(defi == mPropertyDefs.end())
        {
            defi = mPropertyDefs.insert(OgitorsPropertyDefMap::value_type(name, OgitorsPropertyDef(name, displayname, desc, pType, read, write, trackchanges))).first;
        }
        return &(defi->second);
    }
    //-----------------------------------------------------------------------------------------
    OgitorsPropertyDef *CBaseEditorFactory::GetPropertyDefinition(const Ogre::String& name)
    {
        OgitorsPropertyDefMap::iterator defi = mPropertyDefs.find(name);
        if(defi != mPropertyDefs.end())
        {
            return &(defi->second);
        }
        return 0;
    }
    //-----------------------------------------------------------------------------------------
    Ogre::String CBaseEditorFactory::CreateUniqueID(const Ogre::String& name)
    {
        int seed = 0;

        OgitorsRoot *ogRoot = OgitorsRoot::getSingletonPtr();

        Ogre::String tail =  "#" + Ogre::StringConverter::toString(seed);
        while(ogRoot->FindObject(name + tail))
        {
            tail = "#" +  Ogre::StringConverter::toString(++seed);
        }

        return (name + tail);
    }
    //-----------------------------------------------------------------------------------------

}
